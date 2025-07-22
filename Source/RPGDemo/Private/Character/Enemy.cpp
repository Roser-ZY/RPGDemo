// Fill out your copyright notice in the Description page of Project Settings.



#include "Character/Enemy.h"

#include "AIController.h"
#include "Component/AttributeComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Perception/PawnSensingComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "HUD/HealthBarComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Navigation/PathFollowingComponent.h"

// Sets default values
AEnemy::AEnemy()
{
    // Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need
    // it.
    PrimaryActorTick.bCanEverTick = true;

    // Set the attributes of collision.
    USkeletalMeshComponent* skeletal_mesh = GetMesh();
    if (skeletal_mesh) {
        skeletal_mesh->SetCollisionObjectType(ECC_WorldDynamic);
        skeletal_mesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
        skeletal_mesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
        skeletal_mesh->SetGenerateOverlapEvents(true);
        UCapsuleComponent* capsule_component = GetCapsuleComponent();
        if (capsule_component) {
            capsule_component->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
        }
    }

    // The attribute component do not need attach to any component.
    attribute_component_ = CreateDefaultSubobject<UAttributeComponent>(TEXT("Attribute"));
    health_bar_widget_ = CreateDefaultSubobject<UHealthBarComponent>(TEXT("Health Bar"));
    health_bar_widget_->SetupAttachment(RootComponent);

    GetCharacterMovement()->bOrientRotationToMovement = true;
    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;

    // Pawn sensing component.
    pawn_sensing_component_ = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensing"));
    pawn_sensing_component_->SightRadius = 4000.0f;
    pawn_sensing_component_->SetPeripheralVisionAngle(45.0f);
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
    Super::BeginPlay();

    if (health_bar_widget_) {
        health_bar_widget_->SetVisibility(false);
    }

    enemy_ai_controller_ = Cast<AAIController>(GetController());
    moveToTarget(patrol_target_);

    if (pawn_sensing_component_) {
        pawn_sensing_component_->OnSeePawn.AddDynamic(this, &AEnemy::pawnSeen);
    }
}

void AEnemy::moveToTarget(AActor* target)
{
    if (target == nullptr || enemy_ai_controller_ == nullptr) {
        return;
    }

    FAIMoveRequest move_request;
    move_request.SetGoalActor(target);
    move_request.SetAcceptanceRadius(15.0f);
    enemy_ai_controller_->MoveTo(move_request);
}

AActor* AEnemy::choosePatrolTarget()
{
    if (patrol_target_) {
        const int32 num_patrol_targets = patrol_targets_.Num();
        if (num_patrol_targets > 0) {
            TObjectPtr<AActor> new_target = patrol_target_;
            do {
                const int32 target_selection = FMath::RandRange(0, num_patrol_targets - 1);
                new_target = patrol_targets_[target_selection];
            } while (new_target == patrol_target_);
            return new_target;
        }
    }
    return patrol_target_;
}

void AEnemy::pawnSeen(APawn* pawn)
{
    if (enemy_state_ == EEnemyState::EES_Chasing) {
        return;
    }

    if (pawn->ActorHasTag(FName("DemoCharacter"))) {
        // Clear the timer to avoid patrol when the enemy is chasing.
        GetWorldTimerManager().ClearTimer(patrol_timer_);
        combat_target_ = pawn;
        if (enemy_state_ != EEnemyState::EES_Attacking) {
            chaseCombatTarget();
            UE_LOG(LogTemp, Warning, TEXT("See the pawn and now chasing."));
        }
    }
}

void AEnemy::patrolTimeFinished()
{
    moveToTarget(patrol_target_);
}

void AEnemy::chaseCombatTarget()
{
    enemy_state_ = EEnemyState::EES_Chasing;
    GetCharacterMovement()->MaxWalkSpeed = 300.0f;
    moveToTarget(combat_target_);
}
void AEnemy::checkCombatTarget()
{
    if (!inTargetRange(combat_target_, combat_radius_)) {
        // Outside the target range, lose interest.
        combat_target_ = nullptr;
        if (health_bar_widget_) {
            health_bar_widget_->SetVisibility(false);
        }

        enemy_state_ = EEnemyState::EES_Patrolling;
        GetCharacterMovement()->MaxWalkSpeed = 150.0f;
        moveToTarget(patrol_target_);
        UE_LOG(LogTemp, Warning, TEXT("Lose interest."));
    } else if (!inTargetRange(combat_target_, attack_radius_) && enemy_state_ != EEnemyState::EES_Chasing) {
        // Outside attack range and inside combat range, chase character.
        chaseCombatTarget();
        UE_LOG(LogTemp, Warning, TEXT("Chase player."));
    } else if (inTargetRange(combat_target_, attack_radius_) && enemy_state_ != EEnemyState::EES_Attacking) {
        enemy_state_ = EEnemyState::EES_Attacking;
        // Todo: Attack.
        UE_LOG(LogTemp, Warning, TEXT("Attack player."));
    }
}
void AEnemy::checkPatrolTarget()
{
    if (inTargetRange(patrol_target_, patrol_radius_)) {
        patrol_target_ = choosePatrolTarget();
        const float wait_time = FMath::RandRange(wait_min_, wait_max_);
        GetWorldTimerManager().SetTimer(patrol_timer_, this, &AEnemy::patrolTimeFinished, wait_time);
    }
}
// Called every frame
void AEnemy::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (enemy_state_ == EEnemyState::EES_Patrolling) {
        checkPatrolTarget();
    } else {
        checkCombatTarget();
    }
}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AEnemy::calculateHitDirection(const FVector& impact_point) const
{
    // Calculate the hit position of the enemy.
    const FVector forward = GetActorForwardVector();
    const FVector impact_horizontal_with_actor(impact_point.X, impact_point.Y, GetActorLocation().Z);
    const FVector to_hit = (impact_horizontal_with_actor - GetActorLocation()).GetSafeNormal();

    // forward * to_hit = |forward||to_hit| * cos(theta)
    // |forward| = 1, |to_hit| = 1, so forward * to_hit = cos(theta)
    const double cos_theta = FVector::DotProduct(forward, to_hit);
    // Take the inverse cosine (arc - cosine) of cos(theta) to get theta.
    double theta = FMath::Acos(cos_theta);
    // Convert from radians to degrees.
    theta = FMath::RadiansToDegrees(theta);

    // If cross_product points down, theta should be negative.
    const FVector cross_product = FVector::CrossProduct(forward, to_hit);
    if (cross_product.Z < 0) {
        theta *= -1.f;
    }

    if (GEngine) {
        GEngine->AddOnScreenDebugMessage(1, 5.f, FColor::Green, FString::Printf(TEXT("Theta: %f"), theta), true);
    }
}

void AEnemy::getHit_Implementation(const FVector& impact_point)
{
    UE_LOG(LogTemp, Warning, TEXT("The enemy get hit."));
    if (attribute_component_ == nullptr) {
        return;
    }
    if (health_bar_widget_) {
        health_bar_widget_->SetVisibility(true);
    }

    // Todo: These codes should extract to a function.
    if (attribute_component_->isAlive()) {
        calculateHitDirection(impact_point);

        if (!playMontage(hit_react_montage_)) {
            return;
        }

        // Show the hit particle.
        if (!hit_particle_system_) {
            return;
        }
        UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), hit_particle_system_, impact_point);
    } else {
        if (!playMontage(death_montage_)) {
            return;
        }
        death_pose_ = EEnemyDeathPose::EEDP_Death;

        // Redress after death.
        if (health_bar_widget_) {
            health_bar_widget_->SetVisibility(false);
        }
        SetLifeSpan(3.0f);
    }
}

float AEnemy::TakeDamage(float DamageAmount, const struct FDamageEvent& DamageEvent, class AController* EventInstigator,
                         AActor* DamageCauser)
{
    if (attribute_component_ && health_bar_widget_) {
        attribute_component_->receiveDamage(DamageAmount);
        health_bar_widget_->setHealthPercentage(attribute_component_->getHealthPercentage());
    }

    combat_target_ = EventInstigator->GetPawn();
    chaseCombatTarget();
    return DamageAmount;
}

bool AEnemy::inTargetRange(AActor* target, double radius)
{
    if (target == nullptr) {
        return false;
    }
    const double distance_to_target = (target->GetActorLocation() - GetActorLocation()).Size();
    return distance_to_target <= radius;
}
