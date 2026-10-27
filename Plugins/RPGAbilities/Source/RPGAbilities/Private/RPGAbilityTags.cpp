#include "RPGAbilityTags.h"

namespace RPGTags
{
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(SetByCaller_Damage, "SetByCaller.Damage",
		"Base damage of an ability, passed to GE_Damage through SetByCaller.");
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_Attack, "InputTag.Attack",
		"Attack tag.");
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_Dodge, "InputTag.Dodge",
		"Dodge tag.");
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Event_Melee_Hit, "Event.Melee.Hit",
		"Event melee tag.");
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Cooldown_Dodge, "Cooldown.Dodge",
	"Cooldown dodge tag.");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_Invulnerable, "State.Invulnerable",
		"Roll i-frames: melee hits skip the owner while present.");
}