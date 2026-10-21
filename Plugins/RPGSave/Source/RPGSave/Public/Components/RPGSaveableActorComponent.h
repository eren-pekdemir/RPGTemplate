// RPGSaveableActorComponent.h
// Put on an actor PLACED in the level (chest, merchant, enemy, pickup) to save it:
//  - the actor's own SaveGame variables (Blueprint variables with the "SaveGame" checkbox),
//  - every component of it that implements IRPGSaveable (Loot, Merchant, Wallet, Currency Reward...),
//  - that it was destroyed (a picked-up pickup stays gone),
//  - optionally its transform.
// Actors spawned at runtime are not saved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RPGSaveableActorComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FRPGOnActorLoadedSignature);

UCLASS(ClassGroup = (RPG), meta = (BlueprintSpawnableComponent))
class RPGSAVE_API URPGSaveableActorComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	URPGSaveableActorComponent();

	/** Remember that this actor was destroyed and destroy it again after loading. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RPG|Save")
	bool bSaveDestruction = true;

	/** Save location/rotation (physics props, moved objects). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RPG|Save")
	bool bSaveTransform = false;

	/** After this actor's data was restored. Re-apply visuals here (open chest lid, hide empty shelf). */
	UPROPERTY(BlueprintAssignable, Category = "RPG|Save")
	FRPGOnActorLoadedSignature OnLoadedFromSave;

	/** Loaded with the level (not spawned at runtime). Only these are saved. */
	UFUNCTION(BlueprintPure, Category = "RPG|Save")
	bool IsPlacedInLevel() const { return bPlacedInLevel; }

	/** Stable id: the actor's path without the PIE prefix. */
	UFUNCTION(BlueprintPure, Category = "RPG|Save")
	FString GetSaveKey() const;

	void NotifyLoaded() { OnLoadedFromSave.Broadcast(); }

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	bool bPlacedInLevel = false;
};
