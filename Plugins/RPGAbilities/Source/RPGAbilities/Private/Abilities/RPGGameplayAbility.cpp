// RPGGameplayAbility.cpp

#include "Abilities/RPGGameplayAbility.h"

#include "RPGCoreTags.h"

URPGGameplayAbility::URPGGameplayAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	ActivationBlockedTags.AddTag(RPGTags::State_Dead);
}
