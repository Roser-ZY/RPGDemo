// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BaseCharacter.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Item/Weapon.h"

// Sets default values
ABaseCharacter::ABaseCharacter()
{
    // Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need
    // it.
    PrimaryActorTick.bCanEverTick = true;

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
