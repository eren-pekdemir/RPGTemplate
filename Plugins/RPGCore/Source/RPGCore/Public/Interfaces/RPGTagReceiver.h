#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Interface.h"
#include "RPGTagReceiver.generated.h"

UINTERFACE(MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class URPGTagReceiver : public UInterface
{
	GENERATED_BODY()
};

/**
 * Something that can hold gameplay tags granted by other systems (skills, buffs, equipment).
 * Each source owns a snapshot; sending a new snapshot replaces the old one.
 */
class RPGCORE_API IRPGTagReceiver
{
	GENERATED_BODY()

public:
	/** Replaces ALL tags previously granted by Source with Tags. An empty container removes them. */
	virtual void SetGrantedTags(const UObject* Source, const FGameplayTagContainer& Tags) = 0;
};