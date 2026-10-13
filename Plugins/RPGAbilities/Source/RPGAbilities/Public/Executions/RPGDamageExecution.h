// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "Attributes/RPGCombatSet.h"
#include "RPGDamageExecution.generated.h"

struct FRPGDamageStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(Damage);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Armor);

	FRPGDamageStatics()
	{
		// Class, Property, Source/Target, bSnapshot
		DEFINE_ATTRIBUTE_CAPTUREDEF(URPGCombatSet, Damage, Source, true);
		DEFINE_ATTRIBUTE_CAPTUREDEF(URPGCombatSet, Armor,  Target, false);
	}
};

static const FRPGDamageStatics& DamageStatics()
{
	static FRPGDamageStatics Statics;
	return Statics;
}
UCLASS()
class RPGABILITIES_API URPGDamageExecution : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
public:
	
	URPGDamageExecution();
	
	UPROPERTY(EditDefaultsOnly) 
	float MinimumDamage = 1.f;
	
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
	
};
