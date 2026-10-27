// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/RPGAbilitySystemComponent.h"
#include "RPGAbilities.h"
#include "GameplayEffect.h"
#include "AttributeSet.h"
#include "Abilities/RPGGameplayAbility.h"
#include "Misc/ScopeExit.h"
#include "TimerManager.h"


URPGAbilitySystemComponent::URPGAbilitySystemComponent()
{
	bWantsInitializeComponent = true;
}


void URPGAbilitySystemComponent::SetStatModifiers(const UObject* Source, const TArray<FRPGStatModifier>& Modifiers)
{
	// The new snapshot is applied BEFORE the old one is removed. Removing first would drop MaxHealth for a moment,
	// clamp Health down to it, and the health would not come back when the same bonus is re-applied.
	FActiveGameplayEffectHandle OldHandle;
	const bool bHadOld = StatModifierHandles.RemoveAndCopyValue(Source, OldHandle);

	ON_SCOPE_EXIT
	{
		if (bHadOld)
		{
			RemoveActiveGameplayEffect(OldHandle);
		}
	};

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

void URPGAbilitySystemComponent::AddLooseTagForDuration(FGameplayTag Tag, float Duration)
{
	if (!Tag.IsValid() || Duration <= 0.f) return;

	UWorld* World = GetWorld();
	if (!World) return;

	AddLooseGameplayTag(Tag);

	// Weak lambda: the timer does nothing if the component is gone (owner destroyed mid-roll).
	FTimerHandle Handle;
	World->GetTimerManager().SetTimer(Handle, FTimerDelegate::CreateWeakLambda(this, [this, Tag]()
	{
		RemoveLooseGameplayTag(Tag);
	}), Duration, false);
}

void URPGAbilitySystemComponent::AbilityInputTagPressed(FGameplayTag InputTag)
{
	if (!InputTag.IsValid()) return;

	// Phase 1: find matching abilities. Nothing here may change the ability list.
	TArray<FGameplayAbilitySpecHandle> HandlesToActivate;

	for (FGameplayAbilitySpec& Spec : GetActivatableAbilities())
	{
		if (!Spec.GetDynamicSpecSourceTags().HasTagExact(InputTag)) continue;

		// Marks the input as held; if the ability is already running, it receives InputPressed.
		AbilitySpecInputPressed(Spec);

		if (!Spec.IsActive())
		{
			HandlesToActivate.AddUnique(Spec.Handle);
		}
	}

	// Phase 2: activate. An ability may grant or remove abilities here, which is now safe.
	for (const FGameplayAbilitySpecHandle& Handle : HandlesToActivate)
	{
		TryActivateAbility(Handle);
	}
}

void URPGAbilitySystemComponent::AbilityInputTagReleased(FGameplayTag InputTag)
{
	if (!InputTag.IsValid()) return;
	
	for (FGameplayAbilitySpec& Spec : GetActivatableAbilities())
	{
		if (!Spec.GetDynamicSpecSourceTags().HasTagExact(InputTag)) continue;
		AbilitySpecInputReleased(Spec);
	}
}

void URPGAbilitySystemComponent::SetGrantedTags(const UObject* Source, const FGameplayTagContainer& Tags)
{
	// Add the new snapshot before removing the old one (loose tags are counted): a tag present in both
	// never drops to zero, so effects and abilities that depend on it are not switched off and on again.
	FGameplayTagContainer OldTags;
	GrantedTagsBySource.RemoveAndCopyValue(Source, OldTags);

	if (!Tags.IsEmpty())
	{
		AddLooseGameplayTags(Tags);
		GrantedTagsBySource.Add(Source, Tags);
	}

	RemoveLooseGameplayTags(OldTags);
}

void URPGAbilitySystemComponent::BeginPlay()
{
	Super::BeginPlay();

	ApplyDefaultEffects();
	GrantDefaultAbilities();
	
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

void URPGAbilitySystemComponent::GrantDefaultAbilities()
{
	for (const TSubclassOf<URPGGameplayAbility>& AbilityClass : DefaultAbilities)
	{
		if (!AbilityClass)
		{
			UE_LOG(LogRPGAbilities, Warning, TEXT("DefaultAbilities contains an empty entry"));
			continue;
		}
		
		const URPGGameplayAbility* AbilityCDO = AbilityClass->GetDefaultObject<URPGGameplayAbility>();
		FGameplayAbilitySpec Spec(AbilityClass, 1, INDEX_NONE, GetOwner());
		
		if (AbilityCDO->InputTag.IsValid())
		{
			Spec.GetDynamicSpecSourceTags().AddTag(AbilityCDO->InputTag);
		}
		
		GiveAbility(Spec);
	}
	
}

