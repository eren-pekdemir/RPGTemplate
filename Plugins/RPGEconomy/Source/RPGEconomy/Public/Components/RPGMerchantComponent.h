// RPGMerchantComponent.h
// Put on a merchant NPC. Sells its stock and buys items from customers, all through RPGCore interfaces.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "Interfaces/RPGSaveable.h"
#include "RPGEconomyTypes.h"
#include "RPGMerchantComponent.generated.h"

class IRPGCurrencyHolder;
class URPGItemDefinition;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRPGOnTradeSessionSignature, AActor*, Customer);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FRPGOnStockChangedSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FRPGOnItemTradedSignature,
	const URPGItemDefinition*, Item, int32, Quantity, int64, TotalPrice, bool, bCustomerBought);

/**
 * A merchant. Prices come from the items' Value fragment and the rarity multipliers.
 * Every trade is all-or-nothing: items and money move together or not at all.
 */
UCLASS(ClassGroup = (RPG), meta = (BlueprintSpawnableComponent))
class RPGECONOMY_API URPGMerchantComponent : public UActorComponent, public IRPGSaveable
{
	GENERATED_BODY()

public:
	URPGMerchantComponent();

	// ------------------------------------------------------------------ Settings

	/** Title of the trade screen ("Blacksmith"). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RPG|Merchant")
	FText DisplayName;

	/** Empty = default currency. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RPG|Merchant", meta = (Categories = "Currency"))
	FGameplayTag Currency;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RPG|Merchant", meta = (TitleProperty = "Item"))
	TArray<FRPGMerchantStockEntry> StartingStock;

	/** The customer pays item value x this. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RPG|Merchant|Prices", meta = (ClampMin = "0"))
	float BuyPriceMultiplier = 1.f;

	/** The customer receives item value x this. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RPG|Merchant|Prices", meta = (ClampMin = "0"))
	float SellPriceMultiplier = 0.4f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RPG|Merchant|Buying")
	bool bBuysItems = true;

	/** Item types this merchant buys (hierarchical, e.g. Item.Type.Weapon). Empty = everything with a value. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RPG|Merchant|Buying", meta = (Categories = "Item.Type"))
	FGameplayTagContainer AcceptedItemTypes;

	/** Items bought from customers appear in the stock (buy-back). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RPG|Merchant|Buying")
	bool bResellsBoughtItems = true;

	/** If false, the merchant pays from its own currency holder (e.g. a Wallet on the NPC) and can run out. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RPG|Merchant|Buying")
	bool bInfiniteFunds = true;

	/** The trade ends beyond this distance. 0 = never. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RPG|Merchant", meta = (ClampMin = "0", Units = "cm"))
	float MaxTradeDistance = 400.f;

	// ------------------------------------------------------------------ State

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, SaveGame, Category = "RPG|Merchant")
	TArray<FRPGMerchantStockEntry> Stock;

	// ------------------------------------------------------------------ Events

	UPROPERTY(BlueprintAssignable, Category = "RPG|Merchant")
	FRPGOnTradeSessionSignature OnTradeStarted;

	UPROPERTY(BlueprintAssignable, Category = "RPG|Merchant")
	FRPGOnTradeSessionSignature OnTradeEnded;

	UPROPERTY(BlueprintAssignable, Category = "RPG|Merchant")
	FRPGOnStockChangedSignature OnStockChanged;

	UPROPERTY(BlueprintAssignable, Category = "RPG|Merchant")
	FRPGOnItemTradedSignature OnItemTraded;

	// ------------------------------------------------------------------ Session

	/** One customer at a time; a new customer ends the previous session. */
	UFUNCTION(BlueprintCallable, Category = "RPG|Merchant")
	bool BeginTrade(AActor* Customer);

	UFUNCTION(BlueprintCallable, Category = "RPG|Merchant")
	void EndTrade();

	UFUNCTION(BlueprintPure, Category = "RPG|Merchant")
	AActor* GetCustomer() const { return CurrentCustomer.Get(); }

	UFUNCTION(BlueprintPure, Category = "RPG|Merchant")
	bool IsTrading() const { return CurrentCustomer.IsValid(); }

	// ------------------------------------------------------------------ Prices & stock

	UFUNCTION(BlueprintPure, Category = "RPG|Merchant")
	FGameplayTag GetCurrencyTag() const;

	/** Unit price the customer pays. 0 = the item has no value. */
	UFUNCTION(BlueprintPure, Category = "RPG|Merchant")
	int64 GetBuyPrice(const URPGItemDefinition* Item) const;

	/** Unit price the customer receives. 0 = the item has no value. */
	UFUNCTION(BlueprintPure, Category = "RPG|Merchant")
	int64 GetSellPrice(const URPGItemDefinition* Item) const;

	/** Would this merchant buy the item at all (type, value, sellable)? */
	UFUNCTION(BlueprintPure, Category = "RPG|Merchant")
	bool WillBuy(const URPGItemDefinition* Item) const;

	/** 0 = not in stock, MAX_int32 = infinite. */
	UFUNCTION(BlueprintPure, Category = "RPG|Merchant")
	int32 GetStockQuantity(const URPGItemDefinition* Item) const;

	// ------------------------------------------------------------------ Trading

	UFUNCTION(BlueprintPure, Category = "RPG|Merchant")
	ERPGTradeResult CanBuy(AActor* Customer, const URPGItemDefinition* Item, int32 Quantity = 1) const;

	/** The customer buys from the merchant. NoRoom is only known while buying. */
	UFUNCTION(BlueprintCallable, Category = "RPG|Merchant")
	ERPGTradeResult BuyItem(AActor* Customer, const URPGItemDefinition* Item, int32 Quantity = 1);

	UFUNCTION(BlueprintPure, Category = "RPG|Merchant")
	ERPGTradeResult CanSell(AActor* Customer, const URPGItemDefinition* Item, int32 Quantity = 1) const;

	/** The customer sells to the merchant. */
	UFUNCTION(BlueprintCallable, Category = "RPG|Merchant")
	ERPGTradeResult SellItem(AActor* Customer, const URPGItemDefinition* Item, int32 Quantity = 1);

	// IRPGSaveable
	virtual void OnLoadedFromSave() override;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	FRPGMerchantStockEntry* FindStock(const URPGItemDefinition* Item);
	const FRPGMerchantStockEntry* FindStock(const URPGItemDefinition* Item) const;

	void AddToStock(const URPGItemDefinition* Item, int32 Quantity);
	void RemoveFromStock(const URPGItemDefinition* Item, int32 Quantity);

	/** nullptr with infinite funds. */
	IRPGCurrencyHolder* GetMerchantFunds() const;

	void CheckCustomer();

	TWeakObjectPtr<AActor> CurrentCustomer;
	FTimerHandle CheckTimer;
};
