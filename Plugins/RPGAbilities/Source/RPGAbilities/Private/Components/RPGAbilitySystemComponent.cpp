// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/RPGAbilitySystemComponent.h"

#include "RPGAbilities.h"


URPGAbilitySystemComponent::URPGAbilitySystemComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	bWantsInitializeComponent = true;
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

