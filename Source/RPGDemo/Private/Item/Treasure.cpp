// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Treasure.h"

#include "Character/DemoCharacter.h"
#include "Kismet/GameplayStatics.h"

void ATreasure::onSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                     UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                     const FHitResult& SweepResult)
{
    // Set the overlapping item to the character.
    IPickupInterface* pickup_interface = Cast<IPickupInterface>(OtherActor);
    if (pickup_interface) {
        if (pickup_sound_) {
            UGameplayStatics::PlaySoundAtLocation(this, pickup_sound_, GetActorLocation());
        }

        pickup_interface->addTreasure(this);

        // Destroy the treasure when overlapped with the character.
        Destroy();
    }
}

int32 ATreasure::getCoin()
{
    return coin_;
}

void ATreasure::setCoin(int32 coin)
{
    coin_ = coin;
}
