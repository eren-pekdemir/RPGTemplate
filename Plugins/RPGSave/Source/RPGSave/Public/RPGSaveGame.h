// RPGSaveGame.h
// The file on disk: raw SaveGame-property bytes per object, keyed by where the object lives.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "RPGSaveGame.generated.h"

/** One object's UPROPERTY(SaveGame) properties. */
USTRUCT()
struct RPGSAVE_API FRPGSavedObject
{
	GENERATED_BODY()

	/** "Controller/<Component>", "Pawn/<Component>", "PlayerState/<Component>", "World/<ActorPath>/<Component>". */
	UPROPERTY(SaveGame)
	FString Key;

	UPROPERTY(SaveGame)
	TArray<uint8> Data;
};

/** Shown in a load menu. */
USTRUCT(BlueprintType)
struct RPGSAVE_API FRPGSaveInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "RPG|Save")
	FString SlotName;

	UPROPERTY(BlueprintReadOnly, Category = "RPG|Save")
	FString MapName;

	UPROPERTY(BlueprintReadOnly, Category = "RPG|Save")
	FDateTime Timestamp;

	UPROPERTY(BlueprintReadOnly, Category = "RPG|Save")
	int32 SaveVersion = 0;
};

UCLASS()
class RPGSAVE_API URPGSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	/** Bump when the format changes; LoadFromSlot refuses files newer than this. Older files can be migrated. */
	static constexpr int32 CurrentVersion = 1;

	UPROPERTY(SaveGame)
	int32 SaveVersion = 0;

	UPROPERTY(SaveGame)
	FDateTime Timestamp;

	/** Short map name (no PIE prefix), opened by LoadFromSlot. */
	UPROPERTY(SaveGame)
	FString MapName;

	UPROPERTY(SaveGame)
	bool bHasPlayerTransform = false;

	UPROPERTY(SaveGame)
	FTransform PlayerTransform;

	UPROPERTY(SaveGame)
	FRotator ControlRotation = FRotator::ZeroRotator;

	UPROPERTY(SaveGame)
	TArray<FRPGSavedObject> Objects;

	/** Placed actors that were destroyed (picked-up pickups, killed enemies). Destroyed again after loading. */
	UPROPERTY(SaveGame)
	TArray<FString> DestroyedActors;

	/** Placed actors with bSaveTransform (pushed crates). */
	UPROPERTY(SaveGame)
	TMap<FString, FTransform> ActorTransforms;

	const FRPGSavedObject* FindObject(const FString& Key) const;
};
