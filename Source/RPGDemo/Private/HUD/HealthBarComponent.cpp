// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/HealthBarComponent.h"

#include "Components/ProgressBar.h"
#include "HUD/HealthBar.h"

void UHealthBarComponent::setHealthPercentage(float percentage)
{
    // Get the UserWidgetObject set on this component.
    if (health_bar_widget_ == nullptr) {
        health_bar_widget_ = Cast<UHealthBar>(GetUserWidgetObject());
    }
    if (health_bar_widget_ && health_bar_widget_->health_bar) {
        health_bar_widget_->health_bar->SetPercent(percentage);
    }
}