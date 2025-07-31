// Fill out your copyright notice in the Description page of Project Settings.



#include "Character/Enemy.h"

#include "AIController.h"
#include "NiagaraFunctionLibrary.h"
#include "Character/DemoCharacter.h"
#include "Component/AttributeComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Perception/PawnSensingComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "HUD/HealthBarComponent.h"
#include "Item/Soul.h"
#include "Item/Weapon.h"
#include "Kismet/GameplayStatics.h"
#include "Navigation/PathFollowingComponent.h"

// Sets default values
AEnemy::AEnemy()
{
    // Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need
    // it.
    PrimaryActorTick.bCanEverTick = true;

    // Set the attributes of collision.
    initCollision();

    disableRotationWithController();
    rotateToMovementDirection();

    // The attribute component do not need attach to any component.
    health_bar_widget_ = CreateDefaultSubobject<UHealthBarComponent>(TEXT("Health Bar"));
    health_bar_widget_->SetupAttachment(RootComponent);

    // Pawn sensing component.
    pawn_sensing_component_ = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensing"));
    pawn_sensing_component_->SightRadius = 4000.0f;
    pawn_sensing_component_->SetPeripheralVisionAngle(45.0f);
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
    Super::BeginPlay();

    Tags.Add(FName("Enemy"));

    if (health_bar_widget_) {
        health_bar_widget_->SetVisibility(false);
    }

    enemy_ai_controller_ = Cast<AAIController>(GetController());
    moveToTarget(patrol_target_);

    if (pawn_sensing_component_) {
        pawn_sensing_component_->OnSeePawn.AddDynamic(this, &AEnemy::pawnSeen);
    }

    UWorld* world = GetWorld();
    if (world && weapon_class_) {
        AWeapon* weapon = world->SpawnActor<AWeapon>(weapon_class_);
        if (weapon) {
            weapon->equipTo(GetMesh(), FName("left_hand_socket"), this, this);
            equipped_weapon_ = weapon;
        }
    }
}

void AEnemy::moveToTarget(AActor* target)
{
    if (enemy_state_ == EEnemyState::EES_Dead || target == nullptr || enemy_ai_controller_ == nullptr) {
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
    const bool enemy_should_chase_target =
        enemy_state_ != EEnemyState::EES_Chasing && enemy_state_ != EEnemyState::EES_Engaged &&
        enemy_state_ != EEnemyState::EES_Attacking && enemy_state_ != EEnemyState::EES_Dead &&
        pawn->ActorHasTag(FName("EngageableTarget"));

    if (enemy_should_chase_target) {
        // Clear the timer to avoid patrol when the enemy is chasing.
        clearPatrolTimer();
        combat_target_ = pawn;

        chaseCombatTarget();
        UE_LOG(LogTemp, Warning, TEXT("See the pawn and now chasing."));
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

void AEnemy::Destroyed()
{
    Super::Destroyed();

    if (equipped_weapon_) {
        equipped_weapon_->Destroy();
    }
}

void AEnemy::checkCombatTarget()
{
    if (enemy_state_ == EEnemyState::EES_Dead) {
        return;
    }

    bool targetIsOutsideCombatRadius = !inTargetRange(combat_target_, combat_radius_);
    bool targetIsOutsideAttackRadius = !inTargetRange(combat_target_, attack_radius_);
    bool targetIsInsideAttackRadius = inTargetRange(combat_target_, attack_radius_);

    if (combat_target_ == nullptr || (targetIsOutsideCombatRadius && enemy_state_ != EEnemyState::EES_Patrolling)) {
        // Outside the target range, lose interest.
        loseInterest();
        hideHealthBar();
        clearAttackTimer();
        if (enemy_state_ != EEnemyState::EES_Engaged) {
            startPatrolling();
        }
    } else if (targetIsOutsideAttackRadius) {
        // Outside attack range and inside combat range, chase character.
        clearAttackTimer();
        if (enemy_state_ != EEnemyState::EES_Engaged) {
            chaseCombatTarget();
        }
    } else if (targetIsInsideAttackRadius && enemy_state_ != EEnemyState::EES_Attacking &&
               enemy_state_ != EEnemyState::EES_Engaged) {
        clearAttackTimer();
        startAttackTimer();
    }
}
void AEnemy::checkPatrolTarget()
{
    if (enemy_state_ == EEnemyState::EES_Dead) {
        return;
    }
    if (inTargetRange(patrol_target_, patrol_radius_)) {
        patrol_target_ = choosePatrolTarget();
        const float wait_time = FMath::RandRange(patrol_wait_min_, patrol_wait_max_);
        GetWorldTimerManager().SetTimer(patrol_timer_, this, &AEnemy::patrolTimeFinished, wait_time);
    }
}

void AEnemy::hideHealthBar()
{
    if (health_bar_widget_) {
        health_bar_widget_->SetVisibility(false);
    }
}

void AEnemy::showHealthBar()
{
    if (health_bar_widget_) {
        health_bar_widget_->SetVisibility(true);
    }
}

void AEnemy::loseInterest()
{
    combat_target_ = nullptr;
}

void AEnemy::startPatrolling()
{
    enemy_state_ = EEnemyState::EES_Patrolling;
    GetCharacterMovement()->MaxWalkSpeed = patrol_speed_;
    moveToTarget(patrol_target_);
}

void AEnemy::clearPatrolTimer()
{
    GetWorldTimerManager().ClearTimer(patrol_timer_);
}

void AEnemy::startAttackTimer()
{
    enemy_state_ = EEnemyState::EES_Attacking;
    const float attack_time = FMath::RandRange(attack_min_, attack_max_);
    GetWorldTimerManager().SetTimer(attack_timer_, this, &AEnemy::attack, attack_time);
}

void AEnemy::clearAttackTimer()
{
    GetWorldTimerManager().ClearTimer(attack_timer_);
}

void AEnemy::attack()
{
    if (combat_target_ == nullptr || enemy_state_ == EEnemyState::EES_Dead) {
        return;
    }

    if (combat_target_->ActorHasTag(FName("Dead"))) {
        combat_target_ = nullptr;
        return;
    }

    enemy_state_ = EEnemyState::EES_Engaged;
    playMontage(attack_montage_);
}

void AEnemy::end_attack()
{
    enemy_state_ = EEnemyState::EES_NoState;
    checkCombatTarget();
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (enemy_state_ == EEnemyState::EES_Dead) {
        return;
    }

    if (enemy_state_ == EEnemyState::EES_Patrolling) {
        checkPatrolTarget();
    } else {
        checkCombatTarget();
    }
}

void AEnemy::getHit_Implementation(const FVector& impact_point, AActor* hitter)
{
    if (enemy_state_ == EEnemyState::EES_Dead) {
        return;
    }

    UE_LOG(LogTemp, Warning, TEXT("The enemy get hit."));
    showHealthBar();
    clearPatrolTimer();
    clearAttackTimer();
    stopMontage(attack_montage_);
    spawnHitParticles(impact_point);
    setWeaponCollisionEnabled(ECollisionEnabled::NoCollision);

    if (attribute_component_ == nullptr) {
        return;
    }

    // Todo: These codes should extract to a function.
    if (attribute_component_->isAlive()) {
        calculateHitDirection(impact_point);
        playMontage(hit_react_montage_);
    } else {
        playMontage(death_montage_);
        death_pose_ = EEnemyDeathPose::EEDP_Death;
        enemy_state_ = EEnemyState::EES_Dead;

        UWorld* world = GetWorld();
        if (world) {
            ASoul* spawned_soul = world->SpawnActor<ASoul>(soul_class_, GetActorLocation(), GetActorRotation());
            if (spawned_soul && attribute_component_) {
                spawned_soul->setSoul(attribute_component_->getSoul());

                FTimerHandle enable_soul_timer;
                GetWorldTimerManager().SetTimer(enable_soul_timer, spawned_soul, &ASoul::enablePickup, 3.0f);
            }
        }

        // Redress after death.
        hideHealthBar();
        SetLifeSpan(3.0f);
    }
}

float AEnemy::TakeDamage(float DamageAmount, const struct FDamageEvent& DamageEvent, class AController* EventInstigator,
                         AActor* DamageCauser)
{
    if (attribute_component_ == nullptr || enemy_state_ == EEnemyState::EES_Dead) {
        return 0.0f;
    }

    if (health_bar_widget_) {
        attribute_component_->receiveDamage(DamageAmount);
        health_bar_widget_->setHealthPercentage(attribute_component_->getHealthPercentage());
    }

    if (attribute_component_->isAlive()) {
        combat_target_ = EventInstigator->GetPawn();
        chaseCombatTarget();
    }
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


void AEnemy::initCollision()
{
    USkeletalMeshComponent* skeletal_mesh = GetMesh();
    if (skeletal_mesh) {
        skeletal_mesh->SetCollisionObjectType(ECC_WorldDynamic);
        skeletal_mesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
        skeletal_mesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
        skeletal_mesh->SetGenerateOverlapEvents(true);
    }
}
