// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interfaces/RPGItemContainer.h"
#include "RPGInventoryTypes.h"
#include "RPGInventoryComponent.generated.h"

class ARPGItemPickup;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnInventoryItemAddedSignature, const FRPGItemEntry&, Entry, int32, AddedQuantity);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnInventoryItemRemovedSignature, const FRPGItemEntry&, Entry, int32, RemovedQuantity);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnInventoryWeightChangedSignature, float, NewWeight, float , MaxWeight);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryRefreshedSignature);

UCLASS(ClassGroup=(RPG), meta=(BlueprintSpawnableComponent))
class RPGINVENTORY_API URPGInventoryComponent : public UActorComponent , public IRPGItemContainer
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	URPGInventoryComponent();
	
	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnInventoryItemAddedSignature OnItemAdded;
	
	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnInventoryItemRemovedSignature OnItemRemoved;
	
	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnInventoryWeightChangedSignature OnWeightChanged;
	
	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnInventoryRefreshedSignature OnInventoryRefreshed;
	
	UPROPERTY(EditAnywhere, Category = "Inventory" , meta = (ClampMin = 1))
	int32 MaxSlots = 30;
	
	UPROPERTY(EditAnywhere, Category = "Inventory")
	bool bUseWeight = true;
	
	UPROPERTY(EditAnywhere, Category = "Inventory", meta = (EditCondition = "bUseWeight" , Units = "kg"))
	float MaxWeight = 100.0f;
	
	UPROPERTY(EditAnywhere, Category = "Inventory", meta = (EditCondition = "bUseWeight"))
	bool bAllowOverweight = false;
	
	UPROPERTY(EditAnywhere, Category = "Inventory")
	TArray<FRPGStartingItem> StartingItems;
	
	virtual int32 GetItemCount(const URPGItemDefinition* Item) const override;
	virtual bool  CanAddItem(const URPGItemDefinition* Item, int32 Quantity) const override;
	virtual int32 AddItem(const URPGItemDefinition* Item, int32 Quantity) override;
	virtual int32 RemoveItem(const URPGItemDefinition* Item, int32 Quantity) override;
	
	UFUNCTION(BlueprintPure, Category = "Inventory")
	const TArray<FRPGItemEntry>& GetEntries() const;
	
	UFUNCTION(BlueprintPure, Category = "Inventory")
	bool FindEntry(FGuid EntryId, FRPGItemEntry& OutEntry) const;
	
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	int32 RemoveEntry(FGuid EntryId, int32 Quantity);
	
	UFUNCTION(BlueprintPure, Category = "Inventory")
	int32 GetUsedSlots() const;
	
	UFUNCTION(BlueprintPure, Category = "Inventory")
	float GetCurrentWeight() const;
	
	UFUNCTION(BlueprintPure, Category = "Inventory")
	bool IsOverweight() const;
	
	UPROPERTY(EditAnywhere, Category="Inventory|Drop")
	TSubclassOf<ARPGItemPickup> PickupClass;
	
	UPROPERTY(EditAnywhere, Category="Inventory|Drop", meta = (Units = "cm", ClampMin = 0))
	float DropDistance = 100.f;
	
	UFUNCTION(BlueprintCallable, Category = "Inventory|Drop")
	int32 DropEntry(FGuid EntryId, int32 Quantity);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	
	UPROPERTY(VisibleInstanceOnly, SaveGame)
	TArray<FRPGItemEntry> Entries;
	
	float CurrentWeight = 0.f;
	

	void RecalculateWeight();
	
	int32 ComputeAddableQuantity(const URPGItemDefinition* Item, int32 Quantity) const;
	
	int32 FindEntryIndex(FGuid EntryId) const;
	
	bool bSuppressEvents = false;
};
