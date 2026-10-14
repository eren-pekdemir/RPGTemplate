#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Stats/RPGStatModifier.h"
#include "RPGProgressionTypes.h"
#include "RPGProgressionComponent.generated.h"

class APawn;
class APlayerState;
class UCurveFloat;
class URPGSkillDefinition;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FRPGOnExperienceChangedSignature,
    int32, CurrentExperience, int32, RequiredExperience);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FRPGOnLevelUpSignature,
    int32, NewLevel, int32, LevelsGained);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRPGOnSkillPointsChangedSignature,
    int32, SkillPoints);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRPGOnSkillUnlockedSignature,
    const URPGSkillDefinition*, Skill);

/**
 * Level, experience and skill points.
 * Put it on the PlayerState so progress survives death; it then targets the possessed pawn.
 * On any other actor (e.g. an enemy) it targets its owner.
 */
UCLASS(ClassGroup = (RPG), meta = (BlueprintSpawnableComponent))
class RPGPROGRESSION_API URPGProgressionComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    URPGProgressionComponent();

    // ~ Settings

    /** X = level, Y = experience needed to go from that level to the next. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RPG|Progression")
    TObjectPtr<UCurveFloat> ExperienceCurve;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RPG|Progression", meta = (ClampMin = "1"))
    int32 StartingLevel = 1;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RPG|Progression", meta = (ClampMin = "1"))
    int32 MaxLevel = 20;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RPG|Progression", meta = (ClampMin = "0"))
    int32 SkillPointsPerLevel = 1;
    
    /** Added once per level above 1 (Level 3 = 2x these values). */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RPG|Progression", meta = (TitleProperty = "Stat"))
    TArray<FRPGStatModifier> StatsPerLevel;

    // ~ Events

    UPROPERTY(BlueprintAssignable, Category = "RPG|Progression")
    FRPGOnExperienceChangedSignature OnExperienceChanged;

    /** Fired once per AddExperience call, even if several levels were gained. */
    UPROPERTY(BlueprintAssignable, Category = "RPG|Progression")
    FRPGOnLevelUpSignature OnLevelUp;

    UPROPERTY(BlueprintAssignable, Category = "RPG|Progression")
    FRPGOnSkillPointsChangedSignature OnSkillPointsChanged;

    // ~ API

    UFUNCTION(BlueprintCallable, Category = "RPG|Progression")
    void AddExperience(int32 Amount);

    UFUNCTION(BlueprintPure, Category = "RPG|Progression")
    int32 GetLevel() const { return Level; }

    UFUNCTION(BlueprintPure, Category = "RPG|Progression")
    int32 GetExperience() const { return Experience; }

    /** Experience needed for the next level; 0 at max level or without a curve. */
    UFUNCTION(BlueprintPure, Category = "RPG|Progression")
    int32 GetExperienceToNextLevel() const;

    /** Experience / required, in [0, 1]. For XP bars. */
    UFUNCTION(BlueprintPure, Category = "RPG|Progression")
    float GetExperienceNormalized() const;

    UFUNCTION(BlueprintPure, Category = "RPG|Progression")
    int32 GetSkillPoints() const { return SkillPoints; }

    UFUNCTION(BlueprintPure, Category = "RPG|Progression")
    bool IsMaxLevel() const { return Level >= MaxLevel; }

    /** The actor that receives stats and tags: the PlayerState's pawn, or the owner itself. */
    UFUNCTION(BlueprintPure, Category = "RPG|Progression")
    AActor* GetTargetActor() const;
    
    UPROPERTY(BlueprintAssignable, Category = "RPG|Progression")
    FRPGOnSkillUnlockedSignature OnSkillUnlocked;

    UFUNCTION(BlueprintPure, Category = "RPG|Progression|Skills")
    ERPGSkillUnlockResult CanUnlockSkill(const URPGSkillDefinition* Skill) const;

    UFUNCTION(BlueprintCallable, Category = "RPG|Progression|Skills")
    ERPGSkillUnlockResult UnlockSkill(const URPGSkillDefinition* Skill);

    UFUNCTION(BlueprintPure, Category = "RPG|Progression|Skills")
    bool IsSkillUnlocked(const URPGSkillDefinition* Skill) const;


protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
    UFUNCTION()
    void HandlePawnSet(APlayerState* Player, APawn* NewPawn, APawn* OldPawn);

    /** Re-send stats and tags to the target. Empty in Step 1; filled in Steps 2 and 4. */
    void PushToTarget();

    UPROPERTY(SaveGame)
    int32 Level = 1;

    /** Experience gathered inside the current level. */
    UPROPERTY(SaveGame)
    int32 Experience = 0;

    UPROPERTY(SaveGame)
    int32 SkillPoints = 0;
    
    UPROPERTY(SaveGame)
    TArray<TObjectPtr<const URPGSkillDefinition>> UnlockedSkills;
};