// RPGItemFragment.h
// Base class for item fragments. Each system defines its own fragment subclass
// (EquippableFragment in RPGEquipment, SellableFragment in RPGEconomy, ...) and
// designers attach them to item definitions in the editor.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "RPGItemFragment.generated.h"

UCLASS(Abstract, DefaultToInstanced, EditInlineNew, BlueprintType, Blueprintable)
class RPGCORE_API URPGItemFragment : public UObject
{
	GENERATED_BODY()
};
