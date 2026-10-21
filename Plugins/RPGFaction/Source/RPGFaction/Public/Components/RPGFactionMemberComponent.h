// RPGFactionMemberComponent.h
// Put on NPCs. Says which faction they belong to, whether they are hostile to someone, and costs reputation when killed.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "Interfaces/RPGSaveable.h"
#include "RPGFactionMemberComponent.generated.h"

UCLASS(ClassGroup = (RPG), meta = (BlueprintSpawnableComponent))
class RPGFACTION_API URPGFactionMemberComponent : public UActorComponent, public IRPGSaveable
{
	GENERATED_BODY()

public:
	URPGFactionMemberComponent();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RPG|Faction", meta = (Categories = "Faction"))
	FGameplayTag Faction;

	/**
	 * Reputation the killer gains (positive) or loses (negative) with this faction.
	 * Relations apply: killing a bandit (-25 Bandits) can raise Village reputation.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RPG|Faction")
	int32 KillReputationChange = -25;

	/** Call from On Death with the killer. At most once. */
	UFUNCTION(BlueprintCallable, Category = "RPG|Faction")
	void ReportKilledBy(AActor* Killer);

	/** True if Other's rank with this faction is hostile. NPCs without reputation are never hostile. */
	UFUNCTION(BlueprintPure, Category = "RPG|Faction")
	bool IsHostileTo(AActor* Other) const;

	UFUNCTION(BlueprintPure, Category = "RPG|Faction")
	int32 GetReputationOf(AActor* Other) const;

private:
	/** Saved like ExperienceReward's flag: a placed NPC that was not destroyed cannot be "killed" for reputation twice. */
	UPROPERTY(SaveGame)
	bool bKillReported = false;
};
