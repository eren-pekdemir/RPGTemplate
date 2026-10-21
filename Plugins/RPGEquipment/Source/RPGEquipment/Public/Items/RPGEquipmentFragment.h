// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Items/RPGItemFragment.h"
#include "Stats/RPGStatModifier.h"
#include "RPGEquipmentFragment.generated.h"

class URPGItemDefinition;

UCLASS(meta = (DisplayName = "Equipment"))
class RPGEQUIPMENT_API URPGEquipmentFragment : public URPGItemFragment
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipment", meta = (Categories = "Equipment.Slot"))
	FGameplayTag EquipSlot;
	
	UPROPERTY(EditAnywhere,Category = "Equipment")
	TSoftObjectPtr<UStaticMesh> AttachedStaticMesh;
	
	UPROPERTY(EditAnywhere,Category = "Equipment")
	TSoftObjectPtr<USkeletalMesh> AttachedSkeletalMesh;
	
	UPROPERTY(EditAnywhere,Category = "Equipment")
	FName AttachSocket;

	/** Stat bonuses while equipped (moved here from URPGItemFragment; same name, so saved assets keep their values). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipment|Stats", meta = (TitleProperty = "Stat"))
	TArray<FRPGStatModifier> Modifiers;

	UFUNCTION(BlueprintPure, Category = "Equipment")
	static FGameplayTag GetEquipSlot(const URPGItemDefinition* Item);
	
};
