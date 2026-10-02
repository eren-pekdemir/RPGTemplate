// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Items/RPGItemFragment.h"
#include "RPGEquipmentFragment.generated.h"



UCLASS(meta = (DisplayName = "Equipment"))
class RPGEQUIPMENT_API URPGEquipmentFragment : public URPGItemFragment
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (Categories = "Equipment.Slot"))
	FGameplayTag EquipSlot;
	
	UPROPERTY(EditAnywhere,Category = "Equipment")
	TSoftObjectPtr<UStaticMesh> AttachedStaticMesh;
	
	UPROPERTY(EditAnywhere,Category = "Equipment")
	TSoftObjectPtr<USkeletalMesh> AttachedSkeletalMesh;
	
	UPROPERTY(EditAnywhere,Category = "Equipment")
	FName AttachSocket;
	
	UFUNCTION(BlueprintPure, Category = "Equipment")
	static FGameplayTag GetEquipSlot(const URPGItemDefinition* Item);
	
};
