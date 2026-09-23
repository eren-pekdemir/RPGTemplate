// RPGCoreStatics.h
// Lookup helpers + Blueprint access for the C++-only core interfaces.
// Interfaces may be implemented by the actor itself or by one of its components.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"
#include "Components/ActorComponent.h"
#include "RPGCoreStatics.generated.h"

class IRPGItemContainer;
class IRPGCurrencyHolder;
class URPGItemDefinition;

UCLASS()
class RPGCORE_API URPGCoreStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/** Finds an interface on the actor or any of its components. */
	template <typename TInterface, typename TUInterface>
	static TInterface* FindInterface(AActor* Actor)
	{
		if (!Actor)
		{
			return nullptr;
		}
		if (TInterface* OnActor = Cast<TInterface>(Actor))
		{
			return OnActor;
		}
		return Cast<TInterface>(Actor->FindComponentByInterface(TUInterface::StaticClass()));
	}

	static IRPGItemContainer* FindItemContainer(AActor* Actor);
	static IRPGCurrencyHolder* FindCurrencyHolder(AActor* Actor);

	// ---------- Items ----------
	UFUNCTION(BlueprintPure, Category = "RPG|Items")
	static int32 GetItemCount(AActor* Target, const URPGItemDefinition* Item);

	UFUNCTION(BlueprintCallable, Category = "RPG|Items")
	static int32 AddItem(AActor* Target, const URPGItemDefinition* Item, int32 Quantity = 1);

	UFUNCTION(BlueprintCallable, Category = "RPG|Items")
	static int32 RemoveItem(AActor* Target, const URPGItemDefinition* Item, int32 Quantity = 1);

	// ---------- Currency ----------
	UFUNCTION(BlueprintPure, Category = "RPG|Currency")
	static int64 GetCurrency(AActor* Target, FGameplayTag CurrencyTag);

	UFUNCTION(BlueprintCallable, Category = "RPG|Currency")
	static bool SpendCurrency(AActor* Target, FGameplayTag CurrencyTag, int64 Amount);
};
