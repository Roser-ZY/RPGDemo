// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "InputActionValue.h"
#include "Camera/CameraComponent.h"
#include "CharacterTypes.h"
#include "DemoCharacter.generated.h"

class AWeapon;
class UInputMappingContext;
class UInputAction;
class UCameraComponent;
class USpringArmComponent;
class UGroomComponent;

UCLASS()
class RPGDEMO_API ADemoCharacter : public ABaseCharacter {
    GENERATED_BODY()

public:
    ADemoCharacter();

    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

    ECharacterState get_current_state()
    {
        return current_state_;
    }

    UFUNCTION(BlueprintCallable)
    FORCEINLINE void setOverlappingItem(AItem* item)
    {
        overlapping_item_ = item;
    }

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

    void move(const FInputActionValue& input_value);
    void look(const FInputActionValue& input_value);
    void jump(const FInputActionValue& input_value);
    void pickUpAndEquipWeapon(AWeapon* weapon);
    void toggleEquippedWeaponState();
    void interact();

    virtual void attack() override;
    virtual void end_attack() override;

    UFUNCTION(BlueprintCallable)
    void unequip();
    UFUNCTION(BlueprintCallable)
    void equip();
    UFUNCTION(BlueprintCallable)
    void end_equip();

    UFUNCTION()
    virtual void onCapsuleBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                       UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                       const FHitResult& SweepResult);
    UFUNCTION()
    virtual void onCapsuleEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                     UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


    UPROPERTY(EditAnywhere, Category = "Input")
    UInputMappingContext* input_mapping_context_ = nullptr;
    UPROPERTY(EditAnywhere, Category = "Input")
    TObjectPtr<UInputAction> movement_action_ = nullptr;
    UPROPERTY(EditAnywhere, Category = "Input")
    TObjectPtr<UInputAction> look_action_ = nullptr;
    UPROPERTY(EditAnywhere, Category = "Input")
    TObjectPtr<UInputAction> jump_action_ = nullptr;
    UPROPERTY(EditAnywhere, Category = "Input")
    TObjectPtr<UInputAction> attack_action_ = nullptr;
    UPROPERTY(EditAnywhere, Category = "Input")
    TObjectPtr<UInputAction> interact_action_ = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
    ECharacterState current_state_ = ECharacterState::Unequipped;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
    ECharacterActionState action_state_ = ECharacterActionState::Unoccupied;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    UAnimMontage* equip_montage_ = nullptr;

private:
    void bindDelegates();
    void addMappingContext();

    UPROPERTY(EditAnywhere, Category = "Camera")
    TObjectPtr<USpringArmComponent> spring_arm_component_ = nullptr;
    UPROPERTY(EditAnywhere, Category = "Camera")
    TObjectPtr<UCameraComponent> camera_component_ = nullptr;

    UPROPERTY(VisibleAnywhere, Category = "Appearance")
    TObjectPtr<UGroomComponent> hair_component_ = nullptr;
    UPROPERTY(VisibleAnywhere, Category = "Appearance")
    TObjectPtr<UGroomComponent> eyebrows_component_ = nullptr;

    UPROPERTY(VisibleInstanceOnly)
    TObjectPtr<AItem> overlapping_item_ = nullptr;
};
