// RPGWalletComponent.cpp

#include "Components/RPGWalletComponent.h"

#include "RPGEconomyLibrary.h"

URPGWalletComponent::URPGWalletComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void URPGWalletComponent::BeginPlay()
{
	Super::BeginPlay();

	for (const TPair<FGameplayTag, int64>& Pair : StartingCurrency)
	{
		AddCurrency(Pair.Key, Pair.Value);
	}
}

int64 URPGWalletComponent::GetCurrency(const FGameplayTag& Currency) const
{
	return Balances.FindRef(URPGEconomyLibrary::ResolveCurrency(Currency));
}

void URPGWalletComponent::AddCurrency(const FGameplayTag& Currency, int64 Amount)
{
	if (Amount <= 0)
	{
		return;
	}

	const FGameplayTag Resolved = URPGEconomyLibrary::ResolveCurrency(Currency);
	const int64 Current = Balances.FindRef(Resolved);

	// Overflow guard: clamp instead of wrapping to a negative balance.
	const int64 NewAmount = (Current > MAX_int64 - Amount) ? MAX_int64 : Current + Amount;
	SetBalance(Resolved, NewAmount);
}

bool URPGWalletComponent::SpendCurrency(const FGameplayTag& Currency, int64 Amount)
{
	if (Amount < 0)
	{
		return false;
	}
	if (Amount == 0)
	{
		return true;
	}

	const FGameplayTag Resolved = URPGEconomyLibrary::ResolveCurrency(Currency);
	const int64 Current = Balances.FindRef(Resolved);
	if (Current < Amount)
	{
		return false;   // all or nothing
	}

	SetBalance(Resolved, Current - Amount);
	return true;
}

void URPGWalletComponent::SetBalance(const FGameplayTag& Currency, int64 NewAmount)
{
	const int64 OldAmount = Balances.FindRef(Currency);
	if (OldAmount == NewAmount)
	{
		return;
	}

	// State first, then the event.
	Balances.Add(Currency, NewAmount);
	OnCurrencyChanged.Broadcast(Currency, NewAmount, NewAmount - OldAmount);
}

void URPGWalletComponent::OnLoadedFromSave()
{
	// Balances were replaced wholesale: let every listener (currency widget, trade screen) refresh.
	for (const TPair<FGameplayTag, int64>& Pair : Balances)
	{
		OnCurrencyChanged.Broadcast(Pair.Key, Pair.Value, 0);
	}
}
