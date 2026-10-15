// RPGCraftingTypes.h
// Shared crafting types (also used by RPGCraftingUI).

#pragma once

#include "CoreMinimal.h"
#include "RPGCraftingTypes.generated.h"

UENUM(BlueprintType)
enum class ERPGCraftResult : uint8
{
	Success,
	InvalidRecipe,
	UnknownRecipe,
	NoContainer,
	MissingIngredients,
	WrongStation,
	MissingRequirements,
	/** Only known while crafting: removing the ingredients may free the space the outputs need. */
	NoRoom,
	/** A timed craft is already running. */
	Busy,
	/** A timed craft was cancelled; the current item's ingredients were returned. */
	Cancelled
};
