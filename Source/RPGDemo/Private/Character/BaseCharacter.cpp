// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BaseCharacter.h"

#include "Component/AttributeComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Item/Weapon.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ABaseCharacter::ABaseCharacter()
{
    // Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need
    // it.
    PrimaryActorTick.bCanEverTick = true;
    attribute_component_ = CreateDefaultSubobject<UAttributeComponent>(TEXT("Attribute"));

    UCapsuleComponent* capsule_component = GetCapsuleComponent();
    if (capsule_component) {
        capsule_component->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
    }
}

// Called when the game starts or when spawned
void ABaseCharacter::BeginPlay()
{
    Super::BeginPlay();
}

void ABaseCharacter::attack()
{
}

void ABaseCharacter::end_attack()
{
}

// Called every frame
void ABaseCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void ABaseCharacter::setWeaponCollisionEnabled(ECollisionEnabled::Type collision_enabled)
{
    if (equipped_weapon_) {
        equipped_weapon_->clearIgnoredActors();
        equipped_weapon_->setWeaponCollisionEnabled(collision_enabled);
    }
}

void ABaseCharacter::calculateHitDirection(const FVector& impact_point) const
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

float ABaseCharacter::TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent, AController* EventInstigator,
                                 AActor* DamageCauser)
{
    return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

bool ABaseCharacter::playMontage(UAnimMontage* montage, FName section_name)
{
    if (montage == nullptr) {
        UE_LOG(LogTemp, Error, TEXT("The attack montage is not ready!"));
        return false;
    }

    USkeletalMeshComponent* skeletal_mesh_component = GetMesh();
    if (skeletal_mesh_component == nullptr) {
        UE_LOG(LogTemp, Error, TEXT("The skeletal mesh is not ready!"));
        return false;
    }

    UAnimInstance* anim_instance = Cast<UAnimInstance>(skeletal_mesh_component->GetAnimInstance());
    if (anim_instance == nullptr) {
        UE_LOG(LogTemp, Error, TEXT("The animation of the skeletal mesh is not ready!"));
        return false;
    }

    float seconds = anim_instance->Montage_Play(montage);
    if (seconds <= 0.0f) {
        UE_LOG(LogTemp, Error, TEXT("Failed to play the attack montage."));
        return false;
    }
    if (!section_name.IsNone()) {
        anim_instance->Montage_JumpToSection(section_name);
    }
    return true;
}

void ABaseCharacter::stopMontage(UAnimMontage* montage)
{
    if (montage == nullptr) {
        return;
    }

    UAnimInstance* anim_instance = GetMesh()->GetAnimInstance();
    if (anim_instance) {
        anim_instance->Montage_Stop(0.25f, montage);
    }
}

void ABaseCharacter::disableRotationWithController()
{
    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;
}

void ABaseCharacter::rotateToMovementDirection()
{
    UCharacterMovementComponent* character_movement = GetCharacterMovement();
    character_movement->bOrientRotationToMovement = true;
    character_movement->RotationRate = FRotator(0.0f, 400.0f, 0.0f);
}

void ABaseCharacter::spawnHitParticles(const FVector& impact_point)
{
    // Show the hit particle.
    if (hit_particle_system_) {
        UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), hit_particle_system_, impact_point);
    }
}

FVector ABaseCharacter::getTranslationWarpTarget()
{
    if (combat_target_ == nullptr) {
        return FVector::ZeroVector;
    }

    const FVector combat_target_location = combat_target_->GetActorLocation();
    const FVector location = GetActorLocation();

    FVector target_to_me = (location - combat_target_location).GetSafeNormal();
    target_to_me *= warp_target_distance_;
    return combat_target_location + target_to_me;
}

FVector ABaseCharacter::getRotationWarpTarget()
{
    if (combat_target_) {
        return combat_target_->GetActorLocation();
    }
    return FVector::ZeroVector;
}
