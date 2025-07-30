// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SlashOverlay.generated.h"


class UTextBlock;
class UProgressBar;
/**
 *
 */
UCLASS()
class RPGDEMO_API USlashOverlay : public UUserWidget {
    GENERATED_BODY()
public:
    void setHealthBarPercent(float percent);
    void setStaminaBarPercent(float percent);
    void setCoinNumber(int32 coin_number);
    void setSoulNumber(int32 soul_number);

private:
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UProgressBar> HealthProgressBar;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UProgressBar> StaminaProgressBar;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> CoinText;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> SoulText;
};
