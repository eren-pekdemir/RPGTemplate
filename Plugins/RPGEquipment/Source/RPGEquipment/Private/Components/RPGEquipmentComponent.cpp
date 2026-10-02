// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/RPGEquipmentComponent.h"

#include "RPGCoreStatics.h"
#include "RPGEquipment.h"
#include "Items/RPGEquipmentFragment.h"
#include "Items/RPGItemDefinition.h"
#include "RPGCoreTags.h"
#include "Interfaces/RPGItemContainer.h"

URPGEquipmentComponent::URPGEquipmentComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	
	// Humanoid defaults. Designers remove slots a character doesn't have (e.g. a horse).
	AvailableSlots.AddTag(RPGTags::Equipment_Slot_MainHand);
	AvailableSlots.AddTag(RPGTags::Equipment_Slot_OffHand);
	AvailableSlots.AddTag(RPGTags::Equipment_Slot_Head);
	AvailableSlots.AddTag(RPGTags::Equipment_Slot_Chest);
	AvailableSlots.AddTag(RPGTags::Equipment_Slot_Hands);
	AvailableSlots.AddTag(RPGTags::Equipment_Slot_Legs);
	AvailableSlots.AddTag(RPGTags::Equipment_Slot_Feet);
}

// Called when the game starts
void URPGEquipmentComponent::BeginPlay()
{
	Super::BeginPlay();
	
	if (AvailableSlots.IsEmpty())
	{
		UE_LOG(LogRPGEquipment, Warning,
			TEXT("%s: RPGEquipment has no Available Slots. Nothing can be equipped."),
			*GetNameSafe(GetOwner()));
		return;
	}
	
	for (const URPGItemDefinition* Item : StartingEquipment)
	{
		FGameplayTag TagFound = FindSlotForItem(Item);
		if (TagFound.IsValid())
		{
			SetSlotItem(TagFound, Item);
		}
		else
		{
			UE_LOG(LogRPGEquipment,Warning, TEXT("Can't find slot for item %s"), *GetNameSafe(Item));
		}
	}
}

void URPGEquipmentComponent::SetSlotItem(FGameplayTag Slot, const URPGItemDefinition* NewItem)
{
	if (!HasSlot(Slot))
	{
		UE_LOG(LogTemp, Warning, TEXT("Available slots has no slot"));
		return;
	}
	
	const URPGItemDefinition* OldItem = GetEquippedItem(Slot);
	
	if (OldItem == NewItem) return;
	
	if (NewItem)
	{
		EquippedItems.Add(Slot, NewItem);
	}
	else
	{
		EquippedItems.Remove(Slot);
	}
	
	OnEquipmentChanged.Broadcast(Slot, NewItem, OldItem);
}

const URPGItemDefinition* URPGEquipmentComponent::GetEquippedItem(FGameplayTag Slot) const
{
	const TObjectPtr<const URPGItemDefinition>* Found = EquippedItems.Find(Slot);
	return Found ? Found->Get() : nullptr;
}

bool URPGEquipmentComponent::IsSlotEmpty(FGameplayTag Slot) const
{
	return !EquippedItems.Contains(Slot);
}

bool URPGEquipmentComponent::HasSlot(FGameplayTag Slot) const
{
	return AvailableSlots.HasTagExact(Slot);
}
FGameplayTag URPGEquipmentComponent::FindSlotForItem(const URPGItemDefinition* Item) const
{	
	if (!Item) return FGameplayTag();
	FGameplayTag ItemTag = URPGEquipmentFragment::GetEquipSlot(Item);
	if (!ItemTag.IsValid()) return FGameplayTag();
	
	FGameplayTag FirstOccupiedMatch;
	
	for (const FGameplayTag& Slot  : AvailableSlots)
	{
		if (!Slot.MatchesTag(ItemTag))
		{
			continue;
		}
		
		if (IsSlotEmpty(Slot))
		{
			return Slot;	
		}
		
		if (!FirstOccupiedMatch.IsValid())
		{
			FirstOccupiedMatch = Slot;
		}
	}
	
	return FirstOccupiedMatch;
}

bool URPGEquipmentComponent::CanEquip(const URPGItemDefinition* Item) const
{
	return FindSlotForItem(Item).IsValid();
}

ERPGEquipResult URPGEquipmentComponent::UnequipToContainer(FGameplayTag Slot)
{
	const URPGItemDefinition* Item = GetEquippedItem(Slot);
	if (!Item) return ERPGEquipResult::NoValidSlot;
	
	IRPGItemContainer* ItemContainer = URPGCoreStatics::FindItemContainer(GetOwner());
	if (!ItemContainer) return ERPGEquipResult::NoContainer;
	
	int32 Added = ItemContainer->AddItem(Item, 1);
	if (Added == 0) return ERPGEquipResult::ContainerFull;
	SetSlotItem(Slot, nullptr);
	return ERPGEquipResult::Success;
}

ERPGEquipResult URPGEquipmentComponent::EquipFromContainer(const URPGItemDefinition* Item)
{
	FGameplayTag Slot = FindSlotForItem(Item);
	if (!Slot.IsValid()) return ERPGEquipResult::NoValidSlot;
	
	IRPGItemContainer* ItemContainer = URPGCoreStatics::FindItemContainer(GetOwner());
	if (!ItemContainer) return ERPGEquipResult::NoContainer;
	
	int32 ItemCount = ItemContainer->GetItemCount(Item);
	
	if (ItemCount < 1) return ERPGEquipResult::ItemNotInContainer;
	
	const URPGItemDefinition* OldItem = GetEquippedItem(Slot);
	
	int32 Removed = ItemContainer->RemoveItem(Item, 1);
	if (Removed == 0) return ERPGEquipResult::ItemNotInContainer;
	
	if (OldItem)
	{
		int32 Added = ItemContainer->AddItem(OldItem, 1);
		if (Added == 0)
		{
			const int32 Restored = ItemContainer->AddItem(Item, 1);
			ensureMsgf(Restored == 1, TEXT("EquipFromContainer: rollback failed, %s was lost"), *GetNameSafe(Item));
			return ERPGEquipResult::ContainerFull;
		}
	}
	
	SetSlotItem(Slot, Item);
	return ERPGEquipResult::Success;
}

