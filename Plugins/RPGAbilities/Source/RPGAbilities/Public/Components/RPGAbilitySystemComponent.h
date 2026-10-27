// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "Attributes/RPGAttributeSet.h"
#include "Interfaces/RPGStatReceiver.h"
#include "Interfaces/RPGTagReceiver.h"
#include "RPGAbilitySystemComponent.generated.h"

class URPGGameplayAbility;

UCLASS(ClassGroup=(RPG), meta=(BlueprintSpawnableComponent))
class RPGABILITIES_API URPGAbilitySystemComponent : public UAbilitySystemComponent , public IRPGStatReceiver, public IRPGTagReceiver
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	URPGAbilitySystemComponent();
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RPG|Abilities")
	TArray<TSubclassOf<UAttributeSet>> DefaultAttributeSets;

	/** Effects applied once on BeginPlay (starting values, passives). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RPG|Abilities")
	TArray<TSubclassOf<UGameplayEffect>> DefaultEffects;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<UGameplayEffect> StatModifierEffect;
	
	virtual void SetStatModifiers(const UObject* Source, const TArray<FRPGStatModifier>& Modifiers) override;
	
	UPROPERTY(EditAnywhere) 
	TArray<TSubclassOf<URPGGameplayAbility>> DefaultAbilities;
	
	UFUNCTION(BlueprintCallable) 
	void AbilityInputTagPressed(FGameplayTag InputTag);
	
	UFUNCTION(BlueprintCallable) 
	void AbilityInputTagReleased(FGameplayTag InputTag);
	
	UFUNCTION()
	virtual void SetGrantedTags(const UObject* Source, const FGameplayTagContainer& Tags) override;

	/**
	 * Adds a loose tag and removes it after Duration seconds (e.g. State.Invulnerable for roll i-frames).
	 * Loose tags are counted, so overlapping calls keep the tag until the last one expires.
	 */
	UFUNCTION(BlueprintCallable, Category = "RPG|Abilities")
	void AddLooseTagForDuration(FGameplayTag Tag, float Duration);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
	virtual void InitializeComponent() override;

private:
	void CreateDefaultAttributeSets();
	void ApplyDefaultEffects();
	
	TMap<TWeakObjectPtr<const UObject>, FActiveGameplayEffectHandle> StatModifierHandles;
	
	void GrantDefaultAbilities();
	
	TMap<TWeakObjectPtr<const UObject>, FGameplayTagContainer> GrantedTagsBySource;
	
	
};
