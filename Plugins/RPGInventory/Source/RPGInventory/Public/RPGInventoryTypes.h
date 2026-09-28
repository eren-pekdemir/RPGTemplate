// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RPGInventoryTypes.generated.h"

class URPGItemDefinition;

USTRUCT(BlueprintType)
struct RPGINVENTORY_API	 FRPGItemEntry
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadOnly, SaveGame, Category = "Inventory")
	FGuid EntryId;
	
	UPROPERTY(BlueprintReadOnly, SaveGame, Category = "Inventory")
	TObjectPtr<const URPGItemDefinition> Item = nullptr;
	
	UPROPERTY(BlueprintReadOnly, SaveGame, Category = "Inventory")
	int32 Quantity = 0;
	
	bool IsValid() const;
	
	bool CanStackWith(const URPGItemDefinition* Other) const;
};

inline bool FRPGItemEntry::IsValid() const
{
	if (Item && Quantity > 0)
		return true;
	return false;
}

inline bool FRPGItemEntry::CanStackWith(const URPGItemDefinition* Other) const
{
	if (Item == Other) return true;
	return false;
}


USTRUCT(BlueprintType)
struct  RPGINVENTORY_API FRPGStartingItem
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
	TObjectPtr<const URPGItemDefinition> Item = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory", meta = (ClampMin = 1))
	int32 Quantity = 1;
};