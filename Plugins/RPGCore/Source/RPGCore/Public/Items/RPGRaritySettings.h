#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "GameplayTagContainer.h"
#include "RPGRaritySettings.generated.h"

/** How a rarity looks and ranks. */
USTRUCT(BlueprintType)
struct RPGCORE_API FRPGRarityInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rarity")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rarity")
	FLinearColor Color = FLinearColor::White;

	/** Higher = rarer. Used for sorting, comparisons and (later) loot luck. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rarity")
	int32 Tier = 0;
};

/** Project Settings → Game → RPG Rarity. Saved to DefaultGame.ini. */
UCLASS(Config = Game, DefaultConfig, meta = (DisplayName = "RPG Rarity"))
class RPGCORE_API URPGRaritySettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(Config, EditAnywhere, Category = "Rarity", meta = (Categories = "Item.Rarity", ForceInlineRow))
	TMap<FGameplayTag, FRPGRarityInfo> Rarities;

	/** Used for items without a rarity, or with a rarity missing from the map. */
	UPROPERTY(Config, EditAnywhere, Category = "Rarity")
	FRPGRarityInfo FallbackRarity;

	const FRPGRarityInfo& GetRarityInfo(const FGameplayTag& Rarity) const;
};