// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/RPGAbilitySystemComponent.h"
#include "RPGAbilities.h"
#include "GameplayEffect.h"
#include "AttributeSet.h"


URPGAbilitySystemComponent::URPGAbilitySystemComponent()
{
	bWantsInitializeComponent = true;
}


void URPGAbilitySystemComponent::SetStatModifiers(const UObject* Source, const TArray<FRPGStatModifier>& Modifiers)
{
	FActiveGameplayEffectHandle OldHandle;
	if (StatModifierHandles.RemoveAndCopyValue(Source, OldHandle))
	{
		RemoveActiveGameplayEffect(OldHandle);
	}
	if (Modifiers.IsEmpty()) return;
	
	if (!StatModifierEffect)
	{
		UE_LOG(LogRPGAbilities, Warning, TEXT("%s: StatModifierEffect is not set, stat modifiers ignored"),*GetNameSafe(GetOwner()));
		return;
	}
	
	FGameplayEffectContextHandle Context = MakeEffectContext();
	Context.AddSourceObject(Source);
	FGameplayEffectSpecHandle SpecHandle =MakeOutgoingSpec(StatModifierEffect, 1.f, Context);
	if (!SpecHandle.IsValid()) return;
	
	const UGameplayEffect* CDO = StatModifierEffect->GetDefaultObject<UGameplayEffect>();
	for (const FGameplayModifierInfo& ModInfo : CDO->Modifiers)
	{
		FGameplayTag Tag = ModInfo.ModifierMagnitude.GetSetByCallerFloat().DataTag;
		if (Tag.IsValid())
		{
			SpecHandle.Data->SetSetByCallerMagnitude(Tag,0);
		}
	}	
	
	TMap<FGameplayTag, float> Totals;
	
	for (const auto& Mod : Modifiers)
	{
		Totals.FindOrAdd(Mod.Stat) += Mod.Value;
	}
	
	for (const  auto&  Pair : Totals)
	{
		SpecHandle.Data->SetSetByCallerMagnitude(Pair.Key,Pair.Value);
	}
	
	FActiveGameplayEffectHandle ActiveHandle = ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	StatModifierHandles.Add(Source, ActiveHandle);
}

void URPGAbilitySystemComponent::BeginPlay()
{
	Super::BeginPlay();

	ApplyDefaultEffects();
	
}

void URPGAbilitySystemComponent::InitializeComponent()
{
	Super::InitializeComponent();
	
	CreateDefaultAttributeSets();
}

void URPGAbilitySystemComponent::CreateDefaultAttributeSets()
{
	AActor* Owner = GetOwner();
	if (!Owner) return;

	for (const TSubclassOf<UAttributeSet>& SetClass : DefaultAttributeSets)
	{
		if (!SetClass)
		{
			UE_LOG(LogRPGAbilities, Warning, TEXT("%s: DefaultAttributeSets contains an empty entry"),
				*GetNameSafe(Owner));
			continue;
		}

		// GAS supports one set per class; a duplicate would be ignored by lookups.
		if (GetAttributeSet(SetClass))
		{
			UE_LOG(LogRPGAbilities, Warning, TEXT("%s: attribute set %s is listed twice"),
				*GetNameSafe(Owner), *GetNameSafe(SetClass));
			continue;
		}

		UAttributeSet* NewSet = NewObject<UAttributeSet>(Owner, SetClass);
		AddSpawnedAttribute(NewSet);
	}
}

void URPGAbilitySystemComponent::ApplyDefaultEffects()
{
	const FGameplayEffectContextHandle Context = MakeEffectContext();

	for (const TSubclassOf<UGameplayEffect>& EffectClass : DefaultEffects)
	{
		if (!EffectClass)
		{
			UE_LOG(LogRPGAbilities, Warning, TEXT("%s: DefaultEffects contains an empty entry"),
				*GetNameSafe(GetOwner()));
			continue;
		}

		const FGameplayEffectSpecHandle SpecHandle = MakeOutgoingSpec(EffectClass, 1.f, Context);
		if (!SpecHandle.IsValid()) continue;

		ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	}
}

