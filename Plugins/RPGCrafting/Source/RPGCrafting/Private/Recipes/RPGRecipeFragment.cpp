// RPGRecipeFragment.cpp

#include "Recipes/RPGRecipeFragment.h"

#include "Items/RPGItemDefinition.h"
#include "Recipes/RPGRecipeDefinition.h"

const URPGRecipeDefinition* URPGRecipeFragment::GetRecipe(const URPGItemDefinition* Item)
{
	const URPGRecipeFragment* Fragment = Item ? Item->FindFragmentByClass<URPGRecipeFragment>() : nullptr;
	return Fragment ? Fragment->Recipe.Get() : nullptr;
}
