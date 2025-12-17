// RainWeatherManager.h + RainWeatherManager.cpp combined
// Handles probabilistic rain events, cloud spawning, light level changes, and mob triggers

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RainWeatherManager.generated.h"

UCLASS()
class STRONGHOLDSURVIVAL_API ARainWeatherManager : public AActor
{
    GENERATED_BODY()

public:
    ARainWeatherManager();

protected:
    virtual void BeginPlay() override;

    // === Weather State ===
    bool bIsRaining;
    float LightLevel; // 10 = normal day, 5 = rain

    // === Timers ===
    FTimerHandle WeatherCheckHandle;
    FTimerHandle RainDurationHandle;

    // === Functions ===
    void SpinWeatherWheel();
    void StartRain();
    void StopRain();
    void IntensifyRain();

    // Utility
    void UpdateLightLevel();
    void TriggerMobSpawns();
};

// ================= Implementation =================

#include "RainWeatherManager.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"

ARainWeatherManager::ARainWeatherManager()
{
    PrimaryActorTick.bCanEverTick = false;
    bIsRaining = false;
    LightLevel = 10.f; // default day brightness
}

void ARainWeatherManager::BeginPlay()
{
    Super::BeginPlay();

    // Run weather wheel every second
    GetWorldTimerManager().SetTimer(WeatherCheckHandle, this, &ARainWeatherManager::SpinWeatherWheel, 1.0f, true);
}

void ARainWeatherManager::SpinWeatherWheel()
{
    // 0.08% chance per second
    float Chance = FMath::FRandRange(0.f, 100.f);
    if (Chance <= 0.08f)
    {
        if (!bIsRaining)
        {
            StartRain();
        }
        else
        {
            IntensifyRain();
        }
    }
}

void ARainWeatherManager::StartRain()
{
    bIsRaining = true;
    UpdateLightLevel();

    if (GEngine)
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("Rain has started!"));

    // Trigger mobs that only spawn during rain
    TriggerMobSpawns();

    // Clouds + rain particles would be spawned here (assets required)

    // Rain lasts ~20 minutes (1200 seconds)
    GetWorldTimerManager().SetTimer(RainDurationHandle, this, &ARainWeatherManager::StopRain, 1200.f, false);
}

void ARainWeatherManager::StopRain()
{
    bIsRaining = false;
    LightLevel = 10.f; // reset to normal day brightness

    if (GEngine)
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("Rain has stopped."));
}

void ARainWeatherManager::IntensifyRain()
{
    if (GEngine)
        GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Cyan, TEXT("Rain intensifies!"));
    // Could increase particle density, sound volume, etc.
}

void ARainWeatherManager::UpdateLightLevel()
{
    if (bIsRaining)
    {
        LightLevel = 5.f; // darker during rain
        // Example: adjust directional light intensity
        // DirectionalLight->SetIntensity(LightLevel);
    }
    else
    {
        LightLevel = 10.f;
    }
}

void ARainWeatherManager::TriggerMobSpawns()
{
    // Example: spawn mobs only during rain
    // UGameplayStatics::SpawnActor<AMob>(MobClass, SpawnLocation, SpawnRotation);

    if (GEngine)
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Rain mobs are spawning!"));
}
