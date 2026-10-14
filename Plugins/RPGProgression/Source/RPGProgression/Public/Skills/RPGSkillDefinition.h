#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "Stats/RPGStatModifier.h"
#include "RPGSkillDefinition.generated.h"

class UTexture2D;

/** One node of a skill tree: what it costs, what it requires, and what it gives. */
UCLASS(BlueprintType)
class RPGPROGRESSION_API URPGSkillDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Display")
	FText DisplayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Display", meta = (MultiLine = true))
	FText Description;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Display")
	TSoftObjectPtr<UTexture2D> Icon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Requirements", meta = (ClampMin = "0"))
	int32 SkillPointCost = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Requirements", meta = (ClampMin = "1"))
	int32 RequiredLevel = 1;

	/** All of these must be unlocked first. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Requirements")
	TArray<TObjectPtr<const URPGSkillDefinition>> Prerequisites;

	/** Permanent stat bonuses while unlocked. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rewards", meta = (TitleProperty = "Stat"))
	TArray<FRPGStatModifier> StatModifiers;

	/** Tags granted while unlocked (Step 4: used to unlock abilities). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rewards", meta = (Categories = "Skill"))
	FGameplayTagContainer GrantedTags;
};