// RPGCoreStatics.cpp

#include "RPGCoreStatics.h"
#include "Interfaces/RPGItemContainer.h"
#include "Interfaces/RPGCurrencyHolder.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RPGCoreStatics)

IRPGItemContainer* URPGCoreStatics::FindItemContainer(AActor* Actor)
{
	return FindInterface<IRPGItemContainer, URPGItemContainer>(Actor);
}

IRPGCurrencyHolder* URPGCoreStatics::FindCurrencyHolder(AActor* Actor)
{
	return FindInterface<IRPGCurrencyHolder, URPGCurrencyHolder>(Actor);
}

int32 URPGCoreStatics::GetItemCount(AActor* Target, const URPGItemDefinition* Item)
{
	const IRPGItemContainer* Container = FindItemContainer(Target);
	return Container ? Container->GetItemCount(Item) : 0;
}

int32 URPGCoreStatics::AddItem(AActor* Target, const URPGItemDefinition* Item, int32 Quantity)
{
	IRPGItemContainer* Container = FindItemContainer(Target);
	return (Container && Item && Quantity > 0) ? Container->AddItem(Item, Quantity) : 0;
}

int32 URPGCoreStatics::RemoveItem(AActor* Target, const URPGItemDefinition* Item, int32 Quantity)
{
	IRPGItemContainer* Container = FindItemContainer(Target);
	return (Container && Item && Quantity > 0) ? Container->RemoveItem(Item, Quantity) : 0;
}

int64 URPGCoreStatics::GetCurrency(AActor* Target, FGameplayTag CurrencyTag)
{
	const IRPGCurrencyHolder* Holder = FindCurrencyHolder(Target);
	return Holder ? Holder->GetCurrency(CurrencyTag) : 0;
}

bool URPGCoreStatics::SpendCurrency(AActor* Target, FGameplayTag CurrencyTag, int64 Amount)
{
	IRPGCurrencyHolder* Holder = FindCurrencyHolder(Target);
	return (Holder && Amount >= 0) ? Holder->SpendCurrency(CurrencyTag, Amount) : false;
}
