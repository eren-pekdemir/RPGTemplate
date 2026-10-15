// RPGCraftingScreenWidget.h
// Recipe list + details + quantity + craft / cancel + progress. All logic lives in URPGCraftingComponent.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayTagContainer.h"
#include "RPGCraftingTypes.h"
#include "RPGCraftingScreenWidget.generated.h"

class UButton;
class UImage;
class UPanelWidget;
class UProgressBar;
class UTextBlock;
class URPGCraftingComponent;
class URPGRecipeDefinition;
class URPGRecipeEntryWidget;

UCLASS(Abstract)
class RPGCRAFTINGUI_API URPGCraftingScreenWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/** nullptr clears the screen. Called by URPGCraftingUIComponent on open. */
	UFUNCTION(BlueprintCallable, Category = "RPG|Crafting")
	void SetCrafting(URPGCraftingComponent* NewCrafting);

	/** Category tab; empty tag = all recipes. Matches hierarchically. */
	UFUNCTION(BlueprintCallable, Category = "RPG|Crafting")
	void SetCategoryFilter(FGameplayTag Category);

	UFUNCTION(BlueprintCallable, Category = "RPG|Crafting")
	void SelectRecipe(const URPGRecipeDefinition* Recipe);

protected:
	// ------------------------------------------------------------------ List

	/** Any panel (ScrollBox, VerticalBox): one entry per visible recipe. */
	UPROPERTY(BlueprintReadOnly, Category = "RPG|Crafting", meta = (BindWidget))
	TObjectPtr<UPanelWidget> RecipeList;

	UPROPERTY(EditDefaultsOnly, Category = "RPG|Crafting")
	TSubclassOf<URPGRecipeEntryWidget> EntryClass;

	/** Also list recipes for other stations (greyed out). */
	UPROPERTY(EditDefaultsOnly, Category = "RPG|Crafting")
	bool bShowOtherStations = false;

	// ------------------------------------------------------------------ Details (all optional)

	UPROPERTY(BlueprintReadOnly, Category = "RPG|Crafting", meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> StationText;

	UPROPERTY(BlueprintReadOnly, Category = "RPG|Crafting", meta = (BindWidgetOptional))
	TObjectPtr<UImage> OutputIcon;

	UPROPERTY(BlueprintReadOnly, Category = "RPG|Crafting", meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> OutputName;

	UPROPERTY(BlueprintReadOnly, Category = "RPG|Crafting", meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> DescriptionText;

	/** Filled with one text line per ingredient: "Iron Ore  2 / 3". */
	UPROPERTY(BlueprintReadOnly, Category = "RPG|Crafting", meta = (BindWidgetOptional))
	TObjectPtr<UPanelWidget> IngredientList;

	/** "Requires: Forge, Smithing"; collapsed when nothing is missing. */
	UPROPERTY(BlueprintReadOnly, Category = "RPG|Crafting", meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> RequirementText;

	// ------------------------------------------------------------------ Controls

	UPROPERTY(BlueprintReadOnly, Category = "RPG|Crafting", meta = (BindWidget))
	TObjectPtr<UButton> CraftButton;

	UPROPERTY(BlueprintReadOnly, Category = "RPG|Crafting", meta = (BindWidgetOptional))
	TObjectPtr<UButton> CancelButton;

	UPROPERTY(BlueprintReadOnly, Category = "RPG|Crafting", meta = (BindWidgetOptional))
	TObjectPtr<UButton> DecreaseButton;

	UPROPERTY(BlueprintReadOnly, Category = "RPG|Crafting", meta = (BindWidgetOptional))
	TObjectPtr<UButton> IncreaseButton;

	UPROPERTY(BlueprintReadOnly, Category = "RPG|Crafting", meta = (BindWidgetOptional))
	TObjectPtr<UButton> MaxButton;

	UPROPERTY(BlueprintReadOnly, Category = "RPG|Crafting", meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> QuantityText;

	UPROPERTY(BlueprintReadOnly, Category = "RPG|Crafting", meta = (BindWidgetOptional))
	TObjectPtr<UProgressBar> CraftProgressBar;

	UPROPERTY(BlueprintReadOnly, Category = "RPG|Crafting", meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> StatusText;

	// ------------------------------------------------------------------ Style

	UPROPERTY(EditDefaultsOnly, Category = "RPG|Crafting|Style")
	FLinearColor HaveColor = FLinearColor(0.85f, 0.85f, 0.85f, 1.f);

	UPROPERTY(EditDefaultsOnly, Category = "RPG|Crafting|Style")
	FLinearColor MissingColor = FLinearColor(0.9f, 0.2f, 0.15f, 1.f);

	UPROPERTY(EditDefaultsOnly, Category = "RPG|Crafting|Style", meta = (ClampMin = "6"))
	int32 IngredientFontSize = 14;

	/** Ingredient counts are re-read this often (items can arrive while the screen is open). */
	UPROPERTY(EditDefaultsOnly, Category = "RPG|Crafting", meta = (ClampMin = "0.05", Units = "s"))
	float RefreshInterval = 0.25f;

	/** How long a result message ("Crafted 3 x Iron Sword") stays in StatusText. */
	UPROPERTY(EditDefaultsOnly, Category = "RPG|Crafting", meta = (ClampMin = "0", Units = "s"))
	float MessageDuration = 2.5f;

	/** Designer hook: sound, flash. */
	UFUNCTION(BlueprintImplementableEvent, Category = "RPG|Crafting")
	void BP_OnCraftFinished(ERPGCraftResult Result, int32 CraftedCount);

	virtual void NativeOnInitialized() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

private:
	void RebuildList();
	void RefreshAll();
	void ClampQuantity();
	void RefreshList();
	void RefreshDetails();
	void RefreshControls();
	void RefreshTitle();
	void ShowMessage(const FText& Message);

	bool IsVisibleInList(const URPGRecipeDefinition* Recipe) const;

	UFUNCTION() void HandleEntryClicked(URPGRecipeEntryWidget* Entry);
	UFUNCTION() void HandleCraftClicked();
	UFUNCTION() void HandleCancelClicked();
	UFUNCTION() void HandleDecreaseClicked();
	UFUNCTION() void HandleIncreaseClicked();
	UFUNCTION() void HandleMaxClicked();

	UFUNCTION() void HandleRecipeLearned(const URPGRecipeDefinition* Recipe);
	UFUNCTION() void HandleStationChanged(FGameplayTag Station);
	UFUNCTION() void HandleCraftingStarted(const URPGRecipeDefinition* Recipe, int32 Count);
	UFUNCTION() void HandleItemCrafted(const URPGRecipeDefinition* Recipe, int32 CraftedCount);
	UFUNCTION() void HandleCraftingFinished(const URPGRecipeDefinition* Recipe, int32 CraftedCount, ERPGCraftResult Result);

	UPROPERTY()
	TObjectPtr<URPGCraftingComponent> Crafting;

	UPROPERTY()
	TObjectPtr<const URPGRecipeDefinition> SelectedRecipe;

	UPROPERTY()
	TArray<TObjectPtr<URPGRecipeEntryWidget>> Entries;

	FGameplayTag CategoryFilter;
	int32 Quantity = 1;
	float RefreshTimer = 0.f;

	FText Message;
	float MessageTimeLeft = 0.f;
};
