// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/RPGGameplayAbility.h"

#include "RPGCoreTags.h"

URPGGameplayAbility::URPGGameplayAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	ActivationBlockedTags.AddTag(RPGTags::State_Dead);
}
