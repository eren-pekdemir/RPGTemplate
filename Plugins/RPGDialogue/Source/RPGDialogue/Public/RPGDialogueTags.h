// RPGDialogueTags.h

#pragma once

#include "NativeGameplayTags.h"

namespace RPGTags
{
	/** Root for events sent by "Send Event" actions. */
	RPGDIALOGUE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Dialogue_Event);

	/** Suggested event for "open the merchant's trade screen". */
	RPGDIALOGUE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Dialogue_Event_Trade);

	/** Suggested root for dialogue flags (Dialogue.Flag.MetBlacksmith). Any tag works as a flag. */
	RPGDIALOGUE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Dialogue_Flag);
}
