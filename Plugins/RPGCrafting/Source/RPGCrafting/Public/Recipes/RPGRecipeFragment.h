// RPGRecipeFragment.h
// Item fragment that turns an item into a recipe scroll.

#pragma once

#include "CoreMinimal.h"
#include "Items/RPGItemFragment.h"
#include "RPGRecipeFragment.generated.h"

class URPGItemDefinition;
class URPGRecipeDefinition;

/** Using the item teaches the recipe (see URPGCraftingComponent::LearnRecipeFromItem). */
UCLASS(meta = (DisplayName = "Recipe"))
class RPGCRAFTING_API URPGRecipeFragment : public URPGItemFragment
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Recipe")
	TObjectPtr<const URPGRecipeDefinition> Recipe;

	/** The recipe an item teaches, or nullptr. */
	UFUNCTION(BlueprintPure, Category = "RPG|Crafting")
	static const URPGRecipeDefinition* GetRecipe(const URPGItemDefinition* Item);
};
