// RPGFactionMemberComponent.cpp

#include "Components/RPGFactionMemberComponent.h"

#include "Factions/RPGFactionDefinition.h"
#include "Interfaces/RPGReputationHolder.h"
#include "RPGFactionSettings.h"

URPGFactionMemberComponent::URPGFactionMemberComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void URPGFactionMemberComponent::ReportKilledBy(AActor* Killer)
{
	if (bKillReported || !Faction.IsValid() || KillReputationChange == 0)
	{
		return;
	}

	if (IRPGReputationHolder* Holder = IRPGReputationHolder::Find(Killer))
	{
		bKillReported = true;
		Holder->AddReputation(Faction, KillReputationChange);
	}
}

int32 URPGFactionMemberComponent::GetReputationOf(AActor* Other) const
{
	const IRPGReputationHolder* Holder = IRPGReputationHolder::Find(Other);
	return Holder ? Holder->GetReputation(Faction) : 0;
}

bool URPGFactionMemberComponent::IsHostileTo(AActor* Other) const
{
	const IRPGReputationHolder* Holder = IRPGReputationHolder::Find(Other);
	const URPGFactionDefinition* Definition = URPGFactionSettings::FindFaction(Faction);
	if (!Holder || !Definition)
	{
		return false;
	}

	const int32 RankIndex = Definition->GetRankIndex(Holder->GetReputation(Faction));
	return RankIndex != INDEX_NONE && Definition->Ranks[RankIndex].bHostile;
}
