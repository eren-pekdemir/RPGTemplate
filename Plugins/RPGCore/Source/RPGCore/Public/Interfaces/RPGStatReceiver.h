// RPGStatReceiver.h

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Stats/RPGStatModifier.h"
#include "RPGStatReceiver.generated.h"

UINTERFACE(MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class URPGStatReceiver : public UInterface { GENERATED_BODY() };

class RPGCORE_API IRPGStatReceiver
{
	GENERATED_BODY()
public:
	virtual void SetStatModifiers(const UObject* Source, const TArray<FRPGStatModifier>& Modifiers) = 0;
};
