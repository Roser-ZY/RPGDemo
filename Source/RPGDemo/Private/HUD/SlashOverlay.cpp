// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/SlashOverlay.h"

#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void USlashOverlay::setHealthBarPercent(float percent)
{
    if (HealthProgressBar == nullptr) {
        return;
    }
    HealthProgressBar->SetPercent(percent);
}

void USlashOverlay::setStaminaBarPercent(float percent)
{
    if (StaminaProgressBar == nullptr) {
        return;
    }
    StaminaProgressBar->SetPercent(percent);
}

void USlashOverlay::setCoinNumber(int32 coin_number)
{
    if (CoinText == nullptr) {
        return;
    }
    const FString coin_number_string = FString::Printf(TEXT("%d"), coin_number);
    const FText coin_number_text = FText::FromString(coin_number_string);
    CoinText->SetText(coin_number_text);
}

void USlashOverlay::setSoulNumber(int32 soul_number)
{
    if (SoulText == nullptr) {
        return;
    }
    const FString soul_number_string = FString::Printf(TEXT("%d"), soul_number);
    const FText soul_number_text = FText::FromString(soul_number_string);
    SoulText->SetText(soul_number_text);
}
