// RPGEconomyTypes.h
// Shared economy types (also used by RPGEconomyUI).

#pragma once

#include "CoreMinimal.h"
#include "RPGEconomyTypes.generated.h"

class URPGItemDefinition;
class UTexture2D;

UENUM(BlueprintType)
enum class ERPGTradeResult : uint8
{
	Success,
	InvalidItem,
	/** The merchant does not sell this item. */
	NotInStock,
	NotEnoughStock,
	CannotAfford,
	/** The items did not fit; nothing was bought. */
	NoRoom,
	NoContainer,
	NoWallet,
	/** The customer does not have that many to sell. */
	NotEnoughItems,
	/** Not sellable, no value, or a type this merchant does not buy. */
	ItemNotAccepted,
	MerchantCannotAfford
};

/** How a currency looks. */
USTRUCT(BlueprintType)
struct RPGECONOMY_API FRPGCurrencyInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Currency")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Currency")
	TSoftObjectPtr<UTexture2D> Icon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Currency")
	FLinearColor Color = FLinearColor(1.f, 0.82f, 0.3f, 1.f);
};

/** One line of a merchant's stock. */
USTRUCT(BlueprintType)
struct RPGECONOMY_API FRPGMerchantStockEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, SaveGame, Category = "Stock")
	TObjectPtr<const URPGItemDefinition> Item;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, SaveGame, Category = "Stock", meta = (ClampMin = "1", EditCondition = "!bInfinite"))
	int32 Quantity = 1;

	/** Never runs out (arrows, potions). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, SaveGame, Category = "Stock")
	bool bInfinite = false;
};
