// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "SlashHUD.generated.h"

class USlashOverlay;
/**
 *
 */
UCLASS()
class RPGDEMO_API ASlashHUD : public AHUD {
    GENERATED_BODY()

public:
    FORCEINLINE USlashOverlay* getSlashOverlay()
    {
        return slash_overlay_;
    }

protected:
    virtual void BeginPlay() override;

    UPROPERTY(EditAnywhere)
    TSubclassOf<USlashOverlay> slash_overlay_class_;
    UPROPERTY()
    TObjectPtr<USlashOverlay> slash_overlay_;
};
