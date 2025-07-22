// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "CharacterTypes.h"
#include "Interfaces/HitInterface.h"
#include "Enemy.generated.h"

class UPawnSensingComponent;
class AAIController;
enum class EEnemyDeathPose : uint8;
class UHealthBarComponent;

UCLASS()
class RPGDEMO_API AEnemy : public ABaseCharacter {
    GENERATED_BODY()

public:
    // Sets default values for this character's properties
    AEnemy();

    // Called every frame
    virtual void Tick(float DeltaTime) override;

    // Called to bind functionality to input
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
    void calculateHitDirection(const FVector& impact_point) const;

    virtual void getHit_Implementation(const FVector& impact_point) override;

    virtual float TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent, AController* EventInstigator,
                             AActor* DamageCauser) override;

    bool inTargetRange(AActor* target, double radius);

    void patrolTimeFinished();
    void chaseCombatTarget();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

    void checkCombatTarget();
    void checkPatrolTarget();

    void moveToTarget(AActor* target);
    AActor* choosePatrolTarget();

    UFUNCTION()
    void pawnSeen(APawn* pawn);

    UPROPERTY(VisibleAnywhere, Category = "HUD")
    TObjectPtr<UHealthBarComponent> health_bar_widget_;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    EEnemyDeathPose death_pose_ = EEnemyDeathPose::EEDP_Alive;

    UPROPERTY()
    TObjectPtr<AActor> combat_target_;

    UPROPERTY(EditAnywhere)
    double combat_radius_ = 500.0f;

    UPROPERTY(EditAnywhere)
    double attack_radius_ = 150.0f;

    /**
     * Navigation.
     */
    TObjectPtr<AAIController> enemy_ai_controller_;

    UPROPERTY(EditInstanceOnly, Category = "AI Navigation", meta = (AllowPrivateAccess = true))
    TObjectPtr<AActor> patrol_target_ = nullptr;
    UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
    TArray<TObjectPtr<AActor>> patrol_targets_;
    UPROPERTY(EditAnywhere, Category = "AI Navigation")
    double patrol_radius_ = 200.0f;

    FTimerHandle patrol_timer_;
    UPROPERTY(EditAnywhere, Category = "AI Navigation")
    float wait_min_ = 5.0f;
    UPROPERTY(EditAnywhere, Category = "AI Navigation")
    float wait_max_ = 10.0f;

    UPROPERTY(EditAnywhere, Category = "Pawn Sensing")
    TObjectPtr<UPawnSensingComponent> pawn_sensing_component_;

    EEnemyState enemy_state_ = EEnemyState::EES_Patrolling;
};
