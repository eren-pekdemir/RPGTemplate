// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/RPGInventorySlotWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Engine/Texture2D.h"
#include "Items/RPGItemDefinition.h"


void URPGInventorySlotWidget::SetEntry(const FRPGItemEntry& Entry)
{
	if (!Entry.IsValid())
	{
		ClearSlot();
		return;
	}
	
	EntryId = Entry.EntryId;
	IconImage->SetBrushFromSoftTexture(Entry.Item->Icon);
	IconImage->SetVisibility(ESlateVisibility::HitTestInvisible);
	
	
	if (QuantityText)
	{
		if (Entry.Quantity > 1)
		{
			QuantityText->SetText(FText::AsNumber(Entry.Quantity));
			QuantityText->SetVisibility(ESlateVisibility::HitTestInvisible);
		}
		else
		{
			QuantityText->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
	
	
	BP_OnSlotUpdated(false);
}

void URPGInventorySlotWidget::ClearSlot()
{
	
	EntryId.Invalidate();
	IconImage->SetVisibility(ESlateVisibility::Hidden);
	if (QuantityText)
	{
		QuantityText->SetVisibility(ESlateVisibility::Collapsed);
	}
	
	BP_OnSlotUpdated(true);
}

FGuid URPGInventorySlotWidget::GetEntryId() const
{
	return EntryId;
}

bool URPGInventorySlotWidget::IsEmpty() const
{
	return !EntryId.IsValid();
}
