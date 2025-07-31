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
    if (item_effect_) {
        item_effect_->Deactivate();
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

void AWeapon::executeGetHit(const FHitResult& box_hit)
{
    AActor* hit_actor = box_hit.GetActor();

    if (hit_actor) {
        IHitInterface* hit_interface = Cast<IHitInterface>(hit_actor);
        if (hit_interface) {
            hit_interface->Execute_getHit(hit_actor, box_hit.ImpactPoint, GetOwner());
        }
    }
}
bool AWeapon::otherActorIsSameType(AActor* other_actor)
{
    AActor* owner = GetOwner();
    if (owner == nullptr) {
        return false;
    }

    return owner->ActorHasTag(TEXT("Enemy")) && other_actor->ActorHasTag(TEXT("Enemy"));
}
void AWeapon::onBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                const FHitResult& SweepResult)
{
    AActor* owner = GetOwner();
    if (owner == nullptr || otherActorIsSameType(OtherActor)) {
        return;
    }

    ignored_actors_.AddUnique(owner);

    const FVector start = box_trace_start_->GetComponentLocation();
    const FVector end = box_trace_end_->GetComponentLocation();

    FHitResult box_hit;
    UKismetSystemLibrary::BoxTraceSingle(
        this, start, end, box_trace_extent_, box_trace_start_->GetComponentRotation(), TraceTypeQuery1, false,
        ignored_actors_, show_debug_box_ ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None, box_hit, true);

    createField(box_hit.ImpactPoint);

    AActor* hit_actor = box_hit.GetActor();
    if (hit_actor) {
        if (otherActorIsSameType(hit_actor)) {
            return;
        }
        // Apply damage before execute interface function to calculate health before play the animation in getHit().
        UGameplayStatics::ApplyDamage(hit_actor, damage_, GetInstigator()->GetController(), this,
                                      UDamageType::StaticClass());
        executeGetHit(box_hit);
        // Ignore the hit actor to avoid multiple hit.
        ignored_actors_.AddUnique(hit_actor);
    }
}
