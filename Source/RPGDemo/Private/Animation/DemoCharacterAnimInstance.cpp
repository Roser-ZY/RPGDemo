// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/DemoCharacterAnimInstance.h"


#include "Character/DemoCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UDemoCharacterAnimInstance::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation();

    demo_character = Cast<ADemoCharacter>(TryGetPawnOwner());
    if (demo_character != nullptr) {
        character_movement_component = demo_character->GetCharacterMovement();
    }
}

void UDemoCharacterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
    Super::NativeUpdateAnimation(DeltaSeconds);

    if (demo_character == nullptr || character_movement_component == nullptr) {
        return;
    }
    // Get the xy length of the velocity.
    ground_speed = UKismetMathLibrary::VSizeXY(character_movement_component->Velocity);
    is_falling = character_movement_component->IsFalling();
    character_state = demo_character->get_current_state();
}
