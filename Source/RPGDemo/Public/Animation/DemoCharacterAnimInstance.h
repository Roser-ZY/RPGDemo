// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Character/CharacterTypes.h"
#include "DemoCharacterAnimInstance.generated.h"

class UCharacterMovementComponent;
class ADemoCharacter;
/**
 *
 */
UCLASS()
class RPGDEMO_API UDemoCharacterAnimInstance : public UAnimInstance {
    GENERATED_BODY()

public:
    virtual void NativeInitializeAnimation() override;
    virtual void NativeUpdateAnimation(float DeltaSeconds) override;

    UPROPERTY(BlueprintReadOnly)
    ADemoCharacter* demo_character = nullptr;
    UPROPERTY(BlueprintReadOnly)
    UCharacterMovementComponent* character_movement_component = nullptr;
    UPROPERTY(BlueprintReadOnly)
    float ground_speed = 0.0f;
    UPROPERTY(BlueprintReadOnly)
    bool is_falling = false;

    UPROPERTY(BlueprintReadOnly)
    ECharacterState character_state = ECharacterState::Unequipped;
    UPROPERTY(BlueprintReadOnly)
    ECharacterActionState character_action_state = ECharacterActionState::Unoccupied;
};
