// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/HitInterface.h"
#include "Enemy.generated.h"



class UWidgetComponent;
class UAttributeComponent;
UCLASS()
class RPGDEMO_API AEnemy : public ACharacter, public IHitInterface {
    GENERATED_BODY()

public:
    // Sets default values for this character's properties
    AEnemy();

    // Called every frame
    virtual void Tick(float DeltaTime) override;

    // Called to bind functionality to input
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
    void calculateHitDirection(const FVector& impact_point);

    virtual void getHit_Implementation(const FVector& impact_point) override;

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

    UPROPERTY(EditAnywhere, Category = "Animation")
    UAnimMontage* hit_react_montage_ = nullptr;

    UPROPERTY(VisibleAnywhere, Category = "Attribute")
    TObjectPtr<UAttributeComponent> attribute_component_;

    UPROPERTY(VisibleAnywhere, Category = "HUD")
    TObjectPtr<UWidgetComponent> health_bar_widget_;

    UPROPERTY(EditAnywhere, Category = "VisualEffects")
    UParticleSystem* hit_particle_system_ = nullptr;
};
