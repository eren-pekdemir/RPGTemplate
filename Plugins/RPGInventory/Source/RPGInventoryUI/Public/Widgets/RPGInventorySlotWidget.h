// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RPGInventoryTypes.h"
#include "RPGInventorySlotWidget.generated.h"

class UImage;
class UTextBlock;

/**
 * 
 */
UCLASS(Abstract)
class RPGINVENTORYUI_API URPGInventorySlotWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	
	UFUNCTION(BlueprintCallable, Category = "Inventory|Slot")
	void SetEntry(const FRPGItemEntry& Entry);
	
	UFUNCTION(BlueprintCallable, Category = "Inventory|Slot")
	void ClearSlot();
	
	UFUNCTION(BlueprintPure, Category = "Inventory|Slot")
	FGuid GetEntryId() const;
	
	UFUNCTION(BlueprintPure, Category = "Inventory|Slot")
	bool IsEmpty() const;
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Inventory|Slot")
	void BP_OnSlotUpdated(bool bIsEmpty);
	
protected:
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> IconImage;
	
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> QuantityText;
	
private:
	
	FGuid EntryId;
};
