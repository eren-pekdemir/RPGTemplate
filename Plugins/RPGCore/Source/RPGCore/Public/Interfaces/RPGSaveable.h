// RPGSaveable.h
// Marks a component (or actor) whose UPROPERTY(SaveGame) properties are written to save files by RPGSave.
// C++-only interface. Both hooks are optional: inheriting the interface is enough to be saved.

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
	/** Before the SaveGame properties are written: copy runtime-only state into them if needed. */
	virtual void OnBeforeSave() {}

	/** After the SaveGame properties were restored: re-apply derived state and broadcast refresh events. */
	virtual void OnLoadedFromSave() {}
};
