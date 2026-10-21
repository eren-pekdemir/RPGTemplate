// RPGValueFragment.h
// Item fragment: what the item is worth. Merchants price items from it (and the rarity multipliers).

#pragma once

#include "CoreMinimal.h"
#include "Items/RPGItemFragment.h"
#include "RPGValueFragment.generated.h"

UCLASS(meta = (DisplayName = "Value"))
class RPGECONOMY_API URPGValueFragment : public URPGItemFragment
{
	GENERATED_BODY()

public:
	/** Base value in the default currency, before the rarity multiplier. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Value", meta = (ClampMin = "0"))
	int64 Value = 10;

	/** False: merchants never buy it (it can still be bought from their stock). Quest items are never sellable. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Value")
	bool bCanBeSold = true;
};
