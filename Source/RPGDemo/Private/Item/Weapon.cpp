// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Weapon.h"
#include "Components/BoxComponent.h"
#include "Interfaces/HitInterface.h"
#include "Kismet/KismetSystemLibrary.h"

AWeapon::AWeapon()
{
    trace_box_component_ = CreateDefaultSubobject<UBoxComponent>(TEXT("Weapon Trace Box"));
    trace_box_component_->SetupAttachment(RootComponent);
    // Set the collision preset.
    trace_box_component_->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    trace_box_component_->SetCollisionResponseToAllChannels(ECR_Overlap);
    trace_box_component_->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);

    box_trace_start_ = CreateDefaultSubobject<USceneComponent>(TEXT("Box Trace Start"));
    box_trace_start_->SetupAttachment(RootComponent);
    box_trace_end_ = CreateDefaultSubobject<USceneComponent>(TEXT("Box Trace End"));
    box_trace_end_->SetupAttachment(RootComponent);
}

void AWeapon::setWeaponCollisionEnabled(ECollisionEnabled::Type collision_enabled)
{
    if (is_equipped && trace_box_component_) {
        trace_box_component_->SetCollisionEnabled(collision_enabled);
    }
}
void AWeapon::equipTo(USceneComponent* to_parent, FName to_socket_name)
{
    if (is_equipped) {
        return;
    }
    if (to_parent) {
        // Note: The AttachToComponent() is same as Attach Component To Component in Blueprint.
        FAttachmentTransformRules transform_rules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget,
                                                  EAttachmentRule::SnapToTarget, true);
        static_mesh_component_->AttachToComponent(to_parent, transform_rules, to_socket_name);
    }
    is_equipped = true;
}

void AWeapon::BeginPlay()
{
    Super::BeginPlay();

    if (trace_box_component_) {
        trace_box_component_->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::onBoxBeginOverlap);
    }
}


void AWeapon::onSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                   UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                   const FHitResult& SweepResult)
{
    Super::onSphereBeginOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}

void AWeapon::onSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    Super::onSphereEndOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
}

void AWeapon::onBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                const FHitResult& SweepResult)
{
    const FVector start = box_trace_start_->GetComponentLocation();
    const FVector end = box_trace_end_->GetComponentLocation();
    TArray<AActor*> ignored_actors;
    ignored_actors.Add(this);
    FHitResult box_hit;
    UKismetSystemLibrary::BoxTraceSingle(this, start, end, FVector(5.0f, 5.0f, 5.0f),
                                         box_trace_start_->GetComponentRotation(), TraceTypeQuery1, false,
                                         ignored_actors, EDrawDebugTrace::ForDuration, box_hit, true);
    if (box_hit.GetActor()) {
        IHitInterface* hit_interface = Cast<IHitInterface>(box_hit.GetActor());
        if (hit_interface) {
            hit_interface->Execute_getHit(box_hit.GetActor(), box_hit.ImpactPoint);
        }
        createField(box_hit.ImpactPoint);
    }
}
