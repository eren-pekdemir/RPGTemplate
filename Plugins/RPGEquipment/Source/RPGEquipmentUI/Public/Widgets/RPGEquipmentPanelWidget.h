// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "RPGEquipmentPanelWidget.generated.h"

class URPGEquipmentSlotWidget;
/**
 * 
 */
UCLASS(Abstract)
class RPGEQUIPMENTUI_API URPGEquipmentPanelWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	
	UFUNCTION(BlueprintCallable)
	void SetEquipment(URPGEquipmentComponent* NewEquipment);
	
	UFUNCTION()
	void HandleSlotDoubleClicked(URPGEquipmentSlotWidget* SlotWidget);
	
protected:
	
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> StatsText;
	
	virtual void NativeOnInitialized() override;
	
	virtual void NativeDestruct() override;
	
private:
	
	TWeakObjectPtr<URPGEquipmentComponent> Equipment;
	
	UPROPERTY()
	TArray<TObjectPtr<URPGEquipmentSlotWidget>> SlotWidgets;
	
	UFUNCTION()
	void HandleEquipmentChanged(FGameplayTag ChangedSlot,const URPGItemDefinition* NewItem, const URPGItemDefinition* OldItem);
	
	void Refresh();
	
	
};
