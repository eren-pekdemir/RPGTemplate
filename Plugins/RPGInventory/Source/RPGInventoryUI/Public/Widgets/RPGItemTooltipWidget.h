// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RPGInventoryTypes.h"
#include "RPGItemTooltipWidget.generated.h"

class UTextBlock;
class URPGItemDefinition;

/**
 * 
 */
UCLASS(Abstract)
class RPGINVENTORYUI_API URPGItemTooltipWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	
	UFUNCTION(BlueprintCallable, Category = "Inventory|Tooltip")
	void SetItem(const FRPGItemEntry& Entry);
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Inventory|Tooltip")
	void BP_OnItemSet(const URPGItemDefinition* Item);
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> NameText;
	
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> DescriptionText;
	
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> WeightText;
};
