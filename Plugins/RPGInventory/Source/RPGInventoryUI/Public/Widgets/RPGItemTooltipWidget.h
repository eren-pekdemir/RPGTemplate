// RPGItemTooltipWidget.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RPGInventoryTypes.h"
#include "Items/RPGItemTooltip.h"
#include "RPGItemTooltipWidget.generated.h"

class UTextBlock;
class URPGItemDefinition;

/**
 * 
 */
UCLASS(Abstract)
class RPGINVENTORYUI_API URPGItemTooltipWidget : public UUserWidget, public IRPGItemTooltip
{
	GENERATED_BODY()
public:
	
	UFUNCTION(BlueprintCallable, Category = "Inventory|Tooltip")
	void SetItem(const FRPGItemEntry& Entry);
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Inventory|Tooltip")
	void BP_OnItemSet(const URPGItemDefinition* Item);
	
	virtual void SetTooltipItem_Implementation(const URPGItemDefinition* Item, int32 Quantity) override;
protected:
	UPROPERTY(BlueprintReadOnly, Category = "Inventory|Tooltip", meta = (BindWidget))
	TObjectPtr<UTextBlock> NameText;
	
	UPROPERTY(BlueprintReadOnly, Category = "Inventory|Tooltip", meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> DescriptionText;
	
	UPROPERTY(BlueprintReadOnly, Category = "Inventory|Tooltip", meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> WeightText;
	
	UPROPERTY(BlueprintReadOnly, Category = "Inventory|Tooltip", meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> RarityText;
};
