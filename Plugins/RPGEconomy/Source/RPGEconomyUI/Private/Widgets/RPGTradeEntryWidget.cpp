// RPGTradeEntryWidget.cpp

#include "Widgets/RPGTradeEntryWidget.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Engine/Texture2D.h"
#include "InputCoreTypes.h"
#include "Items/RPGItemDefinition.h"
#include "Items/RPGItemTooltip.h"
#include "RPGCoreStatics.h"

#define LOCTEXT_NAMESPACE "RPGTradeEntry"

void URPGTradeEntryWidget::Setup(const URPGItemDefinition* InItem, int32 InQuantity, int64 InUnitPrice,
	bool bInIsBuyEntry, bool bInAffordable)
{
	Item = InItem;
	Quantity = InQuantity;
	UnitPrice = InUnitPrice;
	bIsBuyEntry = bInIsBuyEntry;

	if (IconImage)
	{
		if (Item && !Item->Icon.IsNull())
		{
			IconImage->SetBrushFromSoftTexture(Item->Icon);
			IconImage->SetVisibility(ESlateVisibility::HitTestInvisible);
		}
		else
		{
			IconImage->SetVisibility(ESlateVisibility::Hidden);
		}
	}

	if (NameText)
	{
		NameText->SetText(Item ? Item->DisplayName : FText::GetEmpty());
		NameText->SetColorAndOpacity(FSlateColor(URPGCoreStatics::GetItemRarityInfo(Item).Color));
	}

	if (QuantityText)
	{
		if (Quantity == INDEX_NONE)
		{
			QuantityText->SetText(LOCTEXT("Infinite", "∞"));
		}
		else if (Quantity > 1)
		{
			QuantityText->SetText(FText::Format(LOCTEXT("Quantity", "x{0}"), FText::AsNumber(Quantity)));
		}
		else
		{
			QuantityText->SetText(FText::GetEmpty());
		}
	}

	if (PriceText)
	{
		PriceText->SetText(FText::AsNumber(UnitPrice));
		PriceText->SetColorAndOpacity(FSlateColor(bInAffordable ? PriceColor : CannotAffordColor));
	}

	// Shared tooltip through the RPGCore interface (created once, reused).
	if (ItemTooltipClass && !ItemTooltip)
	{
		ItemTooltip = CreateWidget<UUserWidget>(this, ItemTooltipClass);
	}
	if (ItemTooltip && Item && ItemTooltip->Implements<URPGItemTooltip>())
	{
		IRPGItemTooltip::Execute_SetTooltipItem(ItemTooltip, Item.Get(), FMath::Max(Quantity, 1));
		SetToolTip(ItemTooltip);
	}
}

FReply URPGTradeEntryWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton && Item)
	{
		OnClicked.Broadcast(this, InMouseEvent.IsShiftDown());
		return FReply::Handled();
	}
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

void URPGTradeEntryWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
	BP_OnHoverChanged(true);
}

void URPGTradeEntryWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);
	BP_OnHoverChanged(false);
}

#undef LOCTEXT_NAMESPACE
