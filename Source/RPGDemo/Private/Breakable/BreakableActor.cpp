// Fill out your copyright notice in the Description page of Project Settings.


#include "Breakable/BreakableActor.h"

#include "Components/CapsuleComponent.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "Item/Treasure.h"

// Sets default values
ABreakableActor::ABreakableActor()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = false;

    geometry_collection_ = CreateDefaultSubobject<UGeometryCollectionComponent>("GeometryCollection");
    SetRootComponent(geometry_collection_);
    geometry_collection_->SetGenerateOverlapEvents(true);

    capsule_collection_ = CreateDefaultSubobject<UCapsuleComponent>("Capsule");
    capsule_collection_->SetupAttachment(GetRootComponent());
    capsule_collection_->SetCollisionResponseToChannels(ECR_Ignore);
    capsule_collection_->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
}

// Called when the game starts or when spawned
void ABreakableActor::BeginPlay()
{
    Super::BeginPlay();
}

// Called every frame
void ABreakableActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}
void ABreakableActor::getHit_Implementation(const FVector& impact_point, AActor* hitter)
{
    UWorld* world = GetWorld();
    if (!world) {
        return;
    }

    int32 treasure_class_num = treasure_classes_.Num();
    if (treasure_class_num > 0) {
        FVector location = GetActorLocation();
        location.Z += 75.0f;
        const int32 selection = FMath::RandRange(0, treasure_class_num - 1);
        world->SpawnActor<ATreasure>(treasure_classes_[selection], location, GetActorRotation());
    }
}
