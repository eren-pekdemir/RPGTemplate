// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/RPGItemTooltipWidget.h"
#include "RPGInventoryTypes.h"
#include "Items/RPGItemDefinition.h"
#include "Components/TextBlock.h"
#include "Items/RPGInventoryFragment.h"

#define LOCTEXT_NAMESPACE "RPGItemTooltip"

void URPGItemTooltipWidget::SetItem(const FRPGItemEntry& Entry)
{
	Execute_SetTooltipItem(this, Entry.Item,Entry.Quantity);
}

void URPGItemTooltipWidget::SetTooltipItem_Implementation(const URPGItemDefinition* Item, int32 Quantity)
{
	if (!Item) return;
	
	NameText->SetText(Item->DisplayName);
	
	if (DescriptionText)
	{
		if (!Item->Description.IsEmpty())
		{
			DescriptionText->SetText(Item->Description);
			DescriptionText->SetVisibility(ESlateVisibility::HitTestInvisible);
		}
		else
		{
			DescriptionText->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
	
	if (WeightText)
	{
		const float TotalWeight = URPGInventoryFragment::GetUnitWeight(Item) * Quantity;

		if (TotalWeight > 0.f)
		{
			FNumberFormattingOptions Options;
			Options.SetMaximumFractionalDigits(1);

			WeightText->SetText(FText::Format(
				LOCTEXT("TooltipWeight", "{0} kg"),
				FText::AsNumber(TotalWeight, &Options)));
			WeightText->SetVisibility(ESlateVisibility::HitTestInvisible);
		}
		else
		{
			WeightText->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
	
	BP_OnItemSet(Item);
}
#undef LOCTEXT_NAMESPACE
