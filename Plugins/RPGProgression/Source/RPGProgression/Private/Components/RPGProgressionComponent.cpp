// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/RPGProgressionComponent.h"

#include "RPGCoreStatics.h"
#include "Curves/CurveFloat.h"
#include "GameFramework/PlayerState.h"
#include "RPGProgression.h"
#include "Interfaces/RPGStatReceiver.h"
#include "GameFramework/Controller.h"
#include "GameFramework/Pawn.h"
#include "Interfaces/RPGTagReceiver.h"
#include "Skills/RPGSkillDefinition.h"
#include "Stats/RPGStatModifier.h"

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

ERPGSkillUnlockResult URPGProgressionComponent::CanUnlockSkill(const URPGSkillDefinition* Skill) const
{
    if (!Skill) return ERPGSkillUnlockResult::InvalidSkill;
    if (IsSkillUnlocked(Skill)) return ERPGSkillUnlockResult::AlreadyUnlocked;
    if (Level < Skill->RequiredLevel) return ERPGSkillUnlockResult::LevelTooLow;

    for (const URPGSkillDefinition* Prerequisite : Skill->Prerequisites)
    {
        if (!Prerequisite) continue;   // empty slot in the data asset

        if (!IsSkillUnlocked(Prerequisite))
        {
            return ERPGSkillUnlockResult::MissingPrerequisite;
        }
    }

    if (SkillPoints < Skill->SkillPointCost) return ERPGSkillUnlockResult::NotEnoughPoints;

    return ERPGSkillUnlockResult::Success;
}

ERPGSkillUnlockResult URPGProgressionComponent::UnlockSkill(const URPGSkillDefinition* Skill)
{
   ERPGSkillUnlockResult Result = CanUnlockSkill(Skill);
    if (Result != ERPGSkillUnlockResult::Success)
    {
        return Result;
    }
    
    SkillPoints -= Skill->SkillPointCost;
    UnlockedSkills.Add(Skill);
    OnSkillPointsChanged.Broadcast(SkillPoints);
    OnSkillUnlocked.Broadcast(Skill);
    PushToTarget();
    
    return ERPGSkillUnlockResult::Success;
    
}

bool URPGProgressionComponent::IsSkillUnlocked(const URPGSkillDefinition* Skill) const
{
   return UnlockedSkills.Contains(Skill);
}


void URPGProgressionComponent::HandlePawnSet(APlayerState* Player, APawn* NewPawn, APawn* OldPawn)
{
    PushToTarget();
}
void URPGProgressionComponent::PushToTarget()
{
    AActor* Target = GetTargetActor();
    PushStats(Target);
    PushTags(Target);
}

void URPGProgressionComponent::PushStats(AActor* Target)
{
    IRPGStatReceiver* Receiver = URPGCoreStatics::FindStatReceiver(Target);
    if (!Receiver) return;

    TArray<FRPGStatModifier> Modifiers;

    // Level bonuses: StatsPerLevel x (Level - 1).
    const int32 Multiplier = Level - 1;
    for (const FRPGStatModifier& Modifier : StatsPerLevel)
    {
        if (!Modifier.Stat.IsValid()) continue;

        FRPGStatModifier Scaled = Modifier;
        Scaled.Value = Modifier.Value * Multiplier;
        Modifiers.Add(Scaled);
    }

    // Skill bonuses: added as they are. The receiver sums duplicate stats.
    for (const URPGSkillDefinition* Skill : UnlockedSkills)
    {
        if (!Skill) continue;

        for (const FRPGStatModifier& Modifier : Skill->StatModifiers)
        {
            if (!Modifier.Stat.IsValid()) continue;
            Modifiers.Add(Modifier);
        }
    }

    Receiver->SetStatModifiers(this, Modifiers);
}

void URPGProgressionComponent::PushTags(AActor* Target)
{
    IRPGTagReceiver* TagReceiver = URPGCoreStatics::FindTagReceiver(Target);
    if (!TagReceiver) return;
    
    FGameplayTagContainer Tags;
    
    for (const auto& Skill : UnlockedSkills)
    {
        if (!Skill) continue;
        
        Tags.AppendTags(Skill->GrantedTags);
    }
    
    TagReceiver->SetGrantedTags(this, Tags);
}

URPGProgressionComponent* URPGProgressionComponent::FindProgressionComponent(const AActor* Actor)
{
    if (!Actor)
    {
        return nullptr;
    }

    if (URPGProgressionComponent* Found = Actor->FindComponentByClass<URPGProgressionComponent>())
    {
        return Found;
    }

    const APlayerState* PlayerState = nullptr;
    if (const APawn* Pawn = Cast<APawn>(Actor))
    {
        PlayerState = Pawn->GetPlayerState();
    }
    else if (const AController* Controller = Cast<AController>(Actor))
    {
        PlayerState = Controller->PlayerState;
    }
    else if (const APawn* InstigatorPawn = Actor->GetInstigator())
    {
        PlayerState = InstigatorPawn->GetPlayerState();
    }

    return PlayerState ? PlayerState->FindComponentByClass<URPGProgressionComponent>() : nullptr;
}
