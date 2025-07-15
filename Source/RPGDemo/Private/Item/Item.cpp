// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Item.h"

#include "Components/SphereComponent.h"
#include "DemoCharacter.h"
#include "NiagaraComponent.h"

// Sets default values
AItem::AItem()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    static_mesh_component_ = CreateDefaultSubobject<UStaticMeshComponent>("StaticMeshComponent");
    RootComponent = static_mesh_component_;

    sphere_component_ = CreateDefaultSubobject<USphereComponent>("SphereComponent");
    sphere_component_->SetupAttachment(RootComponent);

    embers_effect_ = CreateDefaultSubobject<UNiagaraComponent>("EmbersEffect");
    embers_effect_->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
    Super::BeginPlay();

    if (sphere_component_) {
        sphere_component_->OnComponentBeginOverlap.AddDynamic(this, &AItem::onSphereBeginOverlap);
        sphere_component_->OnComponentEndOverlap.AddDynamic(this, &AItem::onSphereEndOverlap);
    }
}

void AItem::onSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                 const FHitResult& SweepResult)
{
    if (GEngine) {
        GEngine->AddOnScreenDebugMessage(0, 30, FColor::Cyan, TEXT("Item begins overlap."));
    }

    // Set the overlapping item to the character.
    ADemoCharacter* demo_character = Cast<ADemoCharacter>(OtherActor);
    if (demo_character) {
        demo_character->setOverlappingItem(this);
    }
}

void AItem::onSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                               UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if (GEngine) {
        GEngine->AddOnScreenDebugMessage(0, 30, FColor::Cyan, TEXT("Item ends overlap."));
    }

    // Unset the overlapping item to the character.
    ADemoCharacter* demo_character = Cast<ADemoCharacter>(OtherActor);
    if (demo_character) {
        demo_character->setOverlappingItem(nullptr);
    }
}

// Called every frame
void AItem::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}
