// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/PickupInterface.h"
#include "Item/Item.h"
#include "Soul.generated.h"

class UNiagaraSystem;
/**
 *
 */
UCLASS()
class RPGDEMO_API ASoul : public AItem {
    GENERATED_BODY()
public:
    void enablePickup();

    int32 getSoul();
    void setSoul(int32 soul);

protected:
    virtual void onSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                      UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                      const FHitResult& SweepResult) override;

private:
    UPROPERTY(EditAnywhere)
    TObjectPtr<UNiagaraSystem> pickup_effect_ = nullptr;

    UPROPERTY(EditAnywhere, Category = "Soul Property")
    int32 soul_;

    bool can_pickup_ = true;
};
