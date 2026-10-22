// RPGMerchantComponent.cpp

#include "Components/RPGMerchantComponent.h"

#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Interfaces/RPGCurrencyHolder.h"
#include "Interfaces/RPGItemContainer.h"
#include "Items/RPGItemDefinition.h"
#include "RPGCoreStatics.h"
#include "RPGEconomy.h"
#include "RPGEconomyLibrary.h"
#include "TimerManager.h"

URPGMerchantComponent::URPGMerchantComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void URPGMerchantComponent::BeginPlay()
{
	Super::BeginPlay();

	Stock = StartingStock;

	for (const FRPGMerchantStockEntry& Entry : Stock)
	{
		if (Entry.Item && URPGEconomyLibrary::GetItemValue(Entry.Item) <= 0)
		{
			UE_LOG(LogRPGEconomy, Warning, TEXT("%s: %s has no Value fragment; it will be free."),
				*GetNameSafe(GetOwner()), *GetNameSafe(Entry.Item));
		}
	}
}

void URPGMerchantComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	EndTrade();
	Super::EndPlay(EndPlayReason);
}

// ===================================================================== Session

bool URPGMerchantComponent::BeginTrade(AActor* Customer)
{
	if (!Customer)
	{
		return false;
	}

	if (IsTrading() && CurrentCustomer.Get() != Customer)
	{
		EndTrade();
	}

	CurrentCustomer = Customer;

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(CheckTimer, this, &ThisClass::CheckCustomer, 0.25f, true);
	}

	OnTradeStarted.Broadcast(Customer);
	return true;
}

void URPGMerchantComponent::EndTrade()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(CheckTimer);
	}

	AActor* OldCustomer = CurrentCustomer.Get();
	CurrentCustomer.Reset();   // state first

	if (OldCustomer)
	{
		OnTradeEnded.Broadcast(OldCustomer);
	}
}

void URPGMerchantComponent::CheckCustomer()
{
	const AActor* Customer = CurrentCustomer.Get();
	if (!Customer)
	{
		EndTrade();
		return;
	}

	if (MaxTradeDistance > 0.f &&
		FVector::DistSquared(Customer->GetActorLocation(), GetOwner()->GetActorLocation()) > FMath::Square(MaxTradeDistance))
	{
		EndTrade();
	}
}

// ===================================================================== Prices & stock

FGameplayTag URPGMerchantComponent::GetCurrencyTag() const
{
	return URPGEconomyLibrary::ResolveCurrency(Currency);
}

int64 URPGMerchantComponent::GetBuyPrice(const URPGItemDefinition* Item) const
{
	const int64 Value = URPGEconomyLibrary::GetItemValue(Item);
	if (Value <= 0)
	{
		return 0;
	}
	// Round up when the merchant sells, never below 1.
	return FMath::Max<int64>(1, static_cast<int64>(FMath::CeilToDouble(static_cast<double>(Value) * BuyPriceMultiplier)));
}

int64 URPGMerchantComponent::GetSellPrice(const URPGItemDefinition* Item) const
{
	const int64 Value = URPGEconomyLibrary::GetItemValue(Item);
	if (Value <= 0)
	{
		return 0;
	}
	// Round down when the merchant buys, never below 1.
	return FMath::Max<int64>(1, static_cast<int64>(FMath::FloorToDouble(static_cast<double>(Value) * SellPriceMultiplier)));
}

bool URPGMerchantComponent::WillBuy(const URPGItemDefinition* Item) const
{
	if (!bBuysItems || !Item || !URPGEconomyLibrary::CanItemBeSold(Item) || GetSellPrice(Item) <= 0)
	{
		return false;
	}
	return AcceptedItemTypes.IsEmpty() || Item->ItemType.MatchesAny(AcceptedItemTypes);
}

int32 URPGMerchantComponent::GetStockQuantity(const URPGItemDefinition* Item) const
{
	const FRPGMerchantStockEntry* Entry = FindStock(Item);
	if (!Entry)
	{
		return 0;
	}
	return Entry->bInfinite ? MAX_int32 : Entry->Quantity;
}

FRPGMerchantStockEntry* URPGMerchantComponent::FindStock(const URPGItemDefinition* Item)
{
	return Stock.FindByPredicate([Item](const FRPGMerchantStockEntry& Entry) { return Item && Entry.Item == Item; });
}

const FRPGMerchantStockEntry* URPGMerchantComponent::FindStock(const URPGItemDefinition* Item) const
{
	return Stock.FindByPredicate([Item](const FRPGMerchantStockEntry& Entry) { return Item && Entry.Item == Item; });
}

void URPGMerchantComponent::AddToStock(const URPGItemDefinition* Item, int32 Quantity)
{
	if (FRPGMerchantStockEntry* Entry = FindStock(Item))
	{
		if (Entry->bInfinite)
		{
			return;
		}
		Entry->Quantity += Quantity;
	}
	else
	{
		FRPGMerchantStockEntry& NewEntry = Stock.AddDefaulted_GetRef();
		NewEntry.Item = Item;
		NewEntry.Quantity = Quantity;
	}

	OnStockChanged.Broadcast();
}

void URPGMerchantComponent::RemoveFromStock(const URPGItemDefinition* Item, int32 Quantity)
{
	FRPGMerchantStockEntry* Entry = FindStock(Item);
	if (!Entry || Entry->bInfinite)
	{
		return;
	}

	Entry->Quantity -= Quantity;
	if (Entry->Quantity <= 0)
	{
		// Only this line: the same item may be listed again further down (RemoveAll would delete that stock too).
		Stock.RemoveAt(static_cast<int32>(Entry - Stock.GetData()));
	}

	OnStockChanged.Broadcast();
}

IRPGCurrencyHolder* URPGMerchantComponent::GetMerchantFunds() const
{
	return bInfiniteFunds ? nullptr : URPGCoreStatics::FindCurrencyHolder(GetOwner());
}

// ===================================================================== Buying (customer pays)

ERPGTradeResult URPGMerchantComponent::CanBuy(AActor* Customer, const URPGItemDefinition* Item, int32 Quantity) const
{
	if (!Item || Quantity < 1)
	{
		return ERPGTradeResult::InvalidItem;
	}

	const FRPGMerchantStockEntry* Entry = FindStock(Item);
	if (!Entry)
	{
		return ERPGTradeResult::NotInStock;
	}

	if (!Entry->bInfinite && Entry->Quantity < Quantity)
	{
		return ERPGTradeResult::NotEnoughStock;
	}

	if (!URPGCoreStatics::FindItemContainer(Customer))
	{
		return ERPGTradeResult::NoContainer;
	}

	const IRPGCurrencyHolder* Wallet = URPGCoreStatics::FindCurrencyHolder(Customer);
	if (!Wallet)
	{
		return ERPGTradeResult::NoWallet;
	}

	if (!Wallet->CanAfford(GetCurrencyTag(), GetBuyPrice(Item) * Quantity))
	{
		return ERPGTradeResult::CannotAfford;
	}

	return ERPGTradeResult::Success;
}

ERPGTradeResult URPGMerchantComponent::BuyItem(AActor* Customer, const URPGItemDefinition* Item, int32 Quantity)
{
	const ERPGTradeResult Check = CanBuy(Customer, Item, Quantity);
	if (Check != ERPGTradeResult::Success)
	{
		return Check;
	}

	IRPGItemContainer* Container = URPGCoreStatics::FindItemContainer(Customer);
	IRPGCurrencyHolder* Wallet = URPGCoreStatics::FindCurrencyHolder(Customer);
	const FGameplayTag CurrencyTag = GetCurrencyTag();
	const int64 TotalPrice = GetBuyPrice(Item) * Quantity;

	// 1. Items first: adding is the step that can partially fail.
	const int32 Added = Container->AddItem(Item, Quantity);
	if (Added < Quantity)
	{
		if (Added > 0)
		{
			Container->RemoveItem(Item, Added);
		}
		return ERPGTradeResult::NoRoom;
	}

	// 2. Payment: all or nothing (checked above, but never trust a check made before a change).
	if (!Wallet->SpendCurrency(CurrencyTag, TotalPrice))
	{
		Container->RemoveItem(Item, Quantity);
		return ERPGTradeResult::CannotAfford;
	}

	if (IRPGCurrencyHolder* Funds = GetMerchantFunds())
	{
		Funds->AddCurrency(CurrencyTag, TotalPrice);
	}

	RemoveFromStock(Item, Quantity);
	OnItemTraded.Broadcast(Item, Quantity, TotalPrice, true);
	return ERPGTradeResult::Success;
}

// ===================================================================== Selling (customer is paid)

ERPGTradeResult URPGMerchantComponent::CanSell(AActor* Customer, const URPGItemDefinition* Item, int32 Quantity) const
{
	if (!Item || Quantity < 1)
	{
		return ERPGTradeResult::InvalidItem;
	}

	if (!WillBuy(Item))
	{
		return ERPGTradeResult::ItemNotAccepted;
	}

	const IRPGItemContainer* Container = URPGCoreStatics::FindItemContainer(Customer);
	if (!Container)
	{
		return ERPGTradeResult::NoContainer;
	}

	if (Container->GetItemCount(Item) < Quantity)
	{
		return ERPGTradeResult::NotEnoughItems;
	}

	if (!URPGCoreStatics::FindCurrencyHolder(Customer))
	{
		return ERPGTradeResult::NoWallet;
	}

	if (!bInfiniteFunds)
	{
		const IRPGCurrencyHolder* Funds = GetMerchantFunds();
		if (!Funds || !Funds->CanAfford(GetCurrencyTag(), GetSellPrice(Item) * Quantity))
		{
			return ERPGTradeResult::MerchantCannotAfford;
		}
	}

	return ERPGTradeResult::Success;
}

ERPGTradeResult URPGMerchantComponent::SellItem(AActor* Customer, const URPGItemDefinition* Item, int32 Quantity)
{
	const ERPGTradeResult Check = CanSell(Customer, Item, Quantity);
	if (Check != ERPGTradeResult::Success)
	{
		return Check;
	}

	IRPGItemContainer* Container = URPGCoreStatics::FindItemContainer(Customer);
	IRPGCurrencyHolder* Wallet = URPGCoreStatics::FindCurrencyHolder(Customer);
	const FGameplayTag CurrencyTag = GetCurrencyTag();
	const int64 TotalPrice = GetSellPrice(Item) * Quantity;

	// 1. Take the items.
	const int32 Removed = Container->RemoveItem(Item, Quantity);
	if (Removed < Quantity)
	{
		if (Removed > 0)
		{
			Container->AddItem(Item, Removed);
		}
		return ERPGTradeResult::NotEnoughItems;
	}

	// 2. The merchant pays (if it has a purse).
	if (IRPGCurrencyHolder* Funds = GetMerchantFunds())
	{
		if (!Funds->SpendCurrency(CurrencyTag, TotalPrice))
		{
			Container->AddItem(Item, Quantity);
			return ERPGTradeResult::MerchantCannotAfford;
		}
	}

	// 3. The customer is paid.
	Wallet->AddCurrency(CurrencyTag, TotalPrice);

	if (bResellsBoughtItems)
	{
		AddToStock(Item, Quantity);
	}

	OnItemTraded.Broadcast(Item, Quantity, TotalPrice, false);
	return ERPGTradeResult::Success;
}

void URPGMerchantComponent::OnLoadedFromSave()
{
	// Stock was restored (BeginPlay had filled it from StartingStock first). A deleted item asset loads as null.
	Stock.RemoveAll([](const FRPGMerchantStockEntry& Entry) { return Entry.Item == nullptr; });
	OnStockChanged.Broadcast();
}
