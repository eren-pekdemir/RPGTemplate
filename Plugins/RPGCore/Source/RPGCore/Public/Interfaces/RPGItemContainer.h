// RPGItemContainer.h
// Contract for anything that holds items (player inventory, chest, vendor stock).
// Crafting, Economy, Quest, Loot talk to containers ONLY through this interface.
// C++-only interface; Blueprint access goes through URPGCoreStatics.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "RPGItemContainer.generated.h"

class URPGItemDefinition;

UINTERFACE(MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class URPGItemContainer : public UInterface
{
	GENERATED_BODY()
};

class RPGCORE_API IRPGItemContainer
{
	GENERATED_BODY()

public:
	/** Total quantity of this item across all stacks. */
	virtual int32 GetItemCount(const URPGItemDefinition* Item) const = 0;

	/** True if the full quantity fits (stack limits, slot limits, weight...). */
	virtual bool CanAddItem(const URPGItemDefinition* Item, int32 Quantity) const = 0;

	/** Adds up to Quantity. Returns the amount actually added. */
	virtual int32 AddItem(const URPGItemDefinition* Item, int32 Quantity) = 0;

	/** Removes up to Quantity. Returns the amount actually removed. */
	virtual int32 RemoveItem(const URPGItemDefinition* Item, int32 Quantity) = 0;

	bool HasItem(const URPGItemDefinition* Item, int32 Quantity = 1) const
	{
		return GetItemCount(Item) >= Quantity;
	}
};
