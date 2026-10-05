// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "RPGAttributeSet.h"
#include "RPGHealthSet.generated.h"

/**
 * 
 */
UCLASS()
class RPGABILITIES_API URPGHealthSet : public URPGAttributeSet
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly, Category = "Health")
	FGameplayAttributeData Health;
	RPG_ATTRIBUTE_ACCESSORS(URPGHealthSet, Health)
	
	UPROPERTY(BlueprintReadOnly, Category = "Health")
	FGameplayAttributeData MaxHealth;
	RPG_ATTRIBUTE_ACCESSORS(URPGHealthSet, MaxHealth)
	
	UPROPERTY(BlueprintReadOnly, Category = "Health")
	FGameplayAttributeData IncomingDamage;
	RPG_ATTRIBUTE_ACCESSORS(URPGHealthSet, IncomingDamage)
};
