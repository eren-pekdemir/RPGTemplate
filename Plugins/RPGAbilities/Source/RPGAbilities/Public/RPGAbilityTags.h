#pragma once

#include "CoreMinimal.h"
#include "NativeGameplayTags.h"

/**
 * Gameplay tags owned by the Abilities system.
 * Shared, cross-system tags (State.Dead, Stat.*, Equipment.Slot.*) live in RPGCoreTags instead.
 */
namespace RPGTags
{
	/** Base damage of an ability, passed to GE_Damage through SetByCaller. */
	RPGABILITIES_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Damage);
	
	RPGABILITIES_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Attack);
	
	RPGABILITIES_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Dodge);
	
	RPGABILITIES_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Melee_Hit);
	
	RPGABILITIES_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Cooldown_Dodge);
}