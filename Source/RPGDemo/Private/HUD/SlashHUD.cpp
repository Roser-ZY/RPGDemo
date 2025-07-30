// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/SlashHUD.h"

#include "Blueprint/UserWidget.h"
#include "HUD/SlashOverlay.h"

void ASlashHUD::BeginPlay()
{
    Super::BeginPlay();

    UWorld* world = GetWorld();
    if (world == nullptr) {
        return;
    }
    APlayerController* controller = world->GetFirstPlayerController();
    if (controller == nullptr || !slash_overlay_class_) {
        return;
    }
    slash_overlay_ = CreateWidget<USlashOverlay>(controller, slash_overlay_class_);
    slash_overlay_->AddToViewport();
}
