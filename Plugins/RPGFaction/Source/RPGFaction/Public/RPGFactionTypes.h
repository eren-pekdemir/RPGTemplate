// RPGFactionTypes.h

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "RPGFactionTypes.generated.h"

/** One step of a faction's reputation ladder (Hostile, Neutral, Friendly, ...). */
USTRUCT(BlueprintType)
struct RPGFACTION_API FRPGReputationRank
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rank")
	FText DisplayName;

	/** Reputation at which this rank starts. Ranks must be sorted from lowest to highest. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rank")
	int32 MinReputation = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rank")
	FLinearColor Color = FLinearColor::White;

	/** Given to the player's pawn while at this rank (e.g. Reputation.Village.Friendly); recipes, dialogue and quests gate on it. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rank")
	FGameplayTag GrantedTag;

	/** Faction members treat the player as an enemy at this rank. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rank")
	bool bHostile = false;
};

/** Reputation changes with one faction spill over to another (allies +, rivals -). */
USTRUCT(BlueprintType)
struct RPGFACTION_API FRPGFactionRelation
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Relation", meta = (Categories = "Faction"))
	FGameplayTag Faction;

	/** Amount x this is applied to Faction. Negative for rivals. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Relation")
	float Multiplier = 0.5f;
};
