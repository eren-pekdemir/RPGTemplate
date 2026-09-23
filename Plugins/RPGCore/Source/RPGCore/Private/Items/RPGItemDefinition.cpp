// RPGItemDefinition.cpp

#include "Items/RPGItemDefinition.h"
#include "Items/RPGItemFragment.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif

#include UE_INLINE_GENERATED_CPP_BY_NAME(RPGItemDefinition)

#define LOCTEXT_NAMESPACE "RPGItemDefinition"

const FPrimaryAssetType URPGItemDefinition::ItemAssetType(TEXT("Item"));

const URPGItemFragment* URPGItemDefinition::FindFragmentByClass(TSubclassOf<URPGItemFragment> FragmentClass) const
{
	if (!FragmentClass)
	{
		return nullptr;
	}

	for (const URPGItemFragment* Fragment : Fragments)
	{
		if (Fragment && Fragment->IsA(FragmentClass))
		{
			return Fragment;
		}
	}
	return nullptr;
}

FPrimaryAssetId URPGItemDefinition::GetPrimaryAssetId() const
{
	return FPrimaryAssetId(ItemAssetType, GetFName());
}

#if WITH_EDITOR
EDataValidationResult URPGItemDefinition::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = Super::IsDataValid(Context);
	bool bHasErrors = false;

	if (DisplayName.IsEmpty())
	{
		Context.AddError(LOCTEXT("EmptyName", "DisplayName is empty."));
		bHasErrors = true;
	}

	if (!ItemType.IsValid())
	{
		Context.AddError(LOCTEXT("NoType", "ItemType is not set."));
		bHasErrors = true;
	}

	if (!Rarity.IsValid())
	{
		Context.AddError(LOCTEXT("NoRarity", "Rarity is not set."));
		bHasErrors = true;
	}

	TSet<const UClass*> SeenFragmentClasses;
	for (int32 Index = 0; Index < Fragments.Num(); ++Index)
	{
		const URPGItemFragment* Fragment = Fragments[Index];
		if (!Fragment)
		{
			Context.AddError(FText::Format(LOCTEXT("NullFragment", "Fragment at index {0} is empty."), Index));
			bHasErrors = true;
			continue;
		}

		if (SeenFragmentClasses.Contains(Fragment->GetClass()))
		{
			Context.AddError(FText::Format(LOCTEXT("DuplicateFragment", "Duplicate fragment of class {0}."),
				FText::FromString(Fragment->GetClass()->GetName())));
			bHasErrors = true;
		}
		SeenFragmentClasses.Add(Fragment->GetClass());
	}

	if (bHasErrors)
	{
		return EDataValidationResult::Invalid;
	}
	return Result == EDataValidationResult::Invalid ? Result : EDataValidationResult::Valid;
}
#endif

#undef LOCTEXT_NAMESPACE
