// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "RPGAttributeSet.h"
#include "RPGHealthSet.generated.h"

struct FGameplayEffectModCallbackData;

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
	
	/** Before a BASE value changes (Instant effects). Clamp here. */
   virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;

	/** Before a CURRENT value changes (Infinite / Duration effects, e.g. equipment). Clamp here. */
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

	/** After any value changed. If MaxHealth dropped below Health, lower Health. */
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;

	/** After an Instant effect executed on this set. Convert IncomingDamage into a Health loss. */
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
	
private:
	/** Health -> [0, MaxHealth], MaxHealth -> at least 1. Other attributes are left untouched. */
	void ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const;
};
