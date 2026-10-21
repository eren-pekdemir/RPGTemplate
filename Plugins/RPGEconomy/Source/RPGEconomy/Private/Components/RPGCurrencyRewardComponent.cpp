// RPGCurrencyRewardComponent.cpp

#include "Components/RPGCurrencyRewardComponent.h"

#include "GameFramework/Actor.h"
#include "GameFramework/Pawn.h"
#include "Interfaces/RPGCurrencyHolder.h"
#include "RPGCoreStatics.h"
#include "RPGEconomy.h"
#include "RPGEconomyLibrary.h"

URPGCurrencyRewardComponent::URPGCurrencyRewardComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

int64 URPGCurrencyRewardComponent::GrantReward(AActor* Receiver)
{
	if (bRewardGranted || !Receiver)
	{
		return 0;
	}

	IRPGCurrencyHolder* Holder = URPGCoreStatics::FindCurrencyHolder(Receiver);
	if (!Holder)
	{
		Holder = URPGCoreStatics::FindCurrencyHolder(Receiver->GetInstigator());
	}

	if (!Holder)
	{
		UE_LOG(LogRPGEconomy, Verbose, TEXT("%s: %s has no wallet, reward kept."), *GetNameSafe(GetOwner()), *GetNameSafe(Receiver));
		return 0;
	}

	const int64 Amount = FMath::RandRange(MinAmount, FMath::Max(MinAmount, MaxAmount));
	if (Amount <= 0)
	{
		return 0;
	}

	// State first.
	bRewardGranted = true;
	Holder->AddCurrency(URPGEconomyLibrary::ResolveCurrency(Currency), Amount);
	return Amount;
}
