// RPGEconomyLibrary.h
// Currency resolution and item values, shared by wallets, merchants, rewards and the UI.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "RPGEconomyTypes.h"
#include "RPGEconomyLibrary.generated.h"

class URPGItemDefinition;

UCLASS()
class RPGECONOMY_API URPGEconomyLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/** The tag itself, or the default currency (Project Settings -> RPG Economy) when it is empty. */
	UFUNCTION(BlueprintPure, Category = "RPG|Economy")
	static FGameplayTag ResolveCurrency(const FGameplayTag& Currency);

	/** Value fragment x rarity multiplier, rounded. 0 = the item has no value. */
	UFUNCTION(BlueprintPure, Category = "RPG|Economy")
	static int64 GetItemValue(const URPGItemDefinition* Item);

	/** Has a Value fragment that allows selling and is not a quest item. */
	UFUNCTION(BlueprintPure, Category = "RPG|Economy")
	static bool CanItemBeSold(const URPGItemDefinition* Item);

	/** Display data for a currency (empty tag = default currency). */
	UFUNCTION(BlueprintPure, Category = "RPG|Economy")
	static FRPGCurrencyInfo GetCurrencyInfo(FGameplayTag Currency);
};
