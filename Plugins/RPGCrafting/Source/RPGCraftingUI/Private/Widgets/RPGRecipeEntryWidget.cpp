// RPGRecipeEntryWidget.cpp

#include "Widgets/RPGRecipeEntryWidget.h"

#include "Components/Border.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Engine/Texture2D.h"
#include "InputCoreTypes.h"
#include "Items/RPGItemDefinition.h"
#include "Recipes/RPGRecipeDefinition.h"
#include "RPGCoreStatics.h"

#define LOCTEXT_NAMESPACE "RPGRecipeEntry"

void URPGRecipeEntryWidget::SetRecipe(const URPGRecipeDefinition* InRecipe)
{
	Recipe = InRecipe;
	const URPGItemDefinition* Output = Recipe ? Recipe->GetPrimaryOutput() : nullptr;

	if (IconImage)
	{
		if (Output && !Output->Icon.IsNull())
		{
			IconImage->SetBrushFromSoftTexture(Output->Icon);
			IconImage->SetVisibility(ESlateVisibility::HitTestInvisible);
		}
		else
		{
			IconImage->SetVisibility(ESlateVisibility::Hidden);
		}
	}

	if (NameText)
	{
		int32 OutputQuantity = 1;
		if (Recipe)
		{
			for (const FRPGItemQuantity& RecipeOutput : Recipe->Outputs)
			{
				if (RecipeOutput.IsValid())
				{
					OutputQuantity = RecipeOutput.Quantity;
					break;
				}
			}
		}

		const FText Name = Recipe ? Recipe->GetDisplayName() : FText::GetEmpty();
		NameText->SetText(OutputQuantity > 1
			? FText::Format(LOCTEXT("NameWithCount", "{0} x{1}"), Name, FText::AsNumber(OutputQuantity))
			: Name);
		NameText->SetColorAndOpacity(FSlateColor(URPGCoreStatics::GetItemRarityInfo(Output).Color));
	}

	SetSelected(false);
}

void URPGRecipeEntryWidget::SetState(ERPGCraftResult CanCraftResult, int32 MaxCraftable)
{
	const bool bCanCraft = CanCraftResult == ERPGCraftResult::Success;

	if (CountText)
	{
		CountText->SetText(MaxCraftable > 0
			? FText::Format(LOCTEXT("Count", "({0})"), FText::AsNumber(MaxCraftable))
			: FText::GetEmpty());
	}

	SetRenderOpacity(bCanCraft ? 1.f : UnavailableOpacity);
	BP_OnStateChanged(bCanCraft);
}

void URPGRecipeEntryWidget::SetSelected(bool bInSelected)
{
	bSelected = bInSelected;

	if (Background)
	{
		Background->SetBrushColor(bSelected ? SelectedColor : NormalColor);
	}
}

FReply URPGRecipeEntryWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton && Recipe)
	{
		OnClicked.Broadcast(this);
		return FReply::Handled();
	}
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

void URPGRecipeEntryWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
	BP_OnHoverChanged(true);
}

void URPGRecipeEntryWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);
	BP_OnHoverChanged(false);
}

#undef LOCTEXT_NAMESPACE
