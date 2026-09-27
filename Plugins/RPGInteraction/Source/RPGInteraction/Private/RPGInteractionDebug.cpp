// RPGInteractionDebug.cpp
// The ONLY place where the console variable is defined.

#include "RPGInteractionDebug.h"
#include "HAL/IConsoleManager.h"

#if ENABLE_DRAW_DEBUG
namespace RPGInteractionDebug
{
	int32 GDebugLevel = 0;

	static FAutoConsoleVariableRef CVarInteractionDebug(
		TEXT("rpg.Interaction.Debug"),
		GDebugLevel,
		TEXT("Draws interaction debug info.\n")
		TEXT("0 = off\n")
		TEXT("1 = trace, hit point, reach radius, focus and hold progress"),
		ECVF_Cheat);
}
#endif
