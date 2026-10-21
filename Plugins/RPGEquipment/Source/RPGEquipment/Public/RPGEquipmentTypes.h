#pragma once

#include "CoreMinimal.h"
#include "RPGEquipmentTypes.generated.h"

UENUM(BlueprintType)
enum class ERPGEquipResult : uint8
{
	Success,
	NoValidSlot,         // this character has no slot for the item (or the slot is empty when unequipping)
	NoContainer,         // the owner has no item container
	ItemNotInContainer,  // the container does not hold the item
	ContainerFull        // the old item does not fit back into the container
};
