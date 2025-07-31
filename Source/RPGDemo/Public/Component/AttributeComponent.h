// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttributeComponent.generated.h"


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class RPGDEMO_API UAttributeComponent : public UActorComponent {
    GENERATED_BODY()

public:
    // Sets default values for this component's properties
    UAttributeComponent();
    // Called every frame
    virtual void TickComponent(float DeltaTime, ELevelTick TickType,
                               FActorComponentTickFunction* ThisTickFunction) override;

    void receiveDamage(float damage);
    float getHealthPercentage();
    bool isAlive();

    int32 getCoin() const;
    int32 getSoul() const;
    void addCoin(int32 coin_number);
    void addSoul(int32 soul_number);

protected:
    // Called when the game starts
    virtual void BeginPlay() override;

private:
    UPROPERTY(EditAnywhere, Category = "Actor Attributes")
    float current_health_ = 100.0f;
    UPROPERTY(EditAnywhere, Category = "Actor Attributes")
    float max_health_ = 100.0f;

    UPROPERTY(EditAnywhere, Category = "Actor Attributes")
    int32 coin_ = 0;
    UPROPERTY(EditAnywhere, Category = "Actor Attributes")
    int32 soul_ = 0;
};
