// LightningWeather.h + LightningWeather.cpp combined
// Handles probabilistic lightning strikes, biome restrictions, fire ignition, and fire spreading

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LightningWeather.generated.h"

UCLASS()
class STRONGHOLDSURVIVAL_API ALightningWeather : public AActor
{
    GENERATED_BODY()

public:
    ALightningWeather();

protected:
    virtual void BeginPlay() override;

    // === Weather State ===
    bool bIsLightningActive;

    // === Timers ===
    FTimerHandle WeatherCheckHandle;

    // === Functions ===
    void SpinWeatherWheel();
    void TriggerLightningStrike();
    bool IsValidBiome();
    void IgniteObject(AActor* HitActor);
    void SpreadFire(AActor* SourceActor);
};

// ================= Implementation =================

#include "LightningWeather.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"
#include "Block.h" // Example block class with BlockType
#include "TreeActor.h" // Example tree actor
#include "GrassActor.h" // Example grass actor

ALightningWeather::ALightningWeather()
{
    PrimaryActorTick.bCanEverTick = false;
    bIsLightningActive = false;
}

void ALightningWeather::BeginPlay()
{
    Super::BeginPlay();

    // Run weather wheel every second
    GetWorldTimerManager().SetTimer(WeatherCheckHandle, this, &ALightningWeather::SpinWeatherWheel, 1.0f, true);
}

bool ALightningWeather::IsValidBiome()
{
    // Example biome check: exclude desert and snow
    ACharacter* Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
    if (!Player) return false;

    FVector Loc = Player->GetActorLocation();

    // Replace with your biome system
    bool bInDesert = (Loc.X > 5000.f && Loc.X < 8000.f); 
    bool bInSnow = (Loc.Z > 2000.f);

    return !(bInDesert || bInSnow);
}

void ALightningWeather::SpinWeatherWheel()
{
    if (!IsValidBiome()) return;

    float Chance = FMath::FRandRange(0.f, 100.f);
    if (Chance <= 0.001f)
    {
        TriggerLightningStrike();
    }
}

void ALightningWeather::TriggerLightningStrike()
{
    bIsLightningActive = true;

    // Pick random location near player
    ACharacter* Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
    if (!Player) return;

    FVector StrikeLoc = Player->GetActorLocation() + FVector(FMath::RandRange(-500.f, 500.f), FMath::RandRange(-500.f, 500.f), 0.f);

    if (GEngine)
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("⚡ Lightning Strike!"));

    // Find actors at strike location
    TArray<AActor*> HitActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), HitActors);

    for (AActor* Actor : HitActors)
    {
        float Dist = FVector::Dist(Actor->GetActorLocation(), StrikeLoc);
        if (Dist < 200.f) // strike radius
        {
            IgniteObject(Actor);
        }
    }

    bIsLightningActive = false;
}

void ALightningWeather::IgniteObject(AActor* HitActor)
{
    if (!HitActor) return;

    // Trees
    if (HitActor->IsA(ATreeActor::StaticClass()))
    {
        if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Orange, TEXT("Tree ignited by lightning!"));
        SpreadFire(HitActor);
    }

    // Grass
    if (HitActor->IsA(AGrassActor::StaticClass()))
    {
        if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Orange, TEXT("Dry grass ignited by lightning!"));
        SpreadFire(HitActor);
    }

    // Blocks
    ABlock* Block = Cast<ABlock>(HitActor);
    if (Block && (Block->BlockType == "Wood" || Block->BlockType == "Nature"))
    {
        if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Orange, TEXT("Wood block ignited by lightning!"));
        SpreadFire(HitActor);
    }
}

void ALightningWeather::SpreadFire(AActor* SourceActor)
{
    // Simple fire spread: ignite nearby actors
    TArray<AActor*> NearbyActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), NearbyActors);

    for (AActor* Actor : NearbyActors)
    {
        float Dist = FVector::Dist(Actor->GetActorLocation(), SourceActor->GetActorLocation());
        if (Dist < 300.f && Actor != SourceActor)
        {
            IgniteObject(Actor);
        }
    }
}
