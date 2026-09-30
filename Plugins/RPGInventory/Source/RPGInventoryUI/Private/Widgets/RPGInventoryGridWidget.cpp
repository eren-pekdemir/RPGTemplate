// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/RPGInventoryGridWidget.h"

#include "Components/RPGInventoryComponent.h"
#include "Widgets/RPGInventorySlotWidget.h"
#include "Components/UniformGridPanel.h"

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
	
	TArray<FRPGItemEntry> Entries = Inventory->GetEntries();
	
	for (int32 i = 0; i < SlotWidgets.Num(); i++)
	{
		if (Entries.IsValidIndex(i))
		{
			SlotWidgets[i]->SetEntry(Entries[i]);
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
