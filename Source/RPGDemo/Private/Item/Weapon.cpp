// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Weapon.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Interfaces/HitInterface.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraComponent.h"

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
    if (item_state_ == EItemState::EIS_Equipped && trace_box_component_) {
        trace_box_component_->SetCollisionEnabled(collision_enabled);
    }
}


void AWeapon::attachMeshToSocket(USceneComponent* to_parent, FName to_socket_name)
{
    // Note: The AttachToComponent() is same as Attach Component To Component in Blueprint.
    FAttachmentTransformRules transform_rules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget,
                                              EAttachmentRule::SnapToTarget, true);
    static_mesh_component_->AttachToComponent(to_parent, transform_rules, to_socket_name);
}

void AWeapon::equipTo(USceneComponent* to_parent, FName to_socket_name, AActor* in_owner, APawn* in_instigator)
{
    if (item_state_ == EItemState::EIS_Equipped) {
        return;
    }
    if (to_parent) {
        attachMeshToSocket(to_parent, to_socket_name);
    }
    item_state_ = EItemState::EIS_Equipped;

    SetOwner(in_owner);
    SetInstigator(in_instigator);

    // Disable the collision of the sphere component.
    if (sphere_component_) {
        sphere_component_->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    }

    // Deactivate the embers effect when equipped.
    if (embers_effect_) {
        embers_effect_->Deactivate();
    }
}

void AWeapon::clearIgnoredActors()
{
    ignored_actors_.Empty();
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
    UE_LOG(LogTemp, Warning, TEXT("Overlapped with %s."), *OtherActor->GetName());
    UE_LOG(LogTemp, Warning, TEXT("Overlapped with %s."), *OtherComp->GetName());
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
    FHitResult box_hit;

    // // Debug: Log the name of the overlapped component and actor.
    // if (OverlappedComponent) {
    //     UE_LOG(LogTemp, Warning, TEXT("%s"), *OtherComp->GetName());
    //     UE_LOG(LogTemp, Warning, TEXT("%s"), *OtherActor->GetName());
    //     DrawDebugPoint(GetWorld(), SweepResult.ImpactPoint, 10.0f, FColor::Yellow, false, 2.0f);
    // }

    UKismetSystemLibrary::BoxTraceSingle(this, start, end, FVector(5.0f, 5.0f, 5.0f),
                                         box_trace_start_->GetComponentRotation(), TraceTypeQuery1, false,
                                         ignored_actors_, EDrawDebugTrace::None, box_hit, true);
    if (box_hit.GetActor()) {
        // Apply damage before execute interface function to calculate health before play the animation in getHit().
        UGameplayStatics::ApplyDamage(box_hit.GetActor(), damage_, GetInstigator()->GetController(), this,
                                      UDamageType::StaticClass());

        IHitInterface* hit_interface = Cast<IHitInterface>(box_hit.GetActor());
        if (hit_interface) {
            hit_interface->Execute_getHit(box_hit.GetActor(), box_hit.ImpactPoint);
            // Ignore the hit actor to avoid multiple hit.
            ignored_actors_.AddUnique(box_hit.GetActor());
        }
        createField(box_hit.ImpactPoint);
    }
}
