// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/RPGEquipmentPanelWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/RPGEquipmentComponent.h"
#include "Widgets/RPGEquipmentSlotWidget.h"
#include "Components/TextBlock.h"

#define LOCTEXT_NAMESPACE "RPGEquipmentPanel"

void URPGEquipmentPanelWidget::SetEquipment(URPGEquipmentComponent* NewEquipment)
{
	if (Equipment.Get())
	{
		Equipment->OnEquipmentChanged.RemoveDynamic(this, &ThisClass::HandleEquipmentChanged);
	}
	
	Equipment = NewEquipment;
	
	if (NewEquipment)
	{
		NewEquipment->OnEquipmentChanged.AddDynamic(this, &ThisClass::HandleEquipmentChanged);
	}
	
	Refresh();
}

void URPGEquipmentPanelWidget::HandleSlotDoubleClicked(URPGEquipmentSlotWidget* SlotWidget)
{
	if (!SlotWidget) return;
	if (!Equipment.IsValid()) return;

	const FGameplayTag SlotTag = SlotWidget->GetSlotTag();
	if (!SlotTag.IsValid()) return;

	Equipment->UnequipToContainer(SlotTag);
}

void URPGEquipmentPanelWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	WidgetTree->ForEachWidget([this](UWidget* Widget)
{
	if (URPGEquipmentSlotWidget* SlotWidget = Cast<URPGEquipmentSlotWidget>(Widget))
	{
		SlotWidgets.Add(SlotWidget);
		SlotWidget->OnDoubleClicked.AddDynamic(this, &ThisClass::HandleSlotDoubleClicked);
	}
});
}

void URPGEquipmentPanelWidget::NativeDestruct()
{
	SetEquipment(nullptr);
	Super::NativeDestruct();
}

void URPGEquipmentPanelWidget::HandleEquipmentChanged(FGameplayTag ChangedSlot,const URPGItemDefinition* NewItem, const URPGItemDefinition* OldItem)
{
	Refresh();
}

void URPGEquipmentPanelWidget::Refresh()
{
	for (URPGEquipmentSlotWidget *SlotWidget : SlotWidgets)
	{
		if (!SlotWidget) continue;
		SlotWidget->SetItem(Equipment.IsValid() ? Equipment->GetEquippedItem(SlotWidget->GetSlotTag()) : nullptr );
	}	
	
	if (!StatsText) return;
	
	const TMap<FGameplayTag, float> Stats = Equipment.IsValid() ? Equipment->GetAllStatModifiers() : TMap<FGameplayTag, float>();
	
	if (Stats.IsEmpty())
	{
		StatsText->SetVisibility(ESlateVisibility::Collapsed);
		return;
	}
	
	TArray<FText> Lines;
	
	for (auto& Pair : Stats)
	{
		FString Name = Pair.Key.ToString();
		int32 DotIndex;
		if (Name.FindLastChar(TEXT('.'), DotIndex))
		{
			Name.RightChopInline(DotIndex + 1);
		}
		Lines.Add(FText::Format(
			LOCTEXT("StatLine", "{0}: {1}"),
			FText::FromString(Name),
			FText::AsNumber(Pair.Value)));
	}
	StatsText->SetText(FText::Join(FText::FromString(TEXT("\n")), Lines));
	StatsText->SetVisibility(ESlateVisibility::HitTestInvisible);
}

#undef LOCTEXT_NAMESPACE