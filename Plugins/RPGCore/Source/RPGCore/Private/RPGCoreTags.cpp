// RPGCoreTags.cpp

#include "RPGCoreTags.h"

namespace RPGTags
{
	// ---------- Item.Type ----------
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Type,            "Item.Type",            "Root: item category");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Type_Weapon,     "Item.Type.Weapon",     "Equippable item used for attacking");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Type_Armor,      "Item.Type.Armor",      "Equippable item providing defense");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Type_Consumable, "Item.Type.Consumable", "Item consumed on use (potions, food)");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Type_Material,   "Item.Type.Material",   "Crafting material");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Type_Quest,      "Item.Type.Quest",      "Quest item; cannot be sold or dropped");

	// ---------- Item.Rarity ----------
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Rarity,           "Item.Rarity",           "Root: item rarity tier");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Rarity_Common,    "Item.Rarity.Common",    "Common rarity tier");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Rarity_Uncommon,  "Item.Rarity.Uncommon",  "Uncommon rarity tier");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Rarity_Rare,      "Item.Rarity.Rare",      "Rare rarity tier");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Rarity_Epic,      "Item.Rarity.Epic",      "Epic rarity tier");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Rarity_Legendary, "Item.Rarity.Legendary", "Legendary rarity tier");

	// ---------- Equipment.Slot ----------
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Equipment_Slot,           "Equipment.Slot",           "Root: equipment slot");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Equipment_Slot_Head,      "Equipment.Slot.Head",      "Head slot");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Equipment_Slot_Chest,     "Equipment.Slot.Chest",     "Chest slot");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Equipment_Slot_Legs,      "Equipment.Slot.Legs",      "Legs slot");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Equipment_Slot_Feet,      "Equipment.Slot.Feet",      "Feet slot");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Equipment_Slot_MainHand,  "Equipment.Slot.MainHand",  "Main hand slot");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Equipment_Slot_OffHand,   "Equipment.Slot.OffHand",   "Off hand slot");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Equipment_Slot_Accessory, "Equipment.Slot.Accessory", "Accessory slot (ring, amulet)");

	// ---------- Stat ----------
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Stat,              "Stat",              "Root: character stat");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Stat_Health,       "Stat.Health",       "Health");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Stat_Mana,         "Stat.Mana",         "Mana");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Stat_Stamina,      "Stat.Stamina",      "Stamina");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Stat_Strength,     "Stat.Strength",     "Strength");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Stat_Dexterity,    "Stat.Dexterity",    "Dexterity");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Stat_Intelligence, "Stat.Intelligence", "Intelligence");

	// ---------- Currency ----------
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Currency,      "Currency",      "Root: currency type");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Currency_Gold, "Currency.Gold", "Primary currency");

	// ---------- Faction ----------
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Faction, "Faction", "Root: faction. Child tags are defined as content in RPGContentTags.ini");

	// ---------- Event.Inventory ----------
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Event_Inventory_ItemAdded,   "Event.Inventory.ItemAdded",   "Broadcast when an item is added to a container");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Event_Inventory_ItemRemoved, "Event.Inventory.ItemRemoved", "Broadcast when an item is removed from a container");

	// ---------- Event.Equipment ----------
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Event_Equipment_Equipped,   "Event.Equipment.Equipped",   "Broadcast when an item is equipped");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Event_Equipment_Unequipped, "Event.Equipment.Unequipped", "Broadcast when an item is unequipped");

	// ---------- Event.Currency ----------
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Event_Currency_Changed, "Event.Currency.Changed", "Broadcast when a currency amount changes");

	// ---------- Event.Interaction ----------
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Event_Interaction_Started,   "Event.Interaction.Started",   "Broadcast when an interaction begins");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Event_Interaction_Completed, "Event.Interaction.Completed", "Broadcast when an interaction completes");

	// ---------- Event.Progression ----------
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Event_Progression_LevelUp,  "Event.Progression.LevelUp",  "Broadcast when a character levels up");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Event_Progression_XPGained, "Event.Progression.XPGained", "Broadcast when experience is gained");

	// ---------- Event.Crafting ----------
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Event_Crafting_ItemCrafted, "Event.Crafting.ItemCrafted", "Broadcast when an item is crafted");

	// ---------- Event.Economy ----------
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Event_Economy_ItemBought, "Event.Economy.ItemBought", "Broadcast when an item is bought from a vendor");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Event_Economy_ItemSold,   "Event.Economy.ItemSold",   "Broadcast when an item is sold to a vendor");

	// ---------- Event.Faction ----------
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Event_Faction_ReputationChanged, "Event.Faction.ReputationChanged", "Broadcast when reputation with a faction changes");
}
