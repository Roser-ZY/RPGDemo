// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"

#include "Component/AttributeComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
AEnemy::AEnemy()
{
    // Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need
    // it.
    PrimaryActorTick.bCanEverTick = true;

    // Set the attributes of collision.
    USkeletalMeshComponent* skeletal_mesh = GetMesh();
    if (skeletal_mesh == nullptr) {
        skeletal_mesh->SetCollisionObjectType(ECC_WorldDynamic);
        skeletal_mesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
        skeletal_mesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
        skeletal_mesh->SetGenerateOverlapEvents(true);
        UCapsuleComponent* capsule_component = GetCapsuleComponent();
        if (capsule_component) {
            capsule_component->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
        }
    }

    attribute_component_ = CreateDefaultSubobject<UAttributeComponent>(TEXT("Attribute"));
    health_bar_widget_ = CreateDefaultSubobject<UWidgetComponent>(TEXT("Health Bar"));
    health_bar_widget_->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
    Super::BeginPlay();
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AEnemy::calculateHitDirection(const FVector& impact_point)
{
    // Calculate the hit position of the enemy.
    const FVector forward = GetActorForwardVector();
    const FVector impact_horizontal_with_actor(impact_point.X, impact_point.Y, GetActorLocation().Z);
    const FVector to_hit = (impact_horizontal_with_actor - GetActorLocation()).GetSafeNormal();

    // forward * to_hit = |forward||to_hit| * cos(theta)
    // |forward| = 1, |to_hit| = 1, so forward * to_hit = cos(theta)
    const double cos_theta = FVector::DotProduct(forward, to_hit);
    // Take the inverse cosine (arc - cosine) of cos(theta) to get theta.
    double theta = FMath::Acos(cos_theta);
    // Convert from radians to degrees.
    theta = FMath::RadiansToDegrees(theta);

    // If cross_product points down, theta should be negative.
    const FVector cross_product = FVector::CrossProduct(forward, to_hit);
    if (cross_product.Z < 0) {
        theta *= -1.f;
    }

    if (GEngine) {
        GEngine->AddOnScreenDebugMessage(1, 5.f, FColor::Green, FString::Printf(TEXT("Theta: %f"), theta), true);
    }
    // // Debug: Draw debug arrow.
    // UKismetSystemLibrary::DrawDebugArrow(this, GetActorLocation(), GetActorLocation() + cross_product * 120.f, 5.f,
    //                                      FColor::Blue, 5.f);
    // UKismetSystemLibrary::DrawDebugArrow(this, GetActorLocation(), GetActorLocation() + forward * 120.f, 5.f,
    //                                      FColor::Red, 5.f);
    // UKismetSystemLibrary::DrawDebugArrow(this, GetActorLocation(), GetActorLocation() + to_hit * 120.f, 5.f,
    //                                      FColor::Green, 5.f);
}
void AEnemy::getHit_Implementation(const FVector& impact_point)
{
    UE_LOG(LogTemp, Warning, TEXT("The enemy get hit."));
    calculateHitDirection(impact_point);

    // Play the hit animation.
    if (hit_react_montage_ == nullptr) {
        return;
    }
    USkeletalMeshComponent* skeletal_mesh = GetMesh();
    if (skeletal_mesh == nullptr) {
        return;
    }
    UAnimInstance* anim_instance = skeletal_mesh->GetAnimInstance();
    if (anim_instance == nullptr) {
    }
    anim_instance->Montage_Play(hit_react_montage_);

    // Show the hit particle.
    if (!hit_particle_system_) {
        return;
    }
    UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), hit_particle_system_, impact_point);
}
