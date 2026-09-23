# RPG Template (UE5)

Modular, data-driven RPG framework for Unreal Engine 5.
Each gameplay system lives in its own plugin and communicates through a shared core
(Gameplay Tags, interfaces, event bus), so systems can be added or removed independently.
Designers create content (items, recipes, shops, dialogues) through Data Assets and
custom editor tools, without writing code.

## Systems
| System | Plugin | Status |
|---|---|---|
| Core (tags, interfaces, event bus) | RPGCore | 🚧 |
| Interaction | RPGInteraction | ⏳ |
| Inventory | RPGInventory | ⏳ |
| Equipment | RPGEquipment | ⏳ |
| Abilities & Stats (GAS) | RPGAbilities | ⏳ |
| Progression (XP, level, skill tree) | RPGProgression | ⏳ |
| Loot & Rarity | RPGLoot | ⏳ |
| Crafting | RPGCrafting | ⏳ |
| Market / Economy | RPGEconomy | ⏳ |
| Faction & Reputation | RPGFaction | ⏳ |
| Dialogue | RPGDialogue | ⏳ |
| Save / Load | RPGSave | ⏳ |
| Editor tools | RPGEditor | ⏳ |

## Architecture
- Plugins depend only on `RPGCore` (and engine modules) — never on each other.
- Cross-system communication: interfaces + Gameplay Tags + event bus in `RPGCore`.
- The sample game module wires systems together.

## Engine
Unreal Engine 5.x · C++ · Rider
