#pragma once

#include "CoreMinimal.h"
#include "RPGEquipmentTypes.generated.h"

UENUM(BlueprintType)
enum class ERPGEquipResult : uint8
{
	Success,
	NoValidSlot,         // item'ın slot'u bu karakterde yok
	NoContainer,         // owner'da envanter yok
	ItemNotInContainer,  // oyuncuda bu item yok
	ContainerFull        // eski item envantere sığmıyor
};
