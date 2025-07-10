// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Treasure.h"

#include "DemoCharacter.h"
#include "Kismet/GameplayStatics.h"

void ATreasure::onSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                     UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                     const FHitResult& SweepResult)
{
    // Set the overlapping item to the character.
    ADemoCharacter* demo_character = Cast<ADemoCharacter>(OtherActor);
    if (demo_character) {
        if (pickup_sound_) {
            UGameplayStatics::PlaySoundAtLocation(this, pickup_sound_, GetActorLocation());
        }
        // Destroy the treasure when overlapped with the character.
        Destroy();
    }
}