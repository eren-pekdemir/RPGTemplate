// RPGReputationComponent.h
// The player's standing with every faction. Put it on the PlayerState (survives respawn, like progression).

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "Interfaces/RPGReputationHolder.h"
#include "Interfaces/RPGSaveable.h"
#include "RPGFactionTypes.h"
#include "RPGReputationComponent.generated.h"

class APawn;
class APlayerState;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FRPGOnReputationChangedSignature,
	FGameplayTag, Faction, int32, NewReputation, int32, Delta);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FRPGOnReputationRankChangedSignature,
	FGameplayTag, Faction, int32, NewRankIndex, int32, OldRankIndex);

/**
 * Reputation per faction tag. Rank tags (FRPGReputationRank::GrantedTag) are pushed to the pawn through
 * IRPGTagReceiver, so any system can gate on "Friendly with the Village" without knowing this plugin.
 */
UCLASS(ClassGroup = (RPG), meta = (BlueprintSpawnableComponent))
class RPGFACTION_API URPGReputationComponent : public UActorComponent, public IRPGReputationHolder, public IRPGSaveable
{
	GENERATED_BODY()

public:
	URPGReputationComponent();

	UPROPERTY(BlueprintAssignable, Category = "RPG|Reputation")
	FRPGOnReputationChangedSignature OnReputationChanged;

	UPROPERTY(BlueprintAssignable, Category = "RPG|Reputation")
	FRPGOnReputationRankChangedSignature OnRankChanged;

	UFUNCTION(BlueprintPure, Category = "RPG|Reputation")
	int32 GetFactionReputation(FGameplayTag Faction) const { return GetReputation(Faction); }

	/** Adds Amount; with bApplyRelations, related factions change by Amount x their multiplier. */
	UFUNCTION(BlueprintCallable, Category = "RPG|Reputation")
	void ChangeReputation(FGameplayTag Faction, int32 Amount, bool bApplyRelations = true);

	UFUNCTION(BlueprintCallable, Category = "RPG|Reputation")
	void SetReputation(FGameplayTag Faction, int32 NewReputation);

	/** INDEX_NONE if the faction is unknown or has no ranks. */
	UFUNCTION(BlueprintPure, Category = "RPG|Reputation")
	int32 GetRankIndex(FGameplayTag Faction) const;

	UFUNCTION(BlueprintPure, Category = "RPG|Reputation")
	bool GetRank(FGameplayTag Faction, FRPGReputationRank& OutRank) const;

	UFUNCTION(BlueprintPure, Category = "RPG|Reputation")
	bool IsHostileWith(FGameplayTag Faction) const;

	/** On the actor, or on the PlayerState of a pawn / controller. */
	UFUNCTION(BlueprintPure, Category = "RPG|Reputation")
	static URPGReputationComponent* FindReputationComponent(AActor* Actor);

	// IRPGReputationHolder
	virtual int32 GetReputation(const FGameplayTag& Faction) const override;
	virtual void AddReputation(const FGameplayTag& Faction, int32 Amount) override;

	// IRPGSaveable
	virtual void OnLoadedFromSave() override;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	/** The single gate: clamps, stores, broadcasts, detects rank changes. */
	void SetReputationInternal(const FGameplayTag& Faction, int32 NewReputation);

	/** Sends the current rank tags of every faction to the pawn. */
	void PushRankTags();

	/** Adds the starting reputation of every faction that is not in the map yet. */
	void AddMissingFactions();

	AActor* GetTargetActor() const;

	UFUNCTION()
	void HandlePawnSet(APlayerState* Player, APawn* NewPawn, APawn* OldPawn);

	UPROPERTY(VisibleInstanceOnly, SaveGame, Category = "RPG|Reputation")
	TMap<FGameplayTag, int32> Reputation;
};
