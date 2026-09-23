// RPGCoreTags.h
// Cross-system native gameplay tags. Only tags that more than one system needs live here.
// Content tags (specific items, factions, materials) belong in Config/Tags/RPGContentTags.ini.

#pragma once

#include "NativeGameplayTags.h"

namespace RPGTags
{
	// ---------- Item.Type ----------
	RPGCORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Type);
	RPGCORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Type_Weapon);
	RPGCORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Type_Armor);
	RPGCORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Type_Consumable);
	RPGCORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Type_Material);
	RPGCORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Type_Quest);

	// ---------- Item.Rarity ----------
	RPGCORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Rarity);
	RPGCORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Rarity_Common);
	RPGCORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Rarity_Uncommon);
	RPGCORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Rarity_Rare);
	RPGCORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Rarity_Epic);
	RPGCORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Rarity_Legendary);

	// ---------- Equipment.Slot ----------
	RPGCORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Equipment_Slot);
	RPGCORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Equipment_Slot_Head);
	RPGCORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Equipment_Slot_Chest);
	RPGCORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Equipment_Slot_Legs);
	RPGCORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Equipment_Slot_Feet);
	RPGCORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Equipment_Slot_MainHand);
	RPGCORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Equipment_Slot_OffHand);
	RPGCORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Equipment_Slot_Accessory);

	// ---------- Stat ----------
	RPGCORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Stat);
	RPGCORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Stat_Health);
	RPGCORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Stat_Mana);
	RPGCORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Stat_Stamina);
	RPGCORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Stat_Strength);
	RPGCORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Stat_Dexterity);
	RPGCORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Stat_Intelligence);

	// ---------- Currency ----------
	RPGCORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Currency);
	RPGCORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Currency_Gold);

	// ---------- Faction (root only; children are content tags) ----------
	RPGCORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Faction);

	// ---------- Event.Inventory ----------
	RPGCORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Inventory_ItemAdded);
	RPGCORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Inventory_ItemRemoved);

	// ---------- Event.Equipment ----------
	RPGCORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Equipment_Equipped);
	RPGCORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Equipment_Unequipped);

	// ---------- Event.Currency ----------
	RPGCORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Currency_Changed);

	// ---------- Event.Interaction ----------
	RPGCORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Interaction_Started);
	RPGCORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Interaction_Completed);

	// ---------- Event.Progression ----------
	RPGCORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Progression_LevelUp);
	RPGCORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Progression_XPGained);

	// ---------- Event.Crafting ----------
	RPGCORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Crafting_ItemCrafted);

	// ---------- Event.Economy ----------
	RPGCORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Economy_ItemBought);
	RPGCORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Economy_ItemSold);

	// ---------- Event.Faction ----------
	RPGCORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Faction_ReputationChanged);
}
