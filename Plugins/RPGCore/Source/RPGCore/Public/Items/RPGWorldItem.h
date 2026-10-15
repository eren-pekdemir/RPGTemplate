#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "RPGWorldItem.generated.h"

class URPGItemDefinition;

UINTERFACE(MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class URPGWorldItem : public UInterface
{
	GENERATED_BODY()
};

/**
 * An actor that represents an item stack in the world (a pickup).
 * Lets loot, crafting or quests spawn one without knowing its class.
 */
class RPGCORE_API IRPGWorldItem
{
	GENERATED_BODY()

public:
	/** Called between SpawnActorDeferred and FinishSpawning, so OnConstruction already sees the item. */
	virtual void InitializeWorldItem(const URPGItemDefinition* Item, int32 Quantity) = 0;
};