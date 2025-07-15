// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/Item.h"
#include "Weapon.generated.h"

class UBoxComponent;
/**
 *
 */
UCLASS()
class RPGDEMO_API AWeapon : public AItem {
    GENERATED_BODY()

public:
    AWeapon();

    UFUNCTION(BlueprintCallable)
    void setWeaponCollisionEnabled(ECollisionEnabled::Type collision_enabled);
    void attachMeshToSocket(USceneComponent* to_parent, FName to_socket_name);

    UFUNCTION(BlueprintCallable)
    void equipTo(USceneComponent* to_parent, FName to_socket_name, AActor* in_owner, APawn* in_instigator);

    void clearIgnoredActors();

protected:
    virtual void BeginPlay() override;

    virtual void onSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                      UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                      const FHitResult& SweepResult) override;

    virtual void onSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

    // This function will be called multiple times in one hit.
    UFUNCTION()
    void onBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                           int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    UFUNCTION(BlueprintImplementableEvent)
    void createField(const FVector& field_location);

    UPROPERTY(VisibleAnywhere, Category = "Weapon Trace")
    UBoxComponent* trace_box_component_ = nullptr;
    UPROPERTY(VisibleAnywhere, Category = "Weapon Trace")
    USceneComponent* box_trace_start_ = nullptr;
    UPROPERTY(VisibleAnywhere, Category = "Weapon Trace")
    USceneComponent* box_trace_end_ = nullptr;

    TArray<AActor*> ignored_actors_;

    UPROPERTY(EditAnywhere, Category = "Weapon Property")
    float damage_ = 20.0f;
};
