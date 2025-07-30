// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/HitInterface.h"
#include "BaseCharacter.generated.h"

class UAttributeComponent;
class AWeapon;

UCLASS()
class RPGDEMO_API ABaseCharacter : public ACharacter, public IHitInterface {
    GENERATED_BODY()

public:
    // Sets default values for this character's properties
    ABaseCharacter();

    // Called every frame
    virtual void Tick(float DeltaTime) override;
    virtual float TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent, AController* EventInstigator,
                             AActor* DamageCauser) override;

    UFUNCTION(BlueprintCallable)
    void setWeaponCollisionEnabled(ECollisionEnabled::Type collision_enabled);

    void calculateHitDirection(const FVector& impact_point) const;

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

    virtual void attack();
    UFUNCTION(BlueprintCallable)
    virtual void end_attack();

    bool playMontage(UAnimMontage* montage, FName seciton_name = NAME_None);
    void stopMontage(UAnimMontage* montage);

    void disableRotationWithController();
    void rotateToMovementDirection();

    void spawnHitParticles(const FVector& impact_point);

    UFUNCTION(BlueprintCallable)
    FVector getTranslationWarpTarget();
    UFUNCTION(BlueprintCallable)
    FVector getRotationWarpTarget();

    UPROPERTY(VisibleAnywhere, Category = "Attribute")
    TObjectPtr<UAttributeComponent> attribute_component_;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Equipment")
    TObjectPtr<AWeapon> equipped_weapon_ = nullptr;

    UPROPERTY(EditAnywhere, Category = "VisualEffects")
    TObjectPtr<UParticleSystem> hit_particle_system_ = nullptr;

    UPROPERTY(EditAnywhere, Category = "Animation")
    TObjectPtr<UAnimMontage> hit_react_montage_ = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    TObjectPtr<UAnimMontage> attack_montage_ = nullptr;

    UPROPERTY(EditAnywhere, Category = "Animation")
    TObjectPtr<UAnimMontage> death_montage_ = nullptr;

    UPROPERTY(BlueprintReadOnly, Category = "Combat")
    TObjectPtr<AActor> combat_target_;

    UPROPERTY(EditAnywhere, Category = "Combat")
    double warp_target_distance_ = 125.0f;
};
