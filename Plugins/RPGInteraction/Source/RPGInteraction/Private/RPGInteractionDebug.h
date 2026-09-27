// RPGInteractionDebug.h
// Module-private debug switches for the RPGInteraction plugin.
// This file answers only ONE question: "should debug info be drawn?"
// WHAT to draw is decided by each class itself, because each class owns its own data.

#pragma once

#include "CoreMinimal.h"
#include "EngineDefines.h" // defines ENABLE_DRAW_DEBUG. Without it, "#if ENABLE_DRAW_DEBUG" is silently false!

namespace RPGInteractionDebug
{
#if ENABLE_DRAW_DEBUG
	/** Backing value of the console variable rpg.Interaction.Debug (defined in RPGInteractionDebug.cpp). */
	extern int32 GDebugLevel;
#endif

	/** True when interaction debug drawing is enabled. Always false in builds without debug drawing (Shipping). */
	inline bool IsEnabled()
	{
#if ENABLE_DRAW_DEBUG
		return GDebugLevel > 0;
#else
		return false;
#endif
	}

	/**
	 * Lifetime for debug shapes drawn by code that repeats every Interval seconds.
	 * Shapes stay on screen until the next repetition, so they don't flicker.
	 * Interval <= 0 means "one-off" -> draw for a single frame.
	 */
	inline float GetDrawLifetime(float Interval)
	{
		constexpr float Padding = 0.02f; // small overlap so there is no empty frame between two scans
		return Interval > 0.f ? Interval + Padding : 0.f;
	}
}
