// RPGCombatLibrary.h
// Small combat helpers for the game layer (soft targeting before a melee swing).

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "RPGCombatLibrary.generated.h"

UCLASS()
class RPGABILITIES_API URPGCombatLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	 * The nearest living pawn with an ability system within Radius and within MaxAngle degrees of the attacker's
	 * facing (2D). Dead targets (State.Dead) and the attacker itself are skipped. nullptr when none qualifies.
	 */
	UFUNCTION(BlueprintCallable, Category = "RPG|Combat", meta = (DefaultToSelf = "Attacker"))
	static AActor* FindMeleeTarget(AActor* Attacker, float Radius = 350.f, float MaxAngle = 70.f);

	/**
	 * Soft targeting: turns the attacker (yaw only) toward FindMeleeTarget, or, with no target, toward the pawn's last
	 * movement input. Returns true when a target was found.
	 */
	UFUNCTION(BlueprintCallable, Category = "RPG|Combat", meta = (DefaultToSelf = "Attacker"))
	static bool FaceMeleeTarget(AActor* Attacker, float Radius = 350.f, float MaxAngle = 70.f);
};
