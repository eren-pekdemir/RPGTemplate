// RPGSaveable.h
// Contract for anything that persists between sessions.
// The save system serializes every UPROPERTY marked with the SaveGame specifier,
// so implementers usually only need GetSaveId() plus optional hooks.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "RPGSaveable.generated.h"

UINTERFACE(MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class URPGSaveable : public UInterface
{
	GENERATED_BODY()
};

class RPGCORE_API IRPGSaveable
{
	GENERATED_BODY()

public:
	/**
	 * Stable identifier across sessions. Level-placed actors can use their name;
	 * runtime-spawned objects must store a persistent FGuid.
	 */
	virtual FName GetSaveId() const = 0;

	/** Called right before SaveGame properties are serialized. */
	virtual void OnPreSave() {}

	/** Called after SaveGame properties are restored. Rebuild derived/runtime state here. */
	virtual void OnPostLoad() {}
};
