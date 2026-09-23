// RPGItemDefinition.h
// Static, designer-authored description of an item. Never modified at runtime.
// Behaviour is composed from fragments instead of inheritance.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "RPGItemDefinition.generated.h"

class URPGItemFragment;
class UTexture2D;

UCLASS(BlueprintType, Const)
class RPGCORE_API URPGItemDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	/** Asset Manager type name. Must match the Primary Asset Type in Project Settings. */
	static const FPrimaryAssetType ItemAssetType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Display")
	FText DisplayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Display", meta = (MultiLine = true))
	FText Description;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Display")
	TSoftObjectPtr<UTexture2D> Icon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Classification", meta = (Categories = "Item.Type"))
	FGameplayTag ItemType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Classification", meta = (Categories = "Item.Rarity"))
	FGameplayTag Rarity;

	/** Free-form tags (content tags such as Item.Material.Iron). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Classification")
	FGameplayTagContainer ItemTags;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stacking", meta = (ClampMin = 1))
	int32 MaxStackSize = 1;

	UPROPERTY(EditDefaultsOnly, Instanced, BlueprintReadOnly, Category = "Fragments")
	TArray<TObjectPtr<URPGItemFragment>> Fragments;

	UFUNCTION(BlueprintCallable, Category = "RPG|Items", meta = (DeterminesOutputType = "FragmentClass"))
	const URPGItemFragment* FindFragmentByClass(TSubclassOf<URPGItemFragment> FragmentClass) const;

	template <typename T>
	const T* FindFragmentByClass() const
	{
		return Cast<T>(FindFragmentByClass(T::StaticClass()));
	}

	virtual FPrimaryAssetId GetPrimaryAssetId() const override;

#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif
};
