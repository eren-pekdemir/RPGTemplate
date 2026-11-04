// RPGEquipmentFragment.h

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Items/RPGItemFragment.h"
#include "Stats/RPGStatModifier.h"
#include "RPGEquipmentFragment.generated.h"

class URPGItemDefinition;

/** Makes an item equippable: the slot it goes to, how it looks on the character and the stats it gives. */
UCLASS(meta = (DisplayName = "Equipment"))
class RPGEQUIPMENT_API URPGEquipmentFragment : public URPGItemFragment
{
	GENERATED_BODY()

public:

	/** Slot the item fits. Slots are matched with MatchesTag, so a parent tag fits every slot under it. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipment", meta = (Categories = "Equipment.Slot"))
	FGameplayTag EquipSlot;

	/** Rigid look (sword, shield, helmet), attached to AttachSocket. Wins when both meshes are set. */
	UPROPERTY(EditAnywhere,Category = "Equipment")
	TSoftObjectPtr<UStaticMesh> AttachedStaticMesh;

	/** Skinned look (armour) that follows the character's skeleton. Used when no static mesh is set. */
	UPROPERTY(EditAnywhere,Category = "Equipment")
	TSoftObjectPtr<USkeletalMesh> AttachedSkeletalMesh;

	/** Socket on the character mesh the static mesh attaches to (e.g. weapon_r). */
	UPROPERTY(EditAnywhere,Category = "Equipment")
	FName AttachSocket;

	/** Stat bonuses while equipped (moved here from URPGItemFragment; same name, so saved assets keep their values). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipment|Stats", meta = (TitleProperty = "Stat"))
	TArray<FRPGStatModifier> Modifiers;

	UFUNCTION(BlueprintPure, Category = "Equipment")
	static FGameplayTag GetEquipSlot(const URPGItemDefinition* Item);
	
};
