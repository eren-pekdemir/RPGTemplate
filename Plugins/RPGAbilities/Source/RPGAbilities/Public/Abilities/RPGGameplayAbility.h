// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "RPGGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class RPGABILITIES_API URPGGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
public:
	URPGGameplayAbility();
	
	UPROPERTY(EditDefaultsOnly, Category = "RPGInput", meta = (Categories = InputTag))
	FGameplayTag InputTag;
};
