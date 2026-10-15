// RPGItemQuantity.h
// An item and an amount: crafting ingredients and outputs, prices, rewards.

#pragma once

#include "CoreMinimal.h"
#include "RPGItemQuantity.generated.h"

class URPGItemDefinition;

USTRUCT(BlueprintType)
struct RPGCORE_API FRPGItemQuantity
{
	GENERATED_BODY()

	FRPGItemQuantity() = default;

	FRPGItemQuantity(const URPGItemDefinition* InItem, int32 InQuantity)
		: Item(InItem), Quantity(InQuantity)
	{
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	TObjectPtr<const URPGItemDefinition> Item;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item", meta = (ClampMin = "1"))
	int32 Quantity = 1;

	bool IsValid() const { return Item != nullptr && Quantity > 0; }
};
