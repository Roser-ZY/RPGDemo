// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Soul.h"
#include "NiagaraFunctionLibrary.h"

void ASoul::enablePickup()
{
    can_pickup_ = true;
}

int32 ASoul::getSoul()
{
    return soul_;
}

void ASoul::setSoul(int32 soul)
{
    soul_ = soul;
}

void ASoul::onSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                 const FHitResult& SweepResult)
{
    if (!can_pickup_) {
        return;
    }

    IPickupInterface* pickup_interface = Cast<IPickupInterface>(OtherActor);
    if (pickup_interface) {
        pickup_interface->addSoul(this);
    }

    if (pickup_effect_) {
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, pickup_effect_, GetActorLocation());
    }

    Destroy();
}
