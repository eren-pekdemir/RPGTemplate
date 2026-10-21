// RPGReputationHolder.h
// Contract for whatever stores the player's standing with factions (RPGFaction's reputation component).
// Dialogue, quests and AI change or read reputation through this interface only.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Interface.h"
#include "RPGReputationHolder.generated.h"

class AActor;

UINTERFACE(MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class URPGReputationHolder : public UInterface
{
	GENERATED_BODY()
};

class RPGCORE_API IRPGReputationHolder
{
	GENERATED_BODY()

public:
	virtual int32 GetReputation(const FGameplayTag& Faction) const = 0;

	/** Adds (or removes, if negative) reputation; related factions may change too. */
	virtual void AddReputation(const FGameplayTag& Faction, int32 Amount) = 0;

	/**
	 * Finds the holder on the actor or its components, or on the PlayerState of a pawn or controller
	 * (or of the actor's instigator: projectiles, traps).
	 */
	static IRPGReputationHolder* Find(AActor* Actor);
};
