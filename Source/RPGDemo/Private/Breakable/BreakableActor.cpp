// Fill out your copyright notice in the Description page of Project Settings.


#include "Breakable/BreakableActor.h"
#include "GeometryCollection/GeometryCollectionComponent.h"

// Sets default values
ABreakableActor::ABreakableActor()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = false;

    geometry_collection_ = CreateDefaultSubobject<UGeometryCollectionComponent>("GeometryCollection");
    SetRootComponent(geometry_collection_);

    geometry_collection_->SetGenerateOverlapEvents(true);
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

void ABreakableActor::getHit_Implementation(const FVector& impact_point)
{
    
}
