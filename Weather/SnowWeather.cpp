// SnowWeather.h + SnowWeather.cpp combined
// Handles probabilistic snow events, biome restriction, visibility changes, mob freezing, and player damage

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SnowWeather.generated.h"

UCLASS()
class STRONGHOLDSURVIVAL_API ASnowWeather : public AActor
{
    GENERATED_BODY()

public:
    ASnowWeather();

protected:
    virtual void BeginPlay() override;

    // === Weather State ===
    bool bIsSnowing;
    float VisibilityLevel; // 10 = clear, 5 = snow minimum

    // === Timers ===
    FTimerHandle WeatherCheckHandle;
    FTimerHandle SnowDurationHandle;
    FTimerHandle PlayerDamageHandle;

    // === Functions ===
    void SpinWeatherWheel();
    void StartSnow();
    void StopSnow();
    void FreezeMobs();
    void DamagePlayerIfNoWinterClothes();

    // Utility
    bool IsInSnowBiome();
    void UpdateVisibility();
};

// ================= Implementation =================

#include "SnowWeather.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"
#include "GameFramework/Character.h"
#include "MobBase.h" // Example mob base class

ASnowWeather::ASnowWeather()
{
    PrimaryActorTick.bCanEverTick = false;
    bIsSnowing = false;
    VisibilityLevel = 10.f; // default clear visibility
}

void ASnowWeather::BeginPlay()
{
    Super::BeginPlay();

    // Run weather wheel every second
    GetWorldTimerManager().SetTimer(WeatherCheckHandle, this, &ASnowWeather::SpinWeatherWheel, 1.0f, true);
}

bool ASnowWeather::IsInSnowBiome()
{
    // Example: check player location tag or biome system
    // Replace with your biome detection logic
    ACharacter* Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
    if (!Player) return false;

    FVector Loc = Player->GetActorLocation();
    // Example: Z > 2000 = snowy biome
    return Loc.Z > 2000.f;
}

void ASnowWeather::SpinWeatherWheel()
{
    if (!IsInSnowBiome()) return;

    float Chance = FMath::FRandRange(0.f, 100.f);
    if (Chance <= 0.5f)
    {
        if (!bIsSnowing)
        {
            StartSnow();
        }
    }
}

void ASnowWeather::StartSnow()
{
    bIsSnowing = true;
    UpdateVisibility();

    if (GEngine)
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, TEXT("Snow has started!"));

    FreezeMobs();

    // Damage player every 5 seconds if no winter clothes
    GetWorldTimerManager().SetTimer(PlayerDamageHandle, this, &ASnowWeather::DamagePlayerIfNoWinterClothes, 5.f, true);

    // Snow lasts ~20 minutes (1200 seconds)
    GetWorldTimerManager().SetTimer(SnowDurationHandle, this, &ASnowWeather::StopSnow, 1200.f, false);
}

void ASnowWeather::StopSnow()
{
    bIsSnowing = false;
    VisibilityLevel = 10.f; // reset to clear

    if (GEngine)
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("Snow has stopped."));

    GetWorldTimerManager().ClearTimer(PlayerDamageHandle);
}

void ASnowWeather::UpdateVisibility()
{
    if (bIsSnowing)
    {
        VisibilityLevel = 5.f; // minimum visibility
        // Example: adjust fog or post-process
        // FogComponent->SetFogDensity(0.7f);
    }
    else
    {
        VisibilityLevel = 10.f;
    }
}

void ASnowWeather::FreezeMobs()
{
    TArray<AActor*> AllMobs;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMobBase::StaticClass(), AllMobs);

    for (AActor* Actor : AllMobs)
    {
        AMobBase* Mob = Cast<AMobBase>(Actor);
        if (Mob)
        {
            Mob->SetFrozen(true); // Example function in MobBase
        }
    }

    if (GEngine)
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, TEXT("All mobs are frozen by snow!"));
}

void ASnowWeather::DamagePlayerIfNoWinterClothes()
{
    ACharacter* Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
    if (!Player) return;

    // Example: check if player has winter clothes equipped
    bool bHasWinterClothes = false; // Replace with your inventory/equipment system

    if (!bHasWinterClothes)
    {
        UGameplayStatics::ApplyDamage(Player, 5.f, nullptr, this, nullptr);
        if (GEngine)
            GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("You are freezing! Wear winter clothes!"));
    }
}
