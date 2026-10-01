// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/RPGItemTooltipWidget.h"
#include "RPGInventoryTypes.h"
#include "Items/RPGItemDefinition.h"
#include "Components/TextBlock.h"
#include "Items/RPGInventoryFragment.h"

#define LOCTEXT_NAMESPACE "RPGItemTooltip"

void URPGItemTooltipWidget::SetItem(const FRPGItemEntry& Entry)
{
	if (!Entry.IsValid()) return;
	
	NameText->SetText(Entry.Item->DisplayName);
	
	if (DescriptionText)
	{
		if (!Entry.Item->Description.IsEmpty())
		{
			DescriptionText->SetText(Entry.Item->Description);
			DescriptionText->SetVisibility(ESlateVisibility::HitTestInvisible);
		}
		else
		{
			DescriptionText->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
	
	if (WeightText)
	{
		const float TotalWeight = URPGInventoryFragment::GetUnitWeight(Entry.Item) * Entry.Quantity;

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
	
	BP_OnItemSet(Entry.Item);
}
#undef LOCTEXT_NAMESPACE