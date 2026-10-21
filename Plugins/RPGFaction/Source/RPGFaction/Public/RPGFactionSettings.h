// RPGFactionSettings.h
// Project Settings -> Game -> RPG Factions.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "GameplayTagContainer.h"
#include "RPGFactionSettings.generated.h"

class URPGFactionDefinition;

UCLASS(Config = Game, DefaultConfig, meta = (DisplayName = "RPG Factions"))
class RPGFACTION_API URPGFactionSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	/** Every faction in the game. */
	UPROPERTY(Config, EditAnywhere, Category = "Factions")
	TArray<TSoftObjectPtr<URPGFactionDefinition>> Factions;

	UPROPERTY(Config, EditAnywhere, Category = "Reputation")
	int32 MinReputation = -1000;

	UPROPERTY(Config, EditAnywhere, Category = "Reputation")
	int32 MaxReputation = 1000;

	/** Loads on first use (small data assets). */
	static const URPGFactionDefinition* FindFaction(const FGameplayTag& FactionTag);
	static TArray<const URPGFactionDefinition*> GetAllFactions();
};
