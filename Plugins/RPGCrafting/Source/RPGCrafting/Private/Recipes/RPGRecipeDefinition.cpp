#include "Recipes/RPGRecipeDefinition.h"

#include "Items/RPGItemDefinition.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif

#define LOCTEXT_NAMESPACE "RPGRecipeDefinition"

FText URPGRecipeDefinition::GetDisplayName() const
{
    if (!DisplayName.IsEmpty())
    {
        return DisplayName;
    }

    const URPGItemDefinition* Output = GetPrimaryOutput();
    return Output ? Output->DisplayName : FText::FromString(GetName());
}

const URPGItemDefinition* URPGRecipeDefinition::GetPrimaryOutput() const
{
    for (const FRPGItemQuantity& Output : Outputs)
    {
        if (Output.IsValid())
        {
            return Output.Item;
        }
    }
    return nullptr;
}

#if WITH_EDITOR

EDataValidationResult URPGRecipeDefinition::IsDataValid(FDataValidationContext& Context) const
{
    EDataValidationResult Result = CombineDataValidationResults(Super::IsDataValid(Context), EDataValidationResult::Valid);

    auto ValidateList = [&](const TArray<FRPGItemQuantity>& List, const FText& ListName)
    {
        TSet<const URPGItemDefinition*> Seen;

        for (int32 Index = 0; Index < List.Num(); ++Index)
        {
            const FRPGItemQuantity& Entry = List[Index];

            if (!Entry.Item)
            {
                Context.AddError(FText::Format(LOCTEXT("NoItem", "{0} [{1}] has no item."), ListName, Index));
                Result = EDataValidationResult::Invalid;
                continue;
            }

            bool bAlreadySeen = false;
            Seen.Add(Entry.Item, &bAlreadySeen);
            if (bAlreadySeen)
            {
                Context.AddWarning(FText::Format(LOCTEXT("Duplicate", "{0}: {1} is listed twice; merge the quantities."),
                    ListName, Entry.Item->DisplayName));
            }
        }
    };

    ValidateList(Ingredients, LOCTEXT("Ingredients", "Ingredients"));
    ValidateList(Outputs, LOCTEXT("Outputs", "Outputs"));

    if (Outputs.IsEmpty())
    {
        Context.AddError(LOCTEXT("NoOutputs", "The recipe has no outputs."));
        Result = EDataValidationResult::Invalid;
    }

    if (Ingredients.IsEmpty())
    {
        Context.AddWarning(LOCTEXT("NoIngredients", "The recipe has no ingredients; it creates items from nothing."));
    }

    if (!Category.IsValid())
    {
        Context.AddWarning(LOCTEXT("NoCategory", "No category: the recipe only appears in the 'All' tab."));
    }

    return Result;
}

#endif // WITH_EDITOR

#undef LOCTEXT_NAMESPACE