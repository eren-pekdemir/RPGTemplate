// RPGFactionDefinition.h

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "RPGFactionTypes.h"
#include "RPGFactionDefinition.generated.h"

class UTexture2D;

/** A faction: name, reputation ladder, and how changes spill over to other factions. */
UCLASS(BlueprintType, Const)
class RPGFACTION_API URPGFactionDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Faction", meta = (Categories = "Faction"))
	FGameplayTag FactionTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Faction")
	FText DisplayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Faction", meta = (MultiLine = true))
	FText Description;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Faction")
	TSoftObjectPtr<UTexture2D> Icon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Faction")
	int32 StartingReputation = 0;

	/** Hidden factions still work, they just do not appear in the reputation panel. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Faction")
	bool bShowInUI = true;

	/** Lowest first. The player's rank is the last one whose MinReputation <= reputation. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ranks", meta = (TitleProperty = "DisplayName"))
	TArray<FRPGReputationRank> Ranks;

	/** Changes here also change these factions (one level deep, no chains). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Relations", meta = (TitleProperty = "Faction"))
	TArray<FRPGFactionRelation> Relations;

	/** INDEX_NONE if the faction has no ranks; below the first rank counts as the first rank. */
	int32 GetRankIndex(int32 Reputation) const;

#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif
};
