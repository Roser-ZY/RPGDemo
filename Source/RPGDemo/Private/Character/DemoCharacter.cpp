// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/DemoCharacter.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GroomComponent.h"
#include "Animation/DemoCharacterAnimInstance.h"
#include "Components/CapsuleComponent.h"
#include "Item/Weapon.h"

// Sets default values
ADemoCharacter::ADemoCharacter()
{
    // Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need
    // it.
    PrimaryActorTick.bCanEverTick = true;

    disableRotationWithController();
    rotateToMovementDirection();

    // Create components.
    spring_arm_component_ = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));
    spring_arm_component_->SetupAttachment(RootComponent);
    spring_arm_component_->TargetArmLength = 300.0f;
    // Let the spring arm rotate with the controller of this character.
    spring_arm_component_->bUsePawnControlRotation = true;

    camera_component_ = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    camera_component_->SetupAttachment(spring_arm_component_);

    hair_component_ = CreateDefaultSubobject<UGroomComponent>(TEXT("Hair"));
    hair_component_->SetupAttachment(GetMesh());
    hair_component_->AttachmentName = FString("head");

    eyebrows_component_ = CreateDefaultSubobject<UGroomComponent>(TEXT("Eyebrows"));
    eyebrows_component_->SetupAttachment(GetMesh());
    eyebrows_component_->AttachmentName = FString("head");
}

void ADemoCharacter::bindDelegates()
{
    UCapsuleComponent* capsule_component = GetCapsuleComponent();
    if (capsule_component) {
        capsule_component->OnComponentBeginOverlap.AddDynamic(this, &ADemoCharacter::onCapsuleBeginOverlap);
        capsule_component->OnComponentEndOverlap.AddDynamic(this, &ADemoCharacter::onCapsuleEndOverlap);
    }
}

void ADemoCharacter::addMappingContext()
{
    if (input_mapping_context_) {
        APlayerController* player_controller = Cast<APlayerController>(Controller);
        if (player_controller) {
            // Get the enhanced input subsystem and set the movement input context.
            // Note: This mapping context will be set in UE5 Editor.
            UEnhancedInputLocalPlayerSubsystem* subsystem =
                ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(player_controller->GetLocalPlayer());
            if (subsystem) {
                subsystem->AddMappingContext(input_mapping_context_, 0);
            }
        }
    } else {
        UE_LOG(LogTemp, Error, TEXT("Input mapping context is not set."));
    }
}

// Called when the game starts or when spawned
void ADemoCharacter::BeginPlay()
{
    Super::BeginPlay();

    Tags.Add(FName("EngageableTarget"));

    bindDelegates();
    addMappingContext();
}

void ADemoCharacter::move(const FInputActionValue& input_value)
{
    if (action_state_ != ECharacterActionState::Unoccupied) {
        return;
    }

    if (Controller && input_value.IsNonZero()) {
        // Get the yaw rotation value of the controller.
        // Note: GetControlRotation() will return the rotation of the controller in the world.
        // Note: The X axis is the forward direction of the controller.
        const FRotator yaw_rotation(0.0f, GetControlRotation().Yaw, 0.0f);

        const FVector forward_direction = FRotationMatrix(yaw_rotation).GetUnitAxis(EAxis::X);
        const FVector right_direction = FRotationMatrix(yaw_rotation).GetUnitAxis(EAxis::Y);

        // Move.
        const FVector2D movement_vector = input_value.Get<FVector2D>();
        AddMovementInput(forward_direction, movement_vector.Y);
        AddMovementInput(right_direction, movement_vector.X);
    }
}

void ADemoCharacter::look(const FInputActionValue& input_value)
{
    const FVector2D look_vector = input_value.Get<FVector2D>();
    AddControllerPitchInput(look_vector.Y);
    AddControllerYawInput(look_vector.X);
}

void ADemoCharacter::jump(const FInputActionValue& input_value)
{
    if (action_state_ != ECharacterActionState::Unoccupied) {
        return;
    }
    const bool is_jumping = input_value.Get<bool>();
    if (is_jumping) {
        Jump();
    }
}

void ADemoCharacter::pickUpAndEquipWeapon(AWeapon* weapon)
{
    if (weapon == nullptr) {
        return;
    }
    weapon->equipTo(GetMesh(), FName("right_hand_socket"), this, this);
    // Set the weapon's owner.
    // weapon->SetOwner(this);
    // weapon->SetInstigator(this);
    equipped_weapon_ = weapon;
    current_state_ = ECharacterState::EquippedOneHandWeapon;
    overlapping_item_ = nullptr;
}

void ADemoCharacter::toggleEquippedWeaponState()
{
    if (equipped_weapon_ == nullptr || action_state_ != ECharacterActionState::Unoccupied) {
        return;
    }

    bool canDisarm = (current_state_ == ECharacterState::EquippedOneHandWeapon);
    bool canArm = (current_state_ == ECharacterState::Unequipped);
    if (canDisarm) {
        if (playMontage(equip_montage_, FName("Unequip"))) {
            current_state_ = ECharacterState::Unequipped;
            action_state_ = ECharacterActionState::EquippingWeapon;
        }
    } else if (canArm) {
        if (playMontage(equip_montage_, FName("Equip"))) {
            current_state_ = ECharacterState::EquippedOneHandWeapon;
            action_state_ = ECharacterActionState::EquippingWeapon;
        }
    }
}
void ADemoCharacter::interact()
{
    // Equip only one weapon.
    AWeapon* weapon = Cast<AWeapon>(overlapping_item_);
    if (weapon) {
        pickUpAndEquipWeapon(weapon);
    } else if (equipped_weapon_) {
        toggleEquippedWeaponState();
    }
}

void ADemoCharacter::attack()
{
    UE_LOG(LogTemp, Warning, TEXT("Attacking."));

    bool characterCanAttack =
        current_state_ == ECharacterState::EquippedOneHandWeapon &&
        (action_state_ == ECharacterActionState::Unoccupied || action_state_ == ECharacterActionState::HitReaction);

    if (characterCanAttack && playMontage(attack_montage_)) {
        action_state_ = ECharacterActionState::Attacking;
    }
}

void ADemoCharacter::end_attack()
{
    action_state_ = ECharacterActionState::Unoccupied;
}

void ADemoCharacter::unequip()
{
    if (equipped_weapon_) {
        equipped_weapon_->attachMeshToSocket(GetMesh(), FName("SpineSwordSocket"));
    }
}

void ADemoCharacter::equip()
{
    if (equipped_weapon_) {
        equipped_weapon_->attachMeshToSocket(GetMesh(), FName("right_hand_socket"));
    }
}

void ADemoCharacter::end_equip()
{
    action_state_ = ECharacterActionState::Unoccupied;
}

void ADemoCharacter::hit_react_end()
{
    action_state_ = ECharacterActionState::Unoccupied;
}

void ADemoCharacter::onCapsuleBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                           UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                           const FHitResult& SweepResult)
{
}

void ADemoCharacter::onCapsuleEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                         UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}

// Called every frame
void ADemoCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ADemoCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    bool input_actions_are_all_ready = movement_action_ && look_action_ && jump_action_;
    if (input_actions_are_all_ready) {
        UEnhancedInputComponent* enhanced_input_component = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);
        if (enhanced_input_component) {
            enhanced_input_component->BindAction(movement_action_, ETriggerEvent::Triggered, this,
                                                 &ADemoCharacter::move);
            enhanced_input_component->BindAction(look_action_, ETriggerEvent::Triggered, this, &ADemoCharacter::look);
            enhanced_input_component->BindAction(jump_action_, ETriggerEvent::Triggered, this, &ADemoCharacter::jump);
            enhanced_input_component->BindAction(attack_action_, ETriggerEvent::Started, this, &ADemoCharacter::attack);
            enhanced_input_component->BindAction(interact_action_, ETriggerEvent::Started, this,
                                                 &ADemoCharacter::interact);
        }
    } else {
        UE_LOG(LogTemp, Error, TEXT("Input actions are not all ready."));
    }
}
void ADemoCharacter::getHit_Implementation(const FVector& impact_point, AActor* hitter)
{
    calculateHitDirection(hitter->GetActorLocation());
    setWeaponCollisionEnabled(ECollisionEnabled::NoCollision);
    spawnHitParticles(impact_point);
    playMontage(hit_react_montage_);
    action_state_ = ECharacterActionState::HitReaction;
}
