// RPGInventoryComponent.h

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interfaces/RPGItemContainer.h"
#include "RPGInventoryTypes.h"
#include "Interfaces/RPGSaveable.h"
#include "RPGInventoryComponent.generated.h"

class ARPGItemPickup;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnInventoryItemAddedSignature, const FRPGItemEntry&, Entry, int32, AddedQuantity);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnInventoryItemRemovedSignature, const FRPGItemEntry&, Entry, int32, RemovedQuantity);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnInventoryWeightChangedSignature, float, NewWeight, float , MaxWeight);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryRefreshedSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FRPGOnItemsReceivedSignature,
	const URPGItemDefinition*, Item, int32, Quantity);

/**
 * Slot-and-weight inventory: a list of stacks (entries), each one item type up to its MaxStackSize.
 *
 * Other systems (crafting, trade, loot, equipment) reach it only through IRPGItemContainer, so any other
 * container can stand in for it. The UI addresses single stacks by EntryId. Saved with its owner (IRPGSaveable).
 */
UCLASS(ClassGroup=(RPG), meta=(BlueprintSpawnableComponent))
class RPGINVENTORY_API URPGInventoryComponent : public UActorComponent , public IRPGItemContainer, public IRPGSaveable
{
	GENERATED_BODY()

public:
	URPGInventoryComponent();

	/** Once per stack that grew or was created. Not fired for StartingItems or while loading a save. */
	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnInventoryItemAddedSignature OnItemAdded;

	/** Once per stack that shrank or emptied; Entry is the stack as it was after the removal. */
	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnInventoryItemRemovedSignature OnItemRemoved;

	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnInventoryWeightChangedSignature OnWeightChanged;

	/** The whole content was replaced (starting items, a loaded save): redraw everything. */
	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnInventoryRefreshedSignature OnInventoryRefreshed;

	/** Once per AddItem call, with the total added: "+3 Iron Ore" notifications. */
	UPROPERTY(BlueprintAssignable, Category = "RPG|Inventory")
	FRPGOnItemsReceivedSignature OnItemsReceived;

	/** Number of stacks, not items. */
	UPROPERTY(EditAnywhere, Category = "Inventory" , meta = (ClampMin = 1))
	int32 MaxSlots = 30;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	bool bUseWeight = true;

	UPROPERTY(EditAnywhere, Category = "Inventory", meta = (EditCondition = "bUseWeight" , Units = "kg"))
	float MaxWeight = 100.0f;

	/** Accept items past MaxWeight (IsOverweight then reports it, e.g. to slow the character down). */
	UPROPERTY(EditAnywhere, Category = "Inventory", meta = (EditCondition = "bUseWeight"))
	bool bAllowOverweight = false;

	/** Added on BeginPlay without item events. A save, when loaded, replaces them. */
	UPROPERTY(EditAnywhere, Category = "Inventory")
	TArray<FRPGStartingItem> StartingItems;

	// IRPGItemContainer: adding tops up existing stacks first, then opens new ones; it adds as many as fit
	// (slots, stack size, weight) and returns that number. Removing takes from the newest stacks first.
	virtual int32 GetItemCount(const URPGItemDefinition* Item) const override;
	virtual bool  CanAddItem(const URPGItemDefinition* Item, int32 Quantity) const override;
	virtual int32 AddItem(const URPGItemDefinition* Item, int32 Quantity) override;
	virtual int32 RemoveItem(const URPGItemDefinition* Item, int32 Quantity) override;
	virtual void  GetItems(TArray<FRPGItemQuantity>& OutItems) const override;

	/** Every stack in display order. */
	UFUNCTION(BlueprintPure, Category = "Inventory")
	const TArray<FRPGItemEntry>& GetEntries() const;

	UFUNCTION(BlueprintPure, Category = "Inventory")
	bool FindEntry(FGuid EntryId, FRPGItemEntry& OutEntry) const;

	/** Removes up to Quantity from one stack (a slot the player picked). Returns the amount removed. */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	int32 RemoveEntry(FGuid EntryId, int32 Quantity);

	UFUNCTION(BlueprintPure, Category = "Inventory")
	int32 GetUsedSlots() const;

	UFUNCTION(BlueprintPure, Category = "Inventory")
	float GetCurrentWeight() const;

	UFUNCTION(BlueprintPure, Category = "Inventory")
	bool IsOverweight() const;

	/** Spawned by DropEntry to carry the dropped items. */
	UPROPERTY(EditAnywhere, Category="Inventory|Drop")
	TSubclassOf<ARPGItemPickup> PickupClass;

	/** How far in front of the owner dropped items appear. */
	UPROPERTY(EditAnywhere, Category="Inventory|Drop", meta = (Units = "cm", ClampMin = 0))
	float DropDistance = 100.f;

	/**
	 * Drops up to Quantity of one stack as a pickup in front of the owner. Items whose inventory fragment
	 * forbids dropping (quest items) stay. Returns the amount dropped.
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory|Drop")
	int32 DropEntry(FGuid EntryId, int32 Quantity);

	/** Drops entries whose item asset no longer exists, recomputes the weight and broadcasts a refresh. */
	virtual void OnLoadedFromSave() override;

protected:
	virtual void BeginPlay() override;

private:

	UPROPERTY(VisibleInstanceOnly, SaveGame, Category = "RPG|Inventory")
	TArray<FRPGItemEntry> Entries;

	/** Cached sum of item weights; RecalculateWeight keeps it current and fires OnWeightChanged. */
	float CurrentWeight = 0.f;

	void RecalculateWeight();

	/** How many of Quantity would fit right now (free stack space, free slots, remaining weight). */
	int32 ComputeAddableQuantity(const URPGItemDefinition* Item, int32 Quantity) const;

	int32 FindEntryIndex(FGuid EntryId) const;

	/** Set while the content is rebuilt in bulk; one OnInventoryRefreshed replaces the per-stack events. */
	bool bSuppressEvents = false;

	/** Starting items are not "received": no OnItemsReceived notification for them. */
	bool bAddingStartingItems = false;
};
