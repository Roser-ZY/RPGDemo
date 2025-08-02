// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/AttributeComponent.h"

// Sets default values for this component's properties
UAttributeComponent::UAttributeComponent()
{
    // Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these
    // features off to improve performance if you don't need them.
    PrimaryComponentTick.bCanEverTick = false;
}


// Called when the game starts
void UAttributeComponent::BeginPlay()
{
    Super::BeginPlay();
}

void UAttributeComponent::receiveDamage(float damage)
{
    // Clamp the value between 0 and max_health when calculating.
    current_health_ = FMath::Clamp(current_health_ - damage, 0, max_health_);
}

float UAttributeComponent::getHealthPercentage()
{
    return current_health_ / max_health_;
}

bool UAttributeComponent::isAlive()
{
    return current_health_ > 0.0f;
}

int32 UAttributeComponent::getCoin() const
{
    return coin_;
}

int32 UAttributeComponent::getSoul() const
{
    return soul_;
}

float UAttributeComponent::getCurrentStamina() const
{
    return current_stamina_;
}

float UAttributeComponent::getStaminaPercentage() const
{
    return current_stamina_ / max_stamina_;
}

void UAttributeComponent::addCoin(int32 coin_number)
{
    coin_ += coin_number;
}

void UAttributeComponent::addSoul(int32 soul_number)
{
    soul_ += soul_number;
}

void UAttributeComponent::useStamina(float used_stamina)
{
    current_stamina_ = FMath::Clamp(current_stamina_ - used_stamina, 0, max_stamina_);
}

void UAttributeComponent::regenerateStamina(float delta_time)
{
    current_stamina_ = FMath::Clamp(current_stamina_ + stamina_recoverty_rate_ * delta_time, 0, max_stamina_);
}

// Called every frame
void UAttributeComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                        FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}
