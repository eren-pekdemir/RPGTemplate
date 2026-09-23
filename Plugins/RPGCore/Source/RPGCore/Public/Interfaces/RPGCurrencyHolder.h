// RPGCurrencyHolder.h
// Contract for anything that holds currency. Currencies are identified by
// Gameplay Tags (Currency.Gold, ...), so new currencies need no code changes.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GameplayTagContainer.h"
#include "RPGCurrencyHolder.generated.h"

UINTERFACE(MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class URPGCurrencyHolder : public UInterface
{
	GENERATED_BODY()
};

class RPGCORE_API IRPGCurrencyHolder
{
	GENERATED_BODY()

public:
	virtual int64 GetCurrency(FGameplayTag CurrencyTag) const = 0;

	virtual void AddCurrency(FGameplayTag CurrencyTag, int64 Amount) = 0;

	/** Deducts Amount only if affordable. Returns true on success. */
	virtual bool SpendCurrency(FGameplayTag CurrencyTag, int64 Amount) = 0;

	bool CanAfford(FGameplayTag CurrencyTag, int64 Amount) const
	{
		return GetCurrency(CurrencyTag) >= Amount;
	}
};
