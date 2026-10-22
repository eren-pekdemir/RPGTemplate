// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/RPGInventoryScreenWidget.h"

#include "Components/ProgressBar.h"
#include "Widgets/RPGInventoryGridWidget.h"
#include "Components/RPGInventoryComponent.h"
#include "Components/TextBlock.h"

#define LOCTEXT_NAMESPACE "RPGInventoryScreen"

void URPGInventoryScreenWidget::SetInventory(URPGInventoryComponent* NewInventory)
{
	if (Inventory.IsValid())
	{
		Inventory->OnWeightChanged.RemoveDynamic(this, &ThisClass::HandleWeightChanged);
	}

	InventoryGrid->SetInventory(NewInventory);
	Inventory = NewInventory;

	if (NewInventory)
	{
		NewInventory->OnWeightChanged.AddDynamic(this, &ThisClass::HandleWeightChanged);
	}

	UpdateWeight(Inventory.IsValid() ? Inventory->GetCurrentWeight() : 0.f,
				 Inventory.IsValid() ? Inventory->MaxWeight : 0.f);
}

void URPGInventoryScreenWidget::SetFilter(FGameplayTag NewFilter)
{
	InventoryGrid->SetFilter(NewFilter);
	BP_OnFilterChanged(InventoryGrid->GetFilter());
}

void URPGInventoryScreenWidget::NativeDestruct()
{
	SetInventory(nullptr);   // also collapses the weight widgets
	
	Super::NativeDestruct();
}

void URPGInventoryScreenWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	InventoryGrid->OnEntryActivated.AddDynamic(this, &URPGInventoryScreenWidget::HandleEntryActivated);
}

void URPGInventoryScreenWidget::HandleWeightChanged(float NewWeight, float MaxWeight)
{
	UpdateWeight(NewWeight, MaxWeight);
}

void URPGInventoryScreenWidget::UpdateWeight(float Current, float Max)
{
	// Both widgets are optional and independent: a screen may have only the bar or only the text.
	const bool bShow = Inventory.Get() && Inventory->bUseWeight;
	const ESlateVisibility WeightVisibility = bShow ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed;

	if (WeightBar)
	{
		WeightBar->SetVisibility(WeightVisibility);
		if (bShow)
		{
			WeightBar->SetPercent(Max > 0 ? Current / Max : 0.f);
			WeightBar->SetFillColorAndOpacity(Current > Max ? OverweightColor : NormalColor);
		}
	}

	if (WeightText)
	{
		WeightText->SetVisibility(WeightVisibility);
		if (bShow)
		{
			FNumberFormattingOptions Options;
			Options.SetMaximumFractionalDigits(1);

			WeightText->SetText(FText::Format(
				LOCTEXT("WeightFormat", "{0} / {1} kg"),
				FText::AsNumber(Current, &Options),
				FText::AsNumber(Max, &Options)));
		}
	}

}

void URPGInventoryScreenWidget::HandleEntryActivated(const FRPGItemEntry& Entry)
{
	OnEntryActivated.Broadcast(Entry);
}

#undef LOCTEXT_NAMESPACE
