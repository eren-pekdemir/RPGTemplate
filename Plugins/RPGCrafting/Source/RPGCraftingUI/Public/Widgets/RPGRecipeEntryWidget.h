// RPGRecipeEntryWidget.h
// One row of the recipe list: icon, name (rarity colour), how many can be made.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RPGCraftingTypes.h"
#include "RPGRecipeEntryWidget.generated.h"

class UBorder;
class UImage;
class UTextBlock;
class URPGRecipeDefinition;
class URPGRecipeEntryWidget;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRPGOnRecipeEntryClickedSignature, URPGRecipeEntryWidget*, Entry);

/** Displays a recipe and reports clicks; the screen decides what selection means. */
UCLASS(Abstract)
class RPGCRAFTINGUI_API URPGRecipeEntryWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category = "RPG|Crafting")
	FRPGOnRecipeEntryClickedSignature OnClicked;

	void SetRecipe(const URPGRecipeDefinition* InRecipe);

	/** CanCraftResult for one craft; MaxCraftable from the ingredients. */
	void SetState(ERPGCraftResult CanCraftResult, int32 MaxCraftable);

	void SetSelected(bool bInSelected);

	const URPGRecipeDefinition* GetRecipe() const { return Recipe; }

protected:
	/** Optional: coloured with NormalColor / SelectedColor. Must be Visible to receive clicks. */
	UPROPERTY(BlueprintReadOnly, Category = "RPG|Crafting", meta = (BindWidgetOptional))
	TObjectPtr<UBorder> Background;

	UPROPERTY(BlueprintReadOnly, Category = "RPG|Crafting", meta = (BindWidgetOptional))
	TObjectPtr<UImage> IconImage;

	UPROPERTY(BlueprintReadOnly, Category = "RPG|Crafting", meta = (BindWidget))
	TObjectPtr<UTextBlock> NameText;

	/** Optional: "(3)" = can be made three times. */
	UPROPERTY(BlueprintReadOnly, Category = "RPG|Crafting", meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> CountText;

	UPROPERTY(EditDefaultsOnly, Category = "RPG|Crafting")
	FLinearColor NormalColor = FLinearColor(0.02f, 0.02f, 0.02f, 0.6f);

	UPROPERTY(EditDefaultsOnly, Category = "RPG|Crafting")
	FLinearColor SelectedColor = FLinearColor(0.25f, 0.18f, 0.05f, 0.9f);

	/** Render opacity of recipes that cannot be crafted right now. */
	UPROPERTY(EditDefaultsOnly, Category = "RPG|Crafting", meta = (ClampMin = "0", ClampMax = "1"))
	float UnavailableOpacity = 0.5f;

	UFUNCTION(BlueprintImplementableEvent, Category = "RPG|Crafting")
	void BP_OnStateChanged(bool bCanCraft);

	UFUNCTION(BlueprintImplementableEvent, Category = "RPG|Crafting")
	void BP_OnHoverChanged(bool bHovered);

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;

private:
	UPROPERTY()
	TObjectPtr<const URPGRecipeDefinition> Recipe;

	bool bSelected = false;
};
