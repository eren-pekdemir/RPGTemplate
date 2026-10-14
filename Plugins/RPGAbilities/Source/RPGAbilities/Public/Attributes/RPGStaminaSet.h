#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "Attributes/RPGAttributeSet.h"
#include "RPGStaminaSet.generated.h"

/** Stamina spent by abilities (Cost effects) and restored by regeneration effects. */
UCLASS()
class RPGABILITIES_API URPGStaminaSet : public URPGAttributeSet
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Category = "RPG|Stamina")
	FGameplayAttributeData Stamina;
	RPG_ATTRIBUTE_ACCESSORS(URPGStaminaSet, Stamina)

	UPROPERTY(BlueprintReadOnly, Category = "RPG|Stamina")
	FGameplayAttributeData MaxStamina;
	RPG_ATTRIBUTE_ACCESSORS(URPGStaminaSet, MaxStamina)

	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;

private:
	/** Stamina -> [0, MaxStamina], MaxStamina -> at least 1. */
	void ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const;
};