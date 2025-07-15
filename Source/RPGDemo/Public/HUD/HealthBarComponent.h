// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "HealthBarComponent.generated.h"

class UHealthBar;

/**
 *
 */
UCLASS()
class RPGDEMO_API UHealthBarComponent : public UWidgetComponent {
    GENERATED_BODY()
public:
    void setHealthPercentage(float percentage);

private:
    UPROPERTY()
    UHealthBar* health_bar_widget_ = nullptr;
};
