// RPGFactionSettings.cpp

#include "RPGFactionSettings.h"

#include "Factions/RPGFactionDefinition.h"

const URPGFactionDefinition* URPGFactionSettings::FindFaction(const FGameplayTag& FactionTag)
{
	if (!FactionTag.IsValid())
	{
		return nullptr;
	}

	for (const TSoftObjectPtr<URPGFactionDefinition>& SoftFaction : GetDefault<URPGFactionSettings>()->Factions)
	{
		const URPGFactionDefinition* Faction = SoftFaction.LoadSynchronous();
		if (Faction && Faction->FactionTag == FactionTag)
		{
			return Faction;
		}
	}
	return nullptr;
}

TArray<const URPGFactionDefinition*> URPGFactionSettings::GetAllFactions()
{
	TArray<const URPGFactionDefinition*> Result;
	for (const TSoftObjectPtr<URPGFactionDefinition>& SoftFaction : GetDefault<URPGFactionSettings>()->Factions)
	{
		if (const URPGFactionDefinition* Faction = SoftFaction.LoadSynchronous())
		{
			Result.Add(Faction);
		}
	}
	return Result;
}
