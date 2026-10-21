// RPGEconomySettings.h
// Project Settings -> Game -> RPG Economy. Saved to DefaultGame.ini.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "GameplayTagContainer.h"
#include "RPGEconomyTypes.h"
#include "RPGEconomySettings.generated.h"

UCLASS(Config = Game, DefaultConfig, meta = (DisplayName = "RPG Economy"))
class RPGECONOMY_API URPGEconomySettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	URPGEconomySettings();

	/** Used wherever a currency tag is left empty (wallets, merchants, rewards, dialogue actions). */
	UPROPERTY(Config, EditAnywhere, Category = "Currency", meta = (Categories = "Currency"))
	FGameplayTag DefaultCurrency;

	/** How each currency looks in the UI. */
	UPROPERTY(Config, EditAnywhere, Category = "Currency", meta = (Categories = "Currency", ForceInlineRow))
	TMap<FGameplayTag, FRPGCurrencyInfo> Currencies;

	/** An item's value is its Value fragment x this (e.g. Rare = 2.5). Rarities missing here count as 1. */
	UPROPERTY(Config, EditAnywhere, Category = "Prices", meta = (Categories = "Item.Rarity", ForceInlineRow))
	TMap<FGameplayTag, float> RarityValueMultipliers;
};
