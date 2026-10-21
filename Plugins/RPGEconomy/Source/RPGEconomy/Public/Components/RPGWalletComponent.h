// RPGWalletComponent.h
// Money for its owner (player or merchant). Implements IRPGCurrencyHolder.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "Interfaces/RPGCurrencyHolder.h"
#include "Interfaces/RPGSaveable.h"
#include "RPGWalletComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FRPGOnCurrencyChangedSignature,
	FGameplayTag, Currency, int64, NewAmount, int64, Delta);

/**
 * Balances per currency tag. An empty tag always means the default currency (Project Settings -> RPG Economy).
 * Every change goes through one function and is broadcast once.
 */
UCLASS(ClassGroup = (RPG), meta = (BlueprintSpawnableComponent))
class RPGECONOMY_API URPGWalletComponent : public UActorComponent, public IRPGCurrencyHolder, public IRPGSaveable
{
	GENERATED_BODY()

public:
	URPGWalletComponent();

	UPROPERTY(EditAnywhere, Category = "RPG|Economy", meta = (Categories = "Currency", ForceInlineRow))
	TMap<FGameplayTag, int64> StartingCurrency;

	UPROPERTY(BlueprintAssignable, Category = "RPG|Economy")
	FRPGOnCurrencyChangedSignature OnCurrencyChanged;

	/** Blueprint access (empty tag = default currency). */
	UFUNCTION(BlueprintPure, Category = "RPG|Economy")
	int64 GetAmount(FGameplayTag Currency) const { return GetCurrency(Currency); }

	UFUNCTION(BlueprintCallable, Category = "RPG|Economy")
	void Add(FGameplayTag Currency, int64 Amount) { AddCurrency(Currency, Amount); }

	UFUNCTION(BlueprintCallable, Category = "RPG|Economy")
	bool Spend(FGameplayTag Currency, int64 Amount) { return SpendCurrency(Currency, Amount); }

	// IRPGCurrencyHolder
	virtual int64 GetCurrency(const FGameplayTag& Currency) const override;
	virtual void AddCurrency(const FGameplayTag& Currency, int64 Amount) override;
	virtual bool SpendCurrency(const FGameplayTag& Currency, int64 Amount) override;

	// IRPGSaveable
	virtual void OnLoadedFromSave() override;

protected:
	virtual void BeginPlay() override;

private:
	/** The single gate: every change goes through here. */
	void SetBalance(const FGameplayTag& Currency, int64 NewAmount);

	UPROPERTY(VisibleInstanceOnly, SaveGame, Category = "RPG|Economy")
	TMap<FGameplayTag, int64> Balances;
};
