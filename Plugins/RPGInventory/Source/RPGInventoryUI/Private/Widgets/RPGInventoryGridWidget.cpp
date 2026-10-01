// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/RPGInventoryGridWidget.h"

#include "Components/RPGInventoryComponent.h"
#include "Widgets/RPGInventorySlotWidget.h"
#include "Components/UniformGridPanel.h"
#include "Items/RPGItemDefinition.h"

void URPGInventoryGridWidget::SetInventory(URPGInventoryComponent* NewInventory)
{
	if (Inventory.Get())
	{
		Inventory->OnItemAdded.RemoveDynamic(this, &URPGInventoryGridWidget::OnItemAddedHandler);
		Inventory->OnItemRemoved.RemoveDynamic(this, &URPGInventoryGridWidget::OnItemRemovedHandler);
		Inventory->OnInventoryRefreshed.RemoveDynamic(this, &URPGInventoryGridWidget::OnInventoryRefreshedHandler);
	}
	
	Inventory = NewInventory;
	
	if (NewInventory)
	{
		NewInventory->OnItemAdded.AddDynamic(this, &URPGInventoryGridWidget::OnItemAddedHandler);
		NewInventory->OnItemRemoved.AddDynamic(this, &URPGInventoryGridWidget::OnItemRemovedHandler);
		NewInventory->OnInventoryRefreshed.AddDynamic(this, &URPGInventoryGridWidget::OnInventoryRefreshedHandler);
	}
	
	Refresh();
}

void URPGInventoryGridWidget::SetFilter(FGameplayTag NewFilter)
{
	if (NewFilter != FilterTag)
	{
		FilterTag = NewFilter;
		Refresh();
	}
}

FGameplayTag URPGInventoryGridWidget::GetFilter() const
{
	return FilterTag;
}

void URPGInventoryGridWidget::NativeDestruct()
{
	SetInventory(nullptr);
	
	Super::NativeDestruct();
}

void URPGInventoryGridWidget::BuildSlots()
{
	if (!SlotWidgetClass) return;
	
	SlotGrid->ClearChildren();
	SlotWidgets.Reset();
	
	for (int32 i = 0; i < Inventory->MaxSlots; i++)
	{
		URPGInventorySlotWidget* NewSlot = CreateWidget<URPGInventorySlotWidget>(this, SlotWidgetClass);
		int32 ColumnIndex = i % Columns;
		int32 RowIndex = i / Columns;
		SlotGrid->AddChildToUniformGrid(NewSlot,RowIndex,ColumnIndex );
		SlotWidgets.Add(NewSlot);
		NewSlot->OnRightClicked.AddDynamic(this, &ThisClass::HandleSlotRightClicked);
	}
}

void URPGInventoryGridWidget::Refresh()
{
	if (!Inventory.IsValid())
	{
		for (URPGInventorySlotWidget* SlotWidget : SlotWidgets)
		{
			SlotWidget->ClearSlot();
		}
		return;
	}
	
	if (SlotWidgets.Num() != Inventory->MaxSlots)
	{
		BuildSlots();
	}
	
	TArray<FRPGItemEntry> VisibleEntries;
	
	for (const FRPGItemEntry& Entry : Inventory->GetEntries())
	{
		if (!FilterTag.IsValid() || Entry.Item->ItemType.MatchesTag(FilterTag))
		{
			VisibleEntries.Add(Entry);
		}
	}
	
	for (int32 i = 0; i < SlotWidgets.Num(); i++)
	{
		if (VisibleEntries.IsValidIndex(i))
		{
			SlotWidgets[i]->SetEntry(VisibleEntries[i]);
		}
		else
		{
			SlotWidgets[i]->ClearSlot();
		}
	}
}

void URPGInventoryGridWidget::OnItemAddedHandler(const FRPGItemEntry& Entry, int32 AddedQuantity)
{
	Refresh();
}

void URPGInventoryGridWidget::OnItemRemovedHandler(const FRPGItemEntry& Entry, int32 RemovedQuantity)
{
	Refresh();
}

void URPGInventoryGridWidget::OnInventoryRefreshedHandler()
{
	Refresh();
}

void URPGInventoryGridWidget::HandleSlotRightClicked(URPGInventorySlotWidget* SlotWidget, bool bWholeStack)
{
	if (!Inventory.IsValid() || !SlotWidget) return;
	
	FGuid EntryId = SlotWidget->GetEntryId();
	FRPGItemEntry FoundEntry;
	
	if (bWholeStack)
	{
		if (Inventory->FindEntry(EntryId, FoundEntry))
		{
			Inventory->DropEntry(EntryId,FoundEntry.Quantity);
		}
	}
	else
	{	
		Inventory->DropEntry(EntryId, 1);
	}
}
