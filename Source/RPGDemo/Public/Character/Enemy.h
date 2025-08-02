// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "CharacterTypes.h"
#include "Enemy.generated.h"


class ASoul;
class UPawnSensingComponent;
class AAIController;
enum class EEnemyDeathPose : uint8;
class UHealthBarComponent;

UCLASS()
class RPGDEMO_API AEnemy : public ABaseCharacter {
    GENERATED_BODY()

public:
    AEnemy();
    virtual void Tick(float DeltaTime) override;
    void spawnSoul();
    virtual float TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent, AController* EventInstigator,
                             AActor* DamageCauser) override;
    virtual void Destroyed() override;

    virtual void getHit_Implementation(const FVector& impact_point, AActor* hitter) override;

protected:
    /**/

    virtual void BeginPlay() override;

    virtual void attack() override;
    virtual void end_attack() override;
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    EEnemyDeathPose death_pose_ = EEnemyDeathPose::EEDP_Alive;
    UPROPERTY(BlueprintReadOnly)
    EEnemyState enemy_state_ = EEnemyState::EES_Patrolling;

private:
    UFUNCTION()
    void pawnSeen(APawn* pawn);

    void checkCombatTarget();
    void checkPatrolTarget();

    void patrolTimeFinished();
    void chaseCombatTarget();

    void hideHealthBar();
    void showHealthBar();

    void loseInterest();
    void startPatrolling();
    void clearPatrolTimer();

    void startAttackTimer();
    void clearAttackTimer();

    bool inTargetRange(AActor* target, double radius);
    void moveToTarget(AActor* target);
    AActor* choosePatrolTarget();

    void initCollision();


    UPROPERTY(VisibleAnywhere, Category = "HUD")
    TObjectPtr<UHealthBarComponent> health_bar_widget_;
    UPROPERTY(EditAnywhere, Category = "Pawn Sensing")
    TObjectPtr<UPawnSensingComponent> pawn_sensing_component_;
    UPROPERTY(EditAnywhere)
    TSubclassOf<AWeapon> weapon_class_;
    UPROPERTY(EditAnywhere)
    double combat_radius_ = 700.0f;
    UPROPERTY(EditAnywhere)
    double attack_radius_ = 300.0f;


    TObjectPtr<AAIController> enemy_ai_controller_;
    UPROPERTY(EditInstanceOnly, Category = "AI Navigation", meta = (AllowPrivateAccess = true))
    TObjectPtr<AActor> patrol_target_ = nullptr;
    UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
    TArray<TObjectPtr<AActor>> patrol_targets_;
    UPROPERTY(EditAnywhere, Category = "AI Navigation")
    double patrol_radius_ = 150.0f;

    FTimerHandle patrol_timer_;
    UPROPERTY(EditAnywhere, Category = "AI Navigation")
    float patrol_wait_min_ = 5.0f;
    UPROPERTY(EditAnywhere, Category = "AI Navigation")
    float patrol_wait_max_ = 10.0f;
    UPROPERTY(EditAnywhere, Category = "AI Navigation")
    float patrol_speed_ = 125.0f;

    FTimerHandle attack_timer_;
    UPROPERTY(EditAnywhere, Category = "Combat")
    float attack_min_ = 0.7f;
    UPROPERTY(EditAnywhere, Category = "Combat")
    float attack_max_ = 1.2f;
    UPROPERTY(EditAnywhere, Category = "AI Navigation")
    float chase_speed_ = 300.0f;

    UPROPERTY(EditAnywhere, Category = "Soul Property")
    TSubclassOf<ASoul> soul_class_;
};
