// RPGSaveSubsystem.h
// Saves and loads slots. In Blueprint: Get Game Instance Subsystem -> RPGSaveSubsystem -> Save To Slot / Load From Slot.
//
// What is saved (only UPROPERTY(SaveGame) properties, generic serialization):
//  - every IRPGSaveable component (and the actor's own SaveGame variables) on the local PlayerController, its pawn and PlayerState,
//  - placed level actors with a URPGSaveableActorComponent (+ which of them were destroyed),
//  - the player's transform and control rotation, the map name.
// Loading reopens the saved map, then restores everything on the next tick and calls IRPGSaveable::OnLoadedFromSave.

#pragma once

#include "CoreMinimal.h"
#include "Engine/EngineBaseTypes.h"
#include "RPGSaveGame.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "RPGSaveSubsystem.generated.h"

class AActor;
class UWorld;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FRPGOnSaveSlotSignature, const FString&, SlotName, bool, bSuccess);

UCLASS()
class RPGSAVE_API URPGSaveSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UPROPERTY(BlueprintAssignable, Category = "RPG|Save")
	FRPGOnSaveSlotSignature OnSaved;

	/** After the data was restored in the reopened map (or a failed attempt). */
	UPROPERTY(BlueprintAssignable, Category = "RPG|Save")
	FRPGOnSaveSlotSignature OnLoaded;

	UFUNCTION(BlueprintCallable, Category = "RPG|Save")
	bool SaveToSlot(const FString& SlotName = TEXT("Quick"));

	/** Starts loading: the map is reopened and the data applied a tick later. False if the slot cannot be read. */
	UFUNCTION(BlueprintCallable, Category = "RPG|Save")
	bool LoadFromSlot(const FString& SlotName = TEXT("Quick"));

	UFUNCTION(BlueprintPure, Category = "RPG|Save")
	bool DoesSaveExist(const FString& SlotName = TEXT("Quick")) const;

	UFUNCTION(BlueprintCallable, Category = "RPG|Save")
	bool DeleteSave(const FString& SlotName);

	UFUNCTION(BlueprintCallable, Category = "RPG|Save")
	bool GetSaveInfo(const FString& SlotName, FRPGSaveInfo& OutInfo) const;

	UFUNCTION(BlueprintPure, Category = "RPG|Save")
	bool IsLoading() const { return PendingLoad != nullptr; }

	/** Forget destroyed level actors (call when starting a new game). */
	UFUNCTION(BlueprintCallable, Category = "RPG|Save")
	void ResetWorldState();

	/** Called by URPGSaveableActorComponent. */
	void RegisterDestroyedActor(const UWorld* World, const FString& Key);

	static URPGSaveSubsystem* Get(const UObject* WorldContextObject);

private:
	struct FSaveTarget
	{
		FString Key;
		UObject* Object = nullptr;
	};

	UWorld* GetGameWorld() const;
	void CollectTargets(UWorld* World, TArray<FSaveTarget>& OutTargets) const;
	static void AddActorTargets(AActor* Actor, const FString& Prefix, TArray<FSaveTarget>& OutTargets);

	static TArray<uint8> SerializeObject(UObject* Object);
	static void DeserializeObject(UObject* Object, const TArray<uint8>& Data);
	static FString GetMapName(const UWorld* World);

	void SyncTrackedMap(const UWorld* World);
	void HandlePostLoadMap(UWorld* LoadedWorld);
	void HandleTravelFailure(UWorld* World, ETravelFailure::Type FailureType, const FString& ErrorString);
	void ApplyPendingLoad();

	UPROPERTY()
	TObjectPtr<URPGSaveGame> PendingLoad;

	FString PendingSlot;
	int32 ApplyAttempts = 0;

	/** Keys of destroyed placed actors in TrackedMap. */
	TSet<FString> DestroyedActors;
	FString TrackedMap;

	FDelegateHandle PostLoadMapHandle;
	FDelegateHandle TravelFailureHandle;

	static constexpr int32 UserIndex = 0;
};
