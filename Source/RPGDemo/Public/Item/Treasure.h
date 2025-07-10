// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/Item.h"
#include "Treasure.generated.h"

/**
 *
 */
UCLASS()
class RPGDEMO_API ATreasure : public AItem {
    GENERATED_BODY()

public:
    virtual void onSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                      UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                      const FHitResult& SweepResult) override;

private:
    UPROPERTY(EditAnywhere, Category = "Sound")
    USoundBase* pickup_sound_;
};
