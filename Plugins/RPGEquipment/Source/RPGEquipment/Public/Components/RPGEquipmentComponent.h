// RPGEquipmentComponent.h

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "RPGEquipmentTypes.h"
#include "Interfaces/RPGSaveable.h"
#include "RPGEquipmentComponent.generated.h"

class URPGItemDefinition;
struct FRPGStatModifier;


/** Slot changed. NewItem is null after an unequip, OldItem is null when the slot was empty. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FRPGOnEquipmentChangedSignature,
	FGameplayTag, Slot,
	const URPGItemDefinition*, NewItem,
	const URPGItemDefinition*, OldItem);

/**
 * What a character wears, one item per slot (Equipment.Slot.* tags).
 *
 * Items move between the owner's item container (any IRPGItemContainer, usually the inventory) and the slots,
 * so an equipped item is never in both places. The stat modifiers of everything equipped are pushed to the
 * owner's IRPGStatReceiver on every change; visuals are handled separately by URPGEquipmentVisualsComponent.
 * Saved with the owner (IRPGSaveable).
 */
UCLASS(ClassGroup=(RPG), meta=(BlueprintSpawnableComponent))
class RPGEQUIPMENT_API URPGEquipmentComponent : public UActorComponent, public IRPGSaveable
{
	GENERATED_BODY()

public:
	URPGEquipmentComponent();

	UPROPERTY(BlueprintAssignable, Category = "Equipment")
	FRPGOnEquipmentChangedSignature OnEquipmentChanged;

	/** Slots this character has. Humanoid defaults; remove the ones a creature lacks. */
	UPROPERTY(EditAnywhere, Category = "Equipment", meta = (Categories = "Equipment.Slot"))
	FGameplayTagContainer AvailableSlots;

	/** Equipped on BeginPlay without touching the container. Each item goes to the first slot that takes it. */
	UPROPERTY(EditAnywhere, Category = "Equipment")
	TArray<TObjectPtr<const URPGItemDefinition>>  StartingEquipment;

	/** The item in Slot, or null. */
	UFUNCTION(BlueprintPure,Category = "Equipment")
	const URPGItemDefinition* GetEquippedItem(FGameplayTag Slot) const;

	UFUNCTION(BlueprintPure,Category = "Equipment")
	bool IsSlotEmpty(FGameplayTag Slot)const;

	/** True when Slot is one of AvailableSlots (exact match). */
	UFUNCTION(BlueprintPure,Category = "Equipment")
	bool HasSlot(FGameplayTag Slot)const;

	/**
	 * The slot Item would go to: the first empty slot matching the item's EquipSlot, else the first occupied one
	 * (equipping there swaps). Invalid tag when the item is not equipment or the character has no such slot.
	 */
	UFUNCTION(BlueprintPure,Category = "Equipment")
	FGameplayTag FindSlotForItem(const URPGItemDefinition* Item)const;

	UFUNCTION(BlueprintPure,Category = "Equipment")
	bool CanEquip(const URPGItemDefinition* Item)const;

	/** Moves the item in Slot back into the owner's container. Fails without change when the container is full. */
	UFUNCTION(BlueprintCallable, Category = "Equipment")
	ERPGEquipResult UnequipToContainer(FGameplayTag Slot);

	/**
	 * Takes one Item out of the owner's container and equips it. An item already in that slot goes back to the
	 * container; if it does not fit, everything is rolled back and ContainerFull is returned.
	 */
	UFUNCTION(BlueprintCallable, Category = "Equipment")
	ERPGEquipResult EquipFromContainer(const URPGItemDefinition* Item);

	/** Sum of Stat over every equipped item's modifiers. */
	UFUNCTION(BlueprintPure, Category = "Equipment|Stats")
	float GetStatModifier(FGameplayTag Stat) const;

	/** Every stat any equipped item modifies, with the summed value. */
	UFUNCTION(BlueprintPure, Category = "Equipment|Stats")
	TMap<FGameplayTag, float> GetAllStatModifiers() const;

	/** Drops entries whose item or slot no longer exists, re-applies the stats and re-announces every slot. */
	virtual void OnLoadedFromSave() override;

protected:
	virtual void BeginPlay() override;

private:
	/** The only place that writes EquippedItems: updates stats and broadcasts OnEquipmentChanged. */
	void SetSlotItem(FGameplayTag Slot, const URPGItemDefinition* NewItem);

	UPROPERTY(VisibleInstanceOnly, SaveGame)
	TMap<FGameplayTag, TObjectPtr<const URPGItemDefinition>> EquippedItems;

	/** Sends the summed modifiers to the owner's stat receiver, replacing what this component sent before. */
	void PushStatModifiers();
};
