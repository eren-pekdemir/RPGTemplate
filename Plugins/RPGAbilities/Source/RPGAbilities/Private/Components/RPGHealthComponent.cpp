// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/RPGHealthComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "GameplayEffectExtension.h"
#include "Attributes/RPGHealthSet.h"
#include "RPGAbilities.h"
#include "RPGCoreTags.h"

URPGHealthComponent::URPGHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

float URPGHealthComponent::GetHealth() const
{
	return AbilitySystem.IsValid() ? AbilitySystem->GetNumericAttribute(URPGHealthSet::GetHealthAttribute()) : 0.f;
}

float URPGHealthComponent::GetMaxHealth() const
{
	return AbilitySystem.IsValid() ? AbilitySystem->GetNumericAttribute(URPGHealthSet::GetMaxHealthAttribute()) : 0.f;
}

float URPGHealthComponent::GetHealthNormalized() const
{
	return GetMaxHealth() > 0.f ? GetHealth() / GetMaxHealth() : 0.f;
}

bool URPGHealthComponent::IsDead() const
{
	return bIsDead;
}

void URPGHealthComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (AbilitySystem.IsValid())
	{
		AbilitySystem->GetGameplayAttributeValueChangeDelegate(URPGHealthSet::GetHealthAttribute()).Remove(HealthChangedHandle);
	}
	Super::EndPlay(EndPlayReason);
}

void URPGHealthComponent::HandleHealthChanged(const FOnAttributeChangeData& Data)
{
	AActor* Instigator = nullptr;
	if (Data.GEModData)
	{
		 Instigator = Data.GEModData->EffectSpec.GetContext().GetOriginalInstigator();
	}
	
	OnHealthChanged.Broadcast(this, Data.OldValue, Data.NewValue, Instigator);
	if (!bIsDead && Data.NewValue <= 0.f)
	{
		HandleDeath(Instigator);
	}
}

void URPGHealthComponent::HandleDeath(AActor* Killer)
{
	bIsDead = true;
	AbilitySystem->AddLooseGameplayTag(RPGTags::State_Dead);
	OnDeath.Broadcast(GetOwner(), Killer);
}


// Called when the game starts
void URPGHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner());
	if (!ASC)
	{
		UE_LOG(LogRPGAbilities, Warning, TEXT("On %s Couldn't find ability system component!"), *GetNameSafe(GetOwner()));
		return;
	}
	
	AbilitySystem = ASC;
	HealthChangedHandle = AbilitySystem->GetGameplayAttributeValueChangeDelegate(URPGHealthSet::GetHealthAttribute()).AddUObject(this, &ThisClass::HandleHealthChanged);
}

