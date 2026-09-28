// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/RPGItemFragment.h"
#include "RPGInventoryFragment.generated.h"

/**
 * 
 */
UCLASS(meta = (DisplayName = "Inventory"))
class RPGINVENTORY_API URPGInventoryFragment : public URPGItemFragment
{
	GENERATED_BODY()
public:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory",meta = (ClampMin = 0.f, Units = "kg"))
	float Weight = 0.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory")
	bool bCanBeDropped = true;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory")
	TSoftObjectPtr<UStaticMesh> Mesh;
	
	static float GetUnitWeight(const URPGItemDefinition* Item);
};
