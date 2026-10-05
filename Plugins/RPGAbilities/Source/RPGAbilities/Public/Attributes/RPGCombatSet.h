// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "RPGAttributeSet.h"
#include "RPGCombatSet.generated.h"

/**
 * 
 */
UCLASS()
class RPGABILITIES_API URPGCombatSet : public URPGAttributeSet
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	FGameplayAttributeData Damage;
	RPG_ATTRIBUTE_ACCESSORS(URPGCombatSet, Damage)
	
	
	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	FGameplayAttributeData Armor;
	RPG_ATTRIBUTE_ACCESSORS(URPGCombatSet, Armor)
};
