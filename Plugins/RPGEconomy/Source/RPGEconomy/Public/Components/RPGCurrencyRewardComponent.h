// RPGCurrencyRewardComponent.h
// Money an enemy or chest is worth. The game layer calls GrantReward(Killer / Opener).

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "Interfaces/RPGSaveable.h"
#include "RPGCurrencyRewardComponent.generated.h"

UCLASS(ClassGroup = (RPG), meta = (BlueprintSpawnableComponent))
class RPGECONOMY_API URPGCurrencyRewardComponent : public UActorComponent, public IRPGSaveable
{
	GENERATED_BODY()

public:
	URPGCurrencyRewardComponent();

	/** Empty = default currency. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RPG|Reward", meta = (Categories = "Currency"))
	FGameplayTag Currency;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RPG|Reward", meta = (ClampMin = "0"))
	int64 MinAmount = 5;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RPG|Reward", meta = (ClampMin = "0"))
	int64 MaxAmount = 15;

	/**
	 * Gives a random amount to Receiver's currency holder (or its instigator's: projectiles, traps).
	 * At most once. Returns the amount given, 0 if nothing was given.
	 */
	UFUNCTION(BlueprintCallable, Category = "RPG|Reward")
	int64 GrantReward(AActor* Receiver);

	UFUNCTION(BlueprintPure, Category = "RPG|Reward")
	bool HasGrantedReward() const { return bRewardGranted; }

private:
	/** Saved: a looted chest stays looted (needs a RPGSaveableActorComponent on the actor). */
	UPROPERTY(SaveGame)
	bool bRewardGranted = false;
};
