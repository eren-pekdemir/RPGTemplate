// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RPGInventoryTypes.h"
#include "RPGInventorySlotWidget.generated.h"

class UImage;
class UTextBlock;
class URPGItemTooltipWidget;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FRPGOnSlotRightClickedSignature, URPGInventorySlotWidget*, SlotWidget, bool, bWholeStack);

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
	
	UFUNCTION(BlueprintImplementableEvent)
	void BP_OnHoverChanged(bool bHovered);
	
	UPROPERTY(BlueprintAssignable)
	FRPGOnSlotRightClickedSignature OnRightClicked;
	
protected:
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> IconImage;
	
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> QuantityText;
	
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<URPGItemTooltipWidget> ItemTooltipClass;
	
	
	
private:
	
	FGuid EntryId;
	
	UPROPERTY()
	TObjectPtr<URPGItemTooltipWidget> ItemTooltip;
};
