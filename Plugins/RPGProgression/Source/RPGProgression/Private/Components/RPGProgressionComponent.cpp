// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/RPGProgressionComponent.h"

#include "Curves/CurveFloat.h"
#include "GameFramework/PlayerState.h"
#include "RPGProgression.h"

URPGProgressionComponent::URPGProgressionComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void URPGProgressionComponent::BeginPlay()
{
    Super::BeginPlay();

    Level = FMath::Clamp(StartingLevel, 1, MaxLevel);

    if (!ExperienceCurve)
    {
        UE_LOG(LogRPGProgression, Warning, TEXT("%s: ExperienceCurve is not set, the character cannot level up"),
            *GetNameSafe(GetOwner()));
    }

    // On a PlayerState, follow the possessed pawn (respawn, possession changes).
    if (APlayerState* PlayerState = Cast<APlayerState>(GetOwner()))
    {
        PlayerState->OnPawnSet.AddDynamic(this, &ThisClass::HandlePawnSet);
    }

    // Sync on subscribe: the pawn may already be possessed.
    PushToTarget();
}

void URPGProgressionComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (APlayerState* PlayerState = Cast<APlayerState>(GetOwner()))
    {
        PlayerState->OnPawnSet.RemoveDynamic(this, &ThisClass::HandlePawnSet);
    }

    Super::EndPlay(EndPlayReason);
}

void URPGProgressionComponent::AddExperience(int32 Amount)
{
    if (Amount <= 0 || IsMaxLevel()) return;

    Experience += Amount;
    int32 LevelsGained = 0;

    // A single large reward can grant several levels.
    while (!IsMaxLevel())
    {
        const int32 Required = GetExperienceToNextLevel();
        if (Required <= 0)
        {
            // Bad curve data would otherwise loop forever.
            UE_LOG(LogRPGProgression, Warning, TEXT("%s: ExperienceCurve returns %d for level %d, cannot level up"),
                *GetNameSafe(GetOwner()), Required, Level);
            break;
        }

        if (Experience < Required) break;

        Experience -= Required;
        ++Level;
        SkillPoints += SkillPointsPerLevel;
        ++LevelsGained;
    }

    if (IsMaxLevel())
    {
        Experience = 0;
    }

    // State first, then broadcast.
    OnExperienceChanged.Broadcast(Experience, GetExperienceToNextLevel());

    if (LevelsGained > 0)
    {
        OnSkillPointsChanged.Broadcast(SkillPoints);
        OnLevelUp.Broadcast(Level, LevelsGained);
        PushToTarget();
    }
}

int32 URPGProgressionComponent::GetExperienceToNextLevel() const
{
    if (IsMaxLevel() || !ExperienceCurve) return 0;

    return FMath::RoundToInt(ExperienceCurve->GetFloatValue(Level));
}

float URPGProgressionComponent::GetExperienceNormalized() const
{
    const int32 Required = GetExperienceToNextLevel();
    if (Required <= 0) return 1.f;   // max level (or no curve): show a full bar

    return FMath::Clamp(static_cast<float>(Experience) / Required, 0.f, 1.f);
}

AActor* URPGProgressionComponent::GetTargetActor() const
{
    if (const APlayerState* PlayerState = Cast<APlayerState>(GetOwner()))
    {
        return PlayerState->GetPawn();
    }
    return GetOwner();
}

void URPGProgressionComponent::HandlePawnSet(APlayerState* Player, APawn* NewPawn, APawn* OldPawn)
{
    PushToTarget();
}

void URPGProgressionComponent::PushToTarget()
{
    // Step 2: push level stat modifiers. Step 4: push skill tags.
    UE_LOG(LogRPGProgression, Log, TEXT("Push to %s"), *GetNameSafe(GetTargetActor()));
}