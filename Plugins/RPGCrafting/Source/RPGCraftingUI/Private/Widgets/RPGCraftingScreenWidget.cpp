// RPGCraftingScreenWidget.cpp

#include "Widgets/RPGCraftingScreenWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/PanelWidget.h"
#include "Components/ProgressBar.h"
#include "Components/RPGCraftingComponent.h"
#include "Components/RPGCraftingStationComponent.h"
#include "Components/TextBlock.h"
#include "Engine/Texture2D.h"
#include "Items/RPGItemDefinition.h"
#include "Recipes/RPGRecipeDefinition.h"
#include "RPGCoreStatics.h"
#include "Widgets/RPGRecipeEntryWidget.h"

#define LOCTEXT_NAMESPACE "RPGCraftingScreen"

namespace
{
	FText GetResultText(ERPGCraftResult Result)
	{
		switch (Result)
		{
		case ERPGCraftResult::Success:             return LOCTEXT("Success", "Ready");
		case ERPGCraftResult::InvalidRecipe:       return LOCTEXT("Invalid", "Invalid recipe");
		case ERPGCraftResult::UnknownRecipe:       return LOCTEXT("Unknown", "Recipe not known");
		case ERPGCraftResult::NoContainer:         return LOCTEXT("NoContainer", "No inventory");
		case ERPGCraftResult::MissingIngredients:  return LOCTEXT("Missing", "Missing ingredients");
		case ERPGCraftResult::WrongStation:        return LOCTEXT("Station", "Requires a crafting station");
		case ERPGCraftResult::MissingRequirements: return LOCTEXT("Requirements", "Requirements not met");
		case ERPGCraftResult::NoRoom:              return LOCTEXT("NoRoom", "Not enough inventory space");
		case ERPGCraftResult::Busy:                return LOCTEXT("Busy", "Already crafting");
		case ERPGCraftResult::Cancelled:           return LOCTEXT("Cancelled", "Cancelled");
		}
		return FText::GetEmpty();
	}

	/** "Crafting.Station.Forge" -> "Forge", "Skill.Smithing" -> "Smithing". */
	FString GetTagLeafName(const FGameplayTag& Tag)
	{
		FString Name = Tag.ToString();
		int32 DotIndex = INDEX_NONE;
		if (Name.FindLastChar(TEXT('.'), DotIndex))
		{
			Name.RightChopInline(DotIndex + 1);
		}
		return Name;
	}
}

// ===================================================================== Setup

void URPGCraftingScreenWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	// Bound once for the widget's lifetime; the widget is reused for every open.
	if (CraftButton)    { CraftButton->OnClicked.AddDynamic(this, &ThisClass::HandleCraftClicked); }
	if (CancelButton)   { CancelButton->OnClicked.AddDynamic(this, &ThisClass::HandleCancelClicked); }
	if (DecreaseButton) { DecreaseButton->OnClicked.AddDynamic(this, &ThisClass::HandleDecreaseClicked); }
	if (IncreaseButton) { IncreaseButton->OnClicked.AddDynamic(this, &ThisClass::HandleIncreaseClicked); }
	if (MaxButton)      { MaxButton->OnClicked.AddDynamic(this, &ThisClass::HandleMaxClicked); }
}

void URPGCraftingScreenWidget::NativeDestruct()
{
	SetCrafting(nullptr);
	Super::NativeDestruct();
}

void URPGCraftingScreenWidget::SetCrafting(URPGCraftingComponent* NewCrafting)
{
	// Unbind -> assign -> bind -> refresh
	if (Crafting)
	{
		Crafting->OnRecipeLearned.RemoveDynamic(this, &ThisClass::HandleRecipeLearned);
		Crafting->OnActiveStationChanged.RemoveDynamic(this, &ThisClass::HandleStationChanged);
		Crafting->OnCraftingStarted.RemoveDynamic(this, &ThisClass::HandleCraftingStarted);
		Crafting->OnItemCrafted.RemoveDynamic(this, &ThisClass::HandleItemCrafted);
		Crafting->OnCraftingFinished.RemoveDynamic(this, &ThisClass::HandleCraftingFinished);
	}

	Crafting = NewCrafting;

	if (Crafting)
	{
		Crafting->OnRecipeLearned.AddDynamic(this, &ThisClass::HandleRecipeLearned);
		Crafting->OnActiveStationChanged.AddDynamic(this, &ThisClass::HandleStationChanged);
		Crafting->OnCraftingStarted.AddDynamic(this, &ThisClass::HandleCraftingStarted);
		Crafting->OnItemCrafted.AddDynamic(this, &ThisClass::HandleItemCrafted);
		Crafting->OnCraftingFinished.AddDynamic(this, &ThisClass::HandleCraftingFinished);
	}

	MessageTimeLeft = 0.f;
	RebuildList();
}

void URPGCraftingScreenWidget::SetCategoryFilter(FGameplayTag Category)
{
	if (CategoryFilter != Category)
	{
		CategoryFilter = Category;
		RebuildList();
	}
}

void URPGCraftingScreenWidget::SelectRecipe(const URPGRecipeDefinition* Recipe)
{
	if (SelectedRecipe != Recipe)
	{
		SelectedRecipe = Recipe;
		Quantity = 1;
	}
	RefreshAll();
}

// ===================================================================== List

bool URPGCraftingScreenWidget::IsVisibleInList(const URPGRecipeDefinition* Recipe) const
{
	if (!Recipe || !Crafting)
	{
		return false;
	}

	const bool bCategoryMatches = !CategoryFilter.IsValid() || Recipe->Category.MatchesTag(CategoryFilter);
	const bool bStationMatches = bShowOtherStations || Crafting->IsAtRequiredStation(Recipe);
	return bCategoryMatches && bStationMatches;
}

void URPGCraftingScreenWidget::RebuildList()
{
	RefreshTitle();

	if (!RecipeList)
	{
		return;
	}

	RecipeList->ClearChildren();
	Entries.Reset();

	TArray<const URPGRecipeDefinition*> Visible;
	if (Crafting)
	{
		for (const URPGRecipeDefinition* Recipe : Crafting->KnownRecipes)
		{
			if (IsVisibleInList(Recipe))
			{
				Visible.Add(Recipe);
			}
		}
	}

	// TArray::Sort on pointers hands the predicate references to the objects.
	Visible.Sort([](const URPGRecipeDefinition& A, const URPGRecipeDefinition& B)
	{
		return A.GetDisplayName().CompareTo(B.GetDisplayName()) < 0;
	});

	if (EntryClass)
	{
		for (const URPGRecipeDefinition* Recipe : Visible)
		{
			URPGRecipeEntryWidget* Entry = CreateWidget<URPGRecipeEntryWidget>(this, EntryClass);
			Entry->SetRecipe(Recipe);
			Entry->OnClicked.AddDynamic(this, &ThisClass::HandleEntryClicked);
			RecipeList->AddChild(Entry);
			Entries.Add(Entry);
		}
	}

	// Keep the selection if it is still listed, otherwise select the first recipe.
	if (!SelectedRecipe || !Visible.Contains(SelectedRecipe.Get()))
	{
		SelectedRecipe = Visible.IsEmpty() ? nullptr : Visible[0];
		Quantity = 1;
	}

	RefreshAll();
}

void URPGCraftingScreenWidget::RefreshAll()
{
	ClampQuantity();
	RefreshList();
	RefreshDetails();
	RefreshControls();
}

void URPGCraftingScreenWidget::ClampQuantity()
{
	// While crafting the ingredients are being used up; keep the number the player chose.
	if (!Crafting || Crafting->IsCrafting())
	{
		return;
	}

	const int32 MaxCraftable = SelectedRecipe ? Crafting->GetMaxCraftable(SelectedRecipe) : 0;
	Quantity = FMath::Clamp(Quantity, 1, FMath::Clamp(MaxCraftable, 1, URPGCraftingComponent::MaxCraftCount));
}

void URPGCraftingScreenWidget::RefreshList()
{
	for (URPGRecipeEntryWidget* Entry : Entries)
	{
		const URPGRecipeDefinition* Recipe = Entry->GetRecipe();
		if (Crafting)
		{
			Entry->SetState(Crafting->CanCraft(Recipe, 1), Crafting->GetMaxCraftable(Recipe));
		}
		Entry->SetSelected(Recipe == SelectedRecipe);
	}
}

void URPGCraftingScreenWidget::RefreshTitle()
{
	if (!StationText)
	{
		return;
	}

	const URPGCraftingStationComponent* Station =
		Crafting ? Cast<URPGCraftingStationComponent>(Crafting->GetActiveStationSource()) : nullptr;

	StationText->SetText(Station && !Station->DisplayName.IsEmpty()
		? Station->DisplayName
		: LOCTEXT("Title", "Crafting"));
}

// ===================================================================== Details

void URPGCraftingScreenWidget::RefreshDetails()
{
	const URPGRecipeDefinition* Recipe = SelectedRecipe;
	const URPGItemDefinition* Output = Recipe ? Recipe->GetPrimaryOutput() : nullptr;

	if (OutputIcon)
	{
		if (Output && !Output->Icon.IsNull())
		{
			OutputIcon->SetBrushFromSoftTexture(Output->Icon);
			OutputIcon->SetVisibility(ESlateVisibility::HitTestInvisible);
		}
		else
		{
			OutputIcon->SetVisibility(ESlateVisibility::Hidden);
		}
	}

	if (OutputName)
	{
		OutputName->SetText(Recipe ? Recipe->GetDisplayName() : FText::GetEmpty());
		OutputName->SetColorAndOpacity(FSlateColor(URPGCoreStatics::GetItemRarityInfo(Output).Color));
	}

	if (DescriptionText)
	{
		FText Description;
		if (Recipe)
		{
			Description = !Recipe->Description.IsEmpty() ? Recipe->Description
				: (Output ? Output->Description : FText::GetEmpty());
		}
		DescriptionText->SetText(Description);
	}

	if (IngredientList)
	{
		IngredientList->ClearChildren();

		if (Recipe && Crafting)
		{
			for (const FRPGItemQuantity& Ingredient : Recipe->Ingredients)
			{
				if (!Ingredient.IsValid())
				{
					continue;
				}

				const int32 Have = URPGCoreStatics::GetItemCount(Crafting->GetOwner(), Ingredient.Item);
				const int32 Need = Ingredient.Quantity * Quantity;

				UTextBlock* Line = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
				Line->SetText(FText::Format(LOCTEXT("Ingredient", "{0}   {1} / {2}"),
					Ingredient.Item->DisplayName, FText::AsNumber(Have), FText::AsNumber(Need)));
				Line->SetColorAndOpacity(FSlateColor(Have >= Need ? HaveColor : MissingColor));

				// Keep the default font, only change its size (an empty FSlateFontInfo would draw nothing).
				FSlateFontInfo Font = Line->GetFont();
				Font.Size = IngredientFontSize;
				Line->SetFont(Font);

				IngredientList->AddChild(Line);
			}
		}
	}

	if (RequirementText)
	{
		TArray<FString> MissingNames;
		if (Recipe && Crafting)
		{
			if (!Crafting->IsAtRequiredStation(Recipe))
			{
				MissingNames.Add(GetTagLeafName(Recipe->RequiredStation));
			}
			for (const FGameplayTag& Tag : Crafting->GetMissingRequirements(Recipe))
			{
				MissingNames.Add(GetTagLeafName(Tag));
			}
		}

		if (MissingNames.IsEmpty())
		{
			RequirementText->SetVisibility(ESlateVisibility::Collapsed);
		}
		else
		{
			RequirementText->SetText(FText::Format(LOCTEXT("Requires", "Requires: {0}"),
				FText::FromString(FString::Join(MissingNames, TEXT(", ")))));
			RequirementText->SetColorAndOpacity(FSlateColor(MissingColor));
			RequirementText->SetVisibility(ESlateVisibility::HitTestInvisible);
		}
	}
}

// ===================================================================== Controls

void URPGCraftingScreenWidget::RefreshControls()
{
	const bool bIsCrafting = Crafting && Crafting->IsCrafting();
	const int32 MaxCraftable = (Crafting && SelectedRecipe) ? Crafting->GetMaxCraftable(SelectedRecipe) : 0;
	const ERPGCraftResult CanResult = (Crafting && SelectedRecipe)
		? Crafting->CanCraft(SelectedRecipe, Quantity)
		: ERPGCraftResult::InvalidRecipe;

	if (CraftButton)
	{
		CraftButton->SetIsEnabled(!bIsCrafting && CanResult == ERPGCraftResult::Success);
	}

	if (CancelButton)
	{
		CancelButton->SetVisibility(bIsCrafting ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}

	if (QuantityText)
	{
		QuantityText->SetText(FText::AsNumber(Quantity));
	}

	if (DecreaseButton) { DecreaseButton->SetIsEnabled(!bIsCrafting && Quantity > 1); }
	if (IncreaseButton) { IncreaseButton->SetIsEnabled(!bIsCrafting && Quantity < MaxCraftable); }
	if (MaxButton)      { MaxButton->SetIsEnabled(!bIsCrafting && MaxCraftable > 1 && Quantity < MaxCraftable); }

	if (CraftProgressBar)
	{
		CraftProgressBar->SetVisibility(bIsCrafting ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Hidden);
		CraftProgressBar->SetPercent(bIsCrafting ? Crafting->GetCraftProgress() : 0.f);
	}

	if (StatusText)
	{
		FText Status;
		if (MessageTimeLeft > 0.f)
		{
			Status = Message;
		}
		else if (bIsCrafting)
		{
			const int32 Done = Crafting->GetCraftedCount();
			const int32 Total = Done + Crafting->GetRemainingCount();
			const URPGRecipeDefinition* Active = Crafting->GetActiveRecipe();
			Status = FText::Format(LOCTEXT("Crafting", "Crafting {0}...  {1} / {2}"),
				Active ? Active->GetDisplayName() : FText::GetEmpty(),
				FText::AsNumber(Done + 1), FText::AsNumber(Total));
		}
		else if (SelectedRecipe && CanResult != ERPGCraftResult::Success)
		{
			Status = GetResultText(CanResult);
		}
		StatusText->SetText(Status);
	}
}

void URPGCraftingScreenWidget::ShowMessage(const FText& InMessage)
{
	Message = InMessage;
	MessageTimeLeft = MessageDuration;
}

// ===================================================================== Tick & input

void URPGCraftingScreenWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// Smooth progress every frame.
	if (CraftProgressBar && Crafting && Crafting->IsCrafting())
	{
		CraftProgressBar->SetPercent(Crafting->GetCraftProgress());
	}

	if (MessageTimeLeft > 0.f)
	{
		MessageTimeLeft -= InDeltaTime;
	}

	// Ingredient counts change without crafting events (pickups while the screen is open).
	RefreshTimer += InDeltaTime;
	if (RefreshTimer >= RefreshInterval)
	{
		RefreshTimer = 0.f;
		RefreshAll();
	}
}

FReply URPGCraftingScreenWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	// Modal: clicks on empty space must not reach the game (e.g. attack).
	Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
	return FReply::Handled();
}

// ===================================================================== Button handlers

void URPGCraftingScreenWidget::HandleEntryClicked(URPGRecipeEntryWidget* Entry)
{
	if (Entry)
	{
		SelectRecipe(Entry->GetRecipe());
	}
}

void URPGCraftingScreenWidget::HandleCraftClicked()
{
	if (!Crafting || !SelectedRecipe)
	{
		return;
	}

	const ERPGCraftResult Result = Crafting->StartCrafting(SelectedRecipe, Quantity);
	if (Result != ERPGCraftResult::Success)
	{
		ShowMessage(GetResultText(Result));
	}
	RefreshAll();
}

void URPGCraftingScreenWidget::HandleCancelClicked()
{
	if (Crafting)
	{
		Crafting->CancelCrafting();
	}
}

void URPGCraftingScreenWidget::HandleDecreaseClicked()
{
	Quantity = FMath::Max(1, Quantity - 1);
	RefreshAll();
}

void URPGCraftingScreenWidget::HandleIncreaseClicked()
{
	++Quantity;   // ClampQuantity keeps it within what can be made
	RefreshAll();
}

void URPGCraftingScreenWidget::HandleMaxClicked()
{
	Quantity = URPGCraftingComponent::MaxCraftCount;   // clamped down to the real maximum
	RefreshAll();
}

// ===================================================================== Component events

void URPGCraftingScreenWidget::HandleRecipeLearned(const URPGRecipeDefinition* Recipe)
{
	RebuildList();
}

void URPGCraftingScreenWidget::HandleStationChanged(FGameplayTag Station)
{
	RebuildList();
}

void URPGCraftingScreenWidget::HandleCraftingStarted(const URPGRecipeDefinition* Recipe, int32 Count)
{
	RefreshAll();
}

void URPGCraftingScreenWidget::HandleItemCrafted(const URPGRecipeDefinition* Recipe, int32 CraftedCount)
{
	RefreshAll();
}

void URPGCraftingScreenWidget::HandleCraftingFinished(const URPGRecipeDefinition* Recipe, int32 CraftedCount, ERPGCraftResult Result)
{
	const FText RecipeName = Recipe ? Recipe->GetDisplayName() : FText::GetEmpty();

	if (Result == ERPGCraftResult::Success)
	{
		ShowMessage(FText::Format(LOCTEXT("Crafted", "Crafted {0} x {1}"), FText::AsNumber(CraftedCount), RecipeName));
	}
	else if (CraftedCount > 0)
	{
		ShowMessage(FText::Format(LOCTEXT("Partial", "{0} ({1} crafted)"), GetResultText(Result), FText::AsNumber(CraftedCount)));
	}
	else
	{
		ShowMessage(GetResultText(Result));
	}

	BP_OnCraftFinished(Result, CraftedCount);
	RefreshAll();
}

#undef LOCTEXT_NAMESPACE
