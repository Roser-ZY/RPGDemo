// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterTypes.h"
#include "GameFramework/Character.h"
#include "Interfaces/HitInterface.h"
#include "Enemy.generated.h"


class AAIController;
enum class EEnemyDeathPose : uint8;
class UHealthBarComponent;
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
    void calculateHitDirection(const FVector& impact_point) const;

    virtual void getHit_Implementation(const FVector& impact_point) override;

    virtual float TakeDamage(float DamageAmount, const struct FDamageEvent& DamageEvent,
                             class AController* EventInstigator, AActor* DamageCauser) override;

    bool inTargetRange(AActor* target, double radius);

    void patrolTimeFinished();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

    void checkCombatTarget();
    void checkPatrolTarget();

    void moveToTarget(AActor* target);
    AActor* choosePatrolTarget();

    UPROPERTY(EditAnywhere, Category = "Animation")
    UAnimMontage* hit_react_montage_ = nullptr;

    UPROPERTY(EditAnywhere, Category = "Animation")
    UAnimMontage* death_montage_ = nullptr;

    UPROPERTY(VisibleAnywhere, Category = "Attribute")
    TObjectPtr<UAttributeComponent> attribute_component_;

    UPROPERTY(VisibleAnywhere, Category = "HUD")
    TObjectPtr<UHealthBarComponent> health_bar_widget_;

    UPROPERTY(EditAnywhere, Category = "VisualEffects")
    UParticleSystem* hit_particle_system_ = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    EEnemyDeathPose death_pose_ = EEnemyDeathPose::EEDP_Alive;

    UPROPERTY()
    TObjectPtr<AActor> combat_target_;

    UPROPERTY(EditAnywhere)
    double combat_radius_ = 500.0f;

    /**
     * Navigation.
     */
    TObjectPtr<AAIController> enemy_ai_controller_;

    UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "AI Navigation", meta = (AllowPrivateAccess = true))
    TObjectPtr<AActor> patrol_target_ = nullptr;
    UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
    TArray<TObjectPtr<AActor>> patrol_targets_;
    UPROPERTY(EditAnywhere, Category = "AI Navigation")
    double patrol_radius_ = 200.0f;

    FTimerHandle patrol_timer_;
    UPROPERTY(EditAnywhere, Category = "AI Navigation")
    float wait_min_ = 5.0f;
    float wait_max_ = 10.0f;
};
