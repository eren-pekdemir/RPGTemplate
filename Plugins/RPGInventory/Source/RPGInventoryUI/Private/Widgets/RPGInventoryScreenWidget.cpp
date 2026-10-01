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
	if (WeightBar && WeightText)
	{
		WeightBar->SetVisibility(ESlateVisibility::Collapsed);
		WeightText->SetVisibility(ESlateVisibility::Collapsed);
	}

	SetInventory(nullptr);
	
	Super::NativeDestruct();
}

void URPGInventoryScreenWidget::HandleWeightChanged(float NewWeight, float MaxWeight)
{
	UpdateWeight(NewWeight, MaxWeight);
}

void URPGInventoryScreenWidget::UpdateWeight(float Current, float Max)
{
	if (!WeightBar || !WeightText) return;
	if (!Inventory.Get() || !Inventory->bUseWeight)
	{
		WeightBar->SetVisibility(ESlateVisibility::Collapsed);
		WeightText->SetVisibility(ESlateVisibility::Collapsed);
		return;
	}
	
	WeightBar->SetVisibility(ESlateVisibility::HitTestInvisible);
	WeightText->SetVisibility(ESlateVisibility::HitTestInvisible);
	
	if (Max > 0)
	{
		WeightBar->SetPercent(Current/Max);
	}
	else
	{
		WeightBar->SetPercent(0);
	}
	
	if (Current > Max)
	{
		WeightBar->SetFillColorAndOpacity(OverweightColor);
	}
	else
	{
		WeightBar->SetFillColorAndOpacity(NormalColor);
	}
	
	FNumberFormattingOptions Options;
	Options.SetMaximumFractionalDigits(1);

	WeightText->SetText(FText::Format(
		LOCTEXT("WeightFormat", "{0} / {1} kg"),
		FText::AsNumber(Current, &Options),
		FText::AsNumber(Max, &Options)));
	
}

#undef LOCTEXT_NAMESPACE