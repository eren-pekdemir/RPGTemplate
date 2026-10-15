// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/RPGInventorySlotWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Engine/Texture2D.h"
#include "Items/RPGItemDefinition.h"
#include "InputCoreTypes.h"
#include "RPGCoreStatics.h"
#include "Widgets/RPGItemTooltipWidget.h"

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
	
	UpdateRarity(Entry.Item); 
	BP_OnSlotUpdated(false);
	
	if(ItemTooltipClass && !ItemTooltip)
	{
		ItemTooltip = CreateWidget<URPGItemTooltipWidget>(this, ItemTooltipClass);
	}

	if (ItemTooltip)
	{
		ItemTooltip->SetItem(Entry);

		if (GetToolTip() != ItemTooltip)
		{
			SetToolTip(ItemTooltip);
		}
	}
	

}

void URPGInventorySlotWidget::ClearSlot()
{
	
	EntryId.Invalidate();
	IconImage->SetVisibility(ESlateVisibility::Hidden);
	if (QuantityText)
	{
		QuantityText->SetVisibility(ESlateVisibility::Collapsed);
	}
	
	UpdateRarity(nullptr); 
	BP_OnSlotUpdated(true);
	
	SetToolTip(nullptr);
}

FGuid URPGInventorySlotWidget::GetEntryId() const
{
	return EntryId;
}

bool URPGInventorySlotWidget::IsEmpty() const
{
	return !EntryId.IsValid();
}

FReply URPGInventorySlotWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
	{
		OnRightClicked.Broadcast(this, InMouseEvent.IsShiftDown());
		return FReply::Handled();
	}
	
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

void URPGInventorySlotWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
	
	BP_OnHoverChanged(true);
}

void URPGInventorySlotWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);
	
	BP_OnHoverChanged(false);
}

FReply URPGInventorySlotWidget::NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry,
	const FPointerEvent& InMouseEvent)
{
	FKey Key = InMouseEvent.GetEffectingButton();
	if (Key == EKeys::LeftMouseButton)
	{
		OnDoubleClicked.Broadcast(this);
		return FReply::Handled();
	}
	if (Key == EKeys::RightMouseButton)
	{
		return NativeOnMouseButtonDown(InGeometry, InMouseEvent);	
	}
	
	return Super::NativeOnMouseButtonDoubleClick(InGeometry, InMouseEvent);
}

void URPGInventorySlotWidget::UpdateRarity(const URPGItemDefinition* Item)
{
	if (!RarityFrame)
	{
		return;
	}

	if (!Item || !Item->Rarity.IsValid())
	{
		RarityFrame->SetVisibility(ESlateVisibility::Hidden);
		return;
	}

	RarityFrame->SetColorAndOpacity(URPGCoreStatics::GetItemRarityInfo(Item).Color);
	RarityFrame->SetVisibility(ESlateVisibility::HitTestInvisible);
}