#pragma once

#include "CoreMinimal.h"
#include "RPGProgressionTypes.generated.h"

UENUM(BlueprintType)
enum class ERPGSkillUnlockResult : uint8
{
	Success,
	InvalidSkill,
	AlreadyUnlocked,
	LevelTooLow,
	MissingPrerequisite,
	NotEnoughPoints
};