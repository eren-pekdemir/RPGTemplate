// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/RPGEquipmentSlotWidget.h"
#include "Items/RPGItemDefinition.h"
#include "Components/Image.h"
#include "InputCoreTypes.h"

void URPGEquipmentSlotWidget::SetItem(const URPGItemDefinition* NewItem)
{
	Item = NewItem;

	const bool bHasIcon = Item && !Item->Icon.IsNull();

	if (bHasIcon)
	{
		IconImage->SetBrushFromSoftTexture(Item->Icon);
		IconImage->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
	else
	{
		IconImage->SetVisibility(ESlateVisibility::Hidden);
	}

	BP_OnSlotUpdated(Item == nullptr);
}
FGameplayTag URPGEquipmentSlotWidget::GetSlotTag() const
{
	return SlotTag;
}

const URPGItemDefinition* URPGEquipmentSlotWidget::GetItem() const
{
	return Item;
}

FReply URPGEquipmentSlotWidget::NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry,
	const FPointerEvent& InMouseEvent)
{
	FKey Key = InMouseEvent.GetEffectingButton();
	if (Key == EKeys::LeftMouseButton && Item)
	{
		OnDoubleClicked.Broadcast(this);
		return FReply::Handled();
	}
	return Super::NativeOnMouseButtonDoubleClick(InGeometry, InMouseEvent);
}
