// RPGTradeEntryWidget.h
// One row of the trade screen: an item the merchant sells, or an item the customer can sell.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RPGTradeEntryWidget.generated.h"

class UImage;
class UTextBlock;
class URPGItemDefinition;
class URPGTradeEntryWidget;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FRPGOnTradeEntryClickedSignature, URPGTradeEntryWidget*, Entry, bool, bAll);

/** Displays an item with quantity and unit price; reports clicks (Shift = as many as possible). */
UCLASS(Abstract)
class RPGECONOMYUI_API URPGTradeEntryWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category = "RPG|Trade")
	FRPGOnTradeEntryClickedSignature OnClicked;

	/** InQuantity INDEX_NONE = unlimited stock. */
	void Setup(const URPGItemDefinition* InItem, int32 InQuantity, int64 InUnitPrice, bool bInIsBuyEntry, bool bInAffordable);

	const URPGItemDefinition* GetItem() const { return Item; }
	int32 GetQuantity() const { return Quantity; }
	int64 GetUnitPrice() const { return UnitPrice; }
	bool IsBuyEntry() const { return bIsBuyEntry; }

protected:
	UPROPERTY(BlueprintReadOnly, Category = "RPG|Trade", meta = (BindWidgetOptional))
	TObjectPtr<UImage> IconImage;

	UPROPERTY(BlueprintReadOnly, Category = "RPG|Trade", meta = (BindWidget))
	TObjectPtr<UTextBlock> NameText;

	/** "x5", "∞", or empty for one. */
	UPROPERTY(BlueprintReadOnly, Category = "RPG|Trade", meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> QuantityText;

	UPROPERTY(BlueprintReadOnly, Category = "RPG|Trade", meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> PriceText;

	/** Any widget implementing IRPGItemTooltip (e.g. the inventory tooltip). */
	UPROPERTY(EditDefaultsOnly, Category = "RPG|Trade", meta = (MustImplement = "/Script/RPGCore.RPGItemTooltip"))
	TSubclassOf<UUserWidget> ItemTooltipClass;

	UPROPERTY(EditDefaultsOnly, Category = "RPG|Trade")
	FLinearColor PriceColor = FLinearColor(1.f, 0.82f, 0.3f, 1.f);

	UPROPERTY(EditDefaultsOnly, Category = "RPG|Trade")
	FLinearColor CannotAffordColor = FLinearColor(0.9f, 0.2f, 0.15f, 1.f);

	UFUNCTION(BlueprintImplementableEvent, Category = "RPG|Trade")
	void BP_OnHoverChanged(bool bHovered);

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;

private:
	UPROPERTY()
	TObjectPtr<const URPGItemDefinition> Item;

	UPROPERTY()
	TObjectPtr<UUserWidget> ItemTooltip;

	int32 Quantity = 0;
	int64 UnitPrice = 0;
	bool bIsBuyEntry = true;
};
