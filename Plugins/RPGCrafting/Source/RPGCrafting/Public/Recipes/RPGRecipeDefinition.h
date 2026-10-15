#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "Items/RPGItemQuantity.h"
#include "RPGRecipeDefinition.generated.h"

class URPGItemDefinition;

/** What goes in, what comes out, and where / by whom it can be made. */
UCLASS(BlueprintType, Const)
class RPGCRAFTING_API URPGRecipeDefinition : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    /** Optional; empty = the first output's name. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Display")
    FText DisplayName;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Display", meta = (MultiLine = true))
    FText Description;

    /** UI tab. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Display", meta = (Categories = "Crafting.Category"))
    FGameplayTag Category;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Recipe", meta = (TitleProperty = "Item"))
    TArray<FRPGItemQuantity> Ingredients;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Recipe", meta = (TitleProperty = "Item"))
    TArray<FRPGItemQuantity> Outputs;

    /** Seconds per craft; 0 = instant. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Recipe", meta = (ClampMin = "0", Units = "s"))
    float CraftTime = 0.f;

    /** Station the crafter must be using; empty = anywhere. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Requirements", meta = (Categories = "Crafting.Station"))
    FGameplayTag RequiredStation;

    /** Tags the crafter must have, e.g. Skill.Smithing from a progression skill. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Requirements")
    FGameplayTagContainer RequiredTags;

    UFUNCTION(BlueprintPure, Category = "RPG|Crafting")
    FText GetDisplayName() const;

    /** First output: the recipe's icon, name and rarity in the UI. */
    UFUNCTION(BlueprintPure, Category = "RPG|Crafting")
    const URPGItemDefinition* GetPrimaryOutput() const;

#if WITH_EDITOR
    virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif
};