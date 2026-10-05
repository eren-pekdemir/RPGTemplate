// Fill out your copyright notice in the Description page of Project Settings.


#include "Attributes/RPGHealthSet.h"
#include "GameplayEffectExtension.h"

void URPGHealthSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);
	
	ClampAttribute(Attribute, NewValue);
}

void URPGHealthSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
	
	ClampAttribute(Attribute, NewValue);
}

void URPGHealthSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);
	
	if (Attribute == GetMaxHealthAttribute())
	{
		if (GetHealth() > GetMaxHealth())
		{
			SetHealth(NewValue);
		}
	}
}

void URPGHealthSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
	
	if (Data.EvaluatedData.Attribute == GetIncomingDamageAttribute())
	{
		float Damage = GetIncomingDamage();
		SetIncomingDamage(0.f);
		float DamagedHealthValue = Health.GetCurrentValue() - Damage; 
		if (DamagedHealthValue > 0)
		{
			SetHealth(DamagedHealthValue);
		}
		else
		{
			SetHealth(0.f);
		}
	}
}

void URPGHealthSet::ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const
{
	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, MaxHealth.GetCurrentValue());
	}
	if (Attribute == GetMaxHealthAttribute())
	{
		NewValue = FMath::Max(NewValue, 1.f);
	}
}
