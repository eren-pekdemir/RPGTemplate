// RPGEquipmentFragment.cpp

#include "Items/RPGEquipmentFragment.h"

#include "Items/RPGItemDefinition.h"

FGameplayTag URPGEquipmentFragment::GetEquipSlot(const URPGItemDefinition* Item)
{
	const URPGEquipmentFragment* EquipmentFragment = Item ? Item->FindFragmentByClass<URPGEquipmentFragment>() : nullptr;
	if (!EquipmentFragment)
	{
		return FGameplayTag();
	}
	
	return EquipmentFragment->EquipSlot;
}
