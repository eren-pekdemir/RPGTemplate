// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/RPGInventoryComponent.h"
#include "RPGInventoryTypes.h"
#include "Items/RPGInventoryFragment.h"
#include "Items/RPGItemDefinition.h"
#include "RPGInventory.h"

namespace
{
	/** One change made by AddItem: a copy of the entry after the change, and how many units went into it. */
	struct FInventoryChange
	{
		FRPGItemEntry Entry;
		int32 Quantity = 0;
	};
}


URPGInventoryComponent::URPGInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

// Called when the game starts
void URPGInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	bSuppressEvents = true;
	
	for (const FRPGStartingItem& StartItem : StartingItems)
	{
		if (StartItem.Item == nullptr) continue;

		const int32 Added = AddItem(StartItem.Item, StartItem.Quantity);

		if (Added < StartItem.Quantity)
		{
			UE_LOG(LogRPGInventory, Warning,
				TEXT("%s: Starting item '%s' only partially added (%d / %d). Check MaxSlots, MaxWeight or MaxStackSize."),
				*GetNameSafe(GetOwner()),
				*GetNameSafe(StartItem.Item),
				Added,
				StartItem.Quantity);
		}
	}
	
	bSuppressEvents = false;
	
	OnInventoryRefreshed.Broadcast();
}


int32 URPGInventoryComponent::ComputeAddableQuantity(const URPGItemDefinition* Item, int32 Quantity) const
{
	if (!Item || Quantity <= 0)
	{
		return 0;
	}

	const int32 MaxStack = FMath::Max(1, Item->MaxStackSize);
	
	int32 WeightLimit = Quantity;
	const float UnitWeight = URPGInventoryFragment::GetUnitWeight(Item);

	if (bUseWeight && !bAllowOverweight && UnitWeight > 0.f)
	{
		const float RemainingWeight = MaxWeight - CurrentWeight;
		WeightLimit = FMath::Max(0, FMath::FloorToInt(RemainingWeight / UnitWeight));
	}
	
	int32 SlotLimit = 0;

	for (const FRPGItemEntry& Entry : Entries)
	{
		if (Entry.CanStackWith(Item))
		{
			SlotLimit += FMath::Max(0, MaxStack - Entry.Quantity);
		}
	}

	const int32 FreeSlots = FMath::Max(0, MaxSlots - Entries.Num());
	SlotLimit += FreeSlots * MaxStack;


	return FMath::Min3(Quantity, WeightLimit, SlotLimit);
}

int32 URPGInventoryComponent::FindEntryIndex(FGuid EntryId) const
{
	return Entries.IndexOfByPredicate([&](const FRPGItemEntry& E) { return E.EntryId == EntryId; });
}

int32 URPGInventoryComponent::GetItemCount(const URPGItemDefinition* Item) const
{
	int32 ItemCount = 0;
	for (const FRPGItemEntry& Entry : Entries)
	{
    		if (Entry.Item == Item)
    		{
    			ItemCount += Entry.Quantity;
    		}
	}
	return ItemCount;
}

bool URPGInventoryComponent::CanAddItem(const URPGItemDefinition* Item, int32 Quantity) const
{
	return ComputeAddableQuantity(Item, Quantity) == Quantity;
}

int32 URPGInventoryComponent::AddItem(const URPGItemDefinition* Item, int32 Quantity)
{
	// [1] How many units can we actually take?
	const int32 ToAdd = ComputeAddableQuantity(Item, Quantity);
	if (ToAdd <= 0)
	{
		return 0;
	}

	const int32 MaxStack = FMath::Max(1, Item->MaxStackSize);
	int32 Remaining = ToAdd;

	// Change log: copies taken while mutating, broadcast only at the very end.
	TArray<FInventoryChange> Changes;

	// [2] Top up existing stacks first
	for (int32 Index = 0; Index < Entries.Num() && Remaining > 0; ++Index)
	{
		FRPGItemEntry& Entry = Entries[Index];
		if (!Entry.CanStackWith(Item))
		{
			continue;
		}

		const int32 Space = FMath::Max(0, MaxStack - Entry.Quantity);
		const int32 Added = FMath::Min(Space, Remaining);
		if (Added <= 0)
		{
			continue;
		}

		Entry.Quantity += Added;
		Remaining -= Added;
		Changes.Add({ Entry, Added });
	}
	
	while (Remaining > 0)
	{
		FRPGItemEntry NewEntry;
		NewEntry.EntryId = FGuid::NewGuid();
		NewEntry.Item = Item;
		NewEntry.Quantity = FMath::Min(MaxStack, Remaining);

		Entries.Add(NewEntry);
		Remaining -= NewEntry.Quantity;
		Changes.Add({ NewEntry, NewEntry.Quantity });
	}

	// ComputeAddableQuantity and the placement above must always agree.
	ensureMsgf(Remaining == 0, TEXT("AddItem placed fewer items than ComputeAddableQuantity allowed"));
	
	RecalculateWeight();
	
	if (!bSuppressEvents)
	{
		for (const FInventoryChange& Change : Changes)
		{
			OnItemAdded.Broadcast(Change.Entry, Change.Quantity);
		}
	}
	
	
	return ToAdd;
}

int32 URPGInventoryComponent::RemoveItem(const URPGItemDefinition* Item, int32 Quantity)
{
	if (!Item || Quantity <= 0) return 0;
	int32 ToRemove = Quantity;
	
	TArray<FInventoryChange> RemoveChanges;

	for (int32 i = Entries.Num() - 1 ; i >= 0 && ToRemove > 0; --i)
	{
		if (Entries[i].Item == Item)
		{
			int32 Removed = FMath::Min(ToRemove, Entries[i].Quantity);
			Entries[i].Quantity -= Removed;
			RemoveChanges.Add({ Entries[i], Removed });
			if (Entries[i].Quantity <= 0)
			{
				Entries.RemoveAt(i);
			}
			ToRemove -= Removed;
		}
	}
	
	RecalculateWeight();
	
	for (const FInventoryChange& Change : RemoveChanges)
	{
		OnItemRemoved.Broadcast(Change.Entry, Change.Quantity);
	}
	
	return Quantity - ToRemove;
}

const TArray<FRPGItemEntry>& URPGInventoryComponent::GetEntries() const
{
	return Entries;
}

bool URPGInventoryComponent::FindEntry(FGuid EntryId, FRPGItemEntry& OutEntry) const
{
	int32 Index = FindEntryIndex(EntryId);
	if (Index == INDEX_NONE) return false;
	
	OutEntry = Entries[Index];
	
	return true;
}

int32 URPGInventoryComponent::RemoveEntry(FGuid EntryId, int32 Quantity)
{
	if (Quantity <= 0) return 0;
	
	int32 Index = FindEntryIndex(EntryId);
	
	if (Index == INDEX_NONE) return 0;
	
	int32 Removed = FMath::Min(Quantity, Entries[Index].Quantity);
	
	Entries[Index].Quantity -= Removed;
	
	FRPGItemEntry RemovedItem = Entries[Index];
	
	if (Entries[Index].Quantity <= 0)
	{
		Entries.RemoveAt(Index);
	}
	
	RecalculateWeight();
	
	OnItemRemoved.Broadcast(RemovedItem, Removed);
	
	return Removed;
}

int32 URPGInventoryComponent::GetUsedSlots() const
{
	return Entries.Num();
}

float URPGInventoryComponent::GetCurrentWeight() const
{
	return CurrentWeight;
}

bool URPGInventoryComponent::IsOverweight() const
{
	return bUseWeight && CurrentWeight > MaxWeight;
}

void URPGInventoryComponent::RecalculateWeight()
{
	float NewWeight = 0.f;
	
	for (const FRPGItemEntry& Entry : Entries)
	{
		NewWeight += URPGInventoryFragment::GetUnitWeight(Entry.Item) * Entry.Quantity;
	}
	
	if (FMath::IsNearlyEqual(NewWeight, CurrentWeight))
	{
		return;
	}
	
	CurrentWeight = NewWeight;
	
	if (!bSuppressEvents)
	{
		OnWeightChanged.Broadcast(CurrentWeight, MaxWeight);
	}
}


