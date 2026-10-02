// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "RPGEquipmentTypes.h"
#include "RPGEquipmentComponent.generated.h"

class URPGItemDefinition;


DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FRPGOnEquipmentChangedSignature,
	FGameplayTag, Slot,
	const URPGItemDefinition*, NewItem,
	const URPGItemDefinition*, OldItem);

UCLASS(ClassGroup=(RPG), meta=(BlueprintSpawnableComponent))
class RPGEQUIPMENT_API URPGEquipmentComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	URPGEquipmentComponent();
	
	UPROPERTY(BlueprintAssignable)
	FRPGOnEquipmentChangedSignature OnEquipmentChanged;
	
	UPROPERTY(EditAnywhere, meta = (Categories = "Equipment.Slot"))
	FGameplayTagContainer AvailableSlots;
	
	UPROPERTY(EditAnywhere, Category = "Equipment")
	TArray<TObjectPtr<const URPGItemDefinition>>  StartingEquipment;
	
	UFUNCTION(BlueprintPure,Category = "Equipment")
	const URPGItemDefinition* GetEquippedItem(FGameplayTag Slot) const;
	
	UFUNCTION(BlueprintPure,Category = "Equipment")
	bool IsSlotEmpty(FGameplayTag Slot)const;
	
	UFUNCTION(BlueprintPure,Category = "Equipment")
	bool HasSlot(FGameplayTag Slot)const;
	
	UFUNCTION(BlueprintPure,Category = "Equipment")
	FGameplayTag FindSlotForItem(const URPGItemDefinition* Item)const;
	
	UFUNCTION(BlueprintPure,Category = "Equipment")
	bool CanEquip(const URPGItemDefinition* Item)const;
	
	UFUNCTION(BlueprintCallable, Category = "Equipment")
	ERPGEquipResult UnequipToContainer(FGameplayTag Slot);
	
	UFUNCTION(BlueprintCallable, Category = "Equipment")
	ERPGEquipResult EquipFromContainer(const URPGItemDefinition* Item);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	void SetSlotItem(FGameplayTag Slot, const URPGItemDefinition* NewItem);
	
	UPROPERTY(VisibleInstanceOnly, SaveGame)
	TMap<FGameplayTag, TObjectPtr<const URPGItemDefinition>> EquippedItems;
};
