// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PickupInterface.generated.h"



class ATreasure;
class ASoul;
class AItem;
// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UPickupInterface : public UInterface {
    GENERATED_BODY()
};

/**
 *
 */
class RPGDEMO_API IPickupInterface {
    GENERATED_BODY()

    // Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
    virtual void setOverlappingItem(AItem* item);
    virtual void addSoul(ASoul* soul);
    virtual void addTreasure(ATreasure* treasure);
};
