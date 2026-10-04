#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "RPGItemTooltip.generated.h"

class URPGItemDefinition;

UINTERFACE(MinimalAPI, Blueprintable)
class URPGItemTooltip : public UInterface
{
	GENERATED_BODY()
};

class RPGCORE_API IRPGItemTooltip
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Item|Tooltip")
	void SetTooltipItem(const URPGItemDefinition* Item, int32 Quantity);
};
