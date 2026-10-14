#include "Components/RPGExperienceRewardComponent.h"

#include "Components/RPGProgressionComponent.h"
#include "RPGProgression.h"

URPGExperienceRewardComponent::URPGExperienceRewardComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

bool URPGExperienceRewardComponent::GrantReward(AActor* Receiver)
{
	if (bRewardGranted || ExperienceReward <= 0)
	{
		return false;
	}

	URPGProgressionComponent* Progression = URPGProgressionComponent::FindProgressionComponent(Receiver);
	if (!Progression)
	{
		UE_LOG(LogRPGProgression, Verbose, TEXT("%s: no progression found for %s, reward skipped."),
			*GetNameSafe(GetOwner()), *GetNameSafe(Receiver));
		return false;
	}

	// State first, then the call: AddExperience may broadcast level-up events.
	bRewardGranted = true;
	Progression->AddExperience(ExperienceReward);
	return true;
}