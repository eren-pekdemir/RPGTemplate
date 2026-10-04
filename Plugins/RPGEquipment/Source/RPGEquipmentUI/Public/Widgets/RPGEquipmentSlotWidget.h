// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Blueprint/UserWidget.h"
#include "RPGEquipmentSlotWidget.generated.h"



class UImage;
class URPGItemDefinition;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRPGOnEquipmentSlotDoubleClickedSignature , URPGEquipmentSlotWidget*, SlotWidget);
/**
 * 
 */
UCLASS(Abstract)
class RPGEQUIPMENTUI_API URPGEquipmentSlotWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	
	UPROPERTY(BlueprintAssignable)
	FRPGOnEquipmentSlotDoubleClickedSignature OnDoubleClicked;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (Categories = "Equipment.Slot"))
	FGameplayTag SlotTag;
	
	UFUNCTION(BlueprintCallable, Category = "Equipment")
	void SetItem(const URPGItemDefinition* NewItem);
	
	UFUNCTION(BlueprintPure, Category = "Equipment")
	FGameplayTag GetSlotTag() const;
	
	UFUNCTION(BlueprintPure, Category = "Equipment")
	const URPGItemDefinition* GetItem() const;
	
	UFUNCTION(BlueprintImplementableEvent)
	void BP_OnSlotUpdated(bool bIsEmpty);
	
	
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> IconImage;
	
	virtual FReply NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	
	UPROPERTY(EditAnywhere, Category = "Equipment|Tooltip", meta = (MustImplement = "/Script/RPGCore.RPGItemTooltip"))
	TSubclassOf<UUserWidget> ItemTooltipClass;
	
private:
	
	UPROPERTY()
	TObjectPtr<const URPGItemDefinition> Item;
	
	UPROPERTY()
	TObjectPtr<UUserWidget> ItemTooltip;
};
