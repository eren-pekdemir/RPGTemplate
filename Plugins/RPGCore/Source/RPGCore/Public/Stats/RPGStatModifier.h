#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "RPGStatModifier.generated.h"  

USTRUCT(BlueprintType)
struct RPGCORE_API FRPGStatModifier
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats", meta = (Categories = "Stat"))
	FGameplayTag Stat;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
	float Value = 0.f;
};