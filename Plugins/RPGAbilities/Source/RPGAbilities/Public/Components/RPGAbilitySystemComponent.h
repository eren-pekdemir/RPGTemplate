// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "Attributes/RPGAttributeSet.h"
#include "RPGAbilitySystemComponent.generated.h"


UCLASS(ClassGroup=(RPG), meta=(BlueprintSpawnableComponent))
class RPGABILITIES_API URPGAbilitySystemComponent : public UAbilitySystemComponent
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

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
	virtual void InitializeComponent() override;

private:
	void CreateDefaultAttributeSets();
	void ApplyDefaultEffects();
};
