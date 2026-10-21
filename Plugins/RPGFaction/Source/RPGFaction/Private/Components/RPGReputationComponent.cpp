// RPGReputationComponent.cpp

#include "Components/RPGReputationComponent.h"

#include "Factions/RPGFactionDefinition.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerState.h"
#include "Interfaces/RPGTagReceiver.h"
#include "RPGCoreStatics.h"
#include "RPGFaction.h"
#include "RPGFactionSettings.h"

URPGReputationComponent::URPGReputationComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void URPGReputationComponent::BeginPlay()
{
	Super::BeginPlay();

	// Starting values for factions the player has never met.
	for (const URPGFactionDefinition* Faction : URPGFactionSettings::GetAllFactions())
	{
		if (Faction->FactionTag.IsValid() && !Reputation.Contains(Faction->FactionTag))
		{
			Reputation.Add(Faction->FactionTag, Faction->StartingReputation);
		}
	}

	if (APlayerState* PlayerState = Cast<APlayerState>(GetOwner()))
	{
		PlayerState->OnPawnSet.AddUniqueDynamic(this, &ThisClass::HandlePawnSet);
	}

	PushRankTags();
}

void URPGReputationComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (APlayerState* PlayerState = Cast<APlayerState>(GetOwner()))
	{
		PlayerState->OnPawnSet.RemoveDynamic(this, &ThisClass::HandlePawnSet);
	}
	Super::EndPlay(EndPlayReason);
}

URPGReputationComponent* URPGReputationComponent::FindReputationComponent(AActor* Actor)
{
	IRPGReputationHolder* Holder = IRPGReputationHolder::Find(Actor);
	return Holder ? Cast<URPGReputationComponent>(Holder->_getUObject()) : nullptr;
}

// ===================================================================== Reading

int32 URPGReputationComponent::GetReputation(const FGameplayTag& Faction) const
{
	if (const int32* Found = Reputation.Find(Faction))
	{
		return *Found;
	}

	const URPGFactionDefinition* Definition = URPGFactionSettings::FindFaction(Faction);
	return Definition ? Definition->StartingReputation : 0;
}

int32 URPGReputationComponent::GetRankIndex(FGameplayTag Faction) const
{
	const URPGFactionDefinition* Definition = URPGFactionSettings::FindFaction(Faction);
	return Definition ? Definition->GetRankIndex(GetReputation(Faction)) : INDEX_NONE;
}

bool URPGReputationComponent::GetRank(FGameplayTag Faction, FRPGReputationRank& OutRank) const
{
	const URPGFactionDefinition* Definition = URPGFactionSettings::FindFaction(Faction);
	const int32 RankIndex = Definition ? Definition->GetRankIndex(GetReputation(Faction)) : INDEX_NONE;
	if (RankIndex == INDEX_NONE)
	{
		return false;
	}

	OutRank = Definition->Ranks[RankIndex];
	return true;
}

bool URPGReputationComponent::IsHostileWith(FGameplayTag Faction) const
{
	FRPGReputationRank Rank;
	return GetRank(Faction, Rank) && Rank.bHostile;
}

// ===================================================================== Writing

void URPGReputationComponent::AddReputation(const FGameplayTag& Faction, int32 Amount)
{
	ChangeReputation(Faction, Amount, true);
}

void URPGReputationComponent::ChangeReputation(FGameplayTag Faction, int32 Amount, bool bApplyRelations)
{
	if (!Faction.IsValid() || Amount == 0)
	{
		return;
	}

	SetReputationInternal(Faction, GetReputation(Faction) + Amount);

	if (!bApplyRelations)
	{
		return;
	}

	// One level deep: Village +20 -> Guards +10, Bandits -20. Relations of relations are not followed.
	if (const URPGFactionDefinition* Definition = URPGFactionSettings::FindFaction(Faction))
	{
		for (const FRPGFactionRelation& Relation : Definition->Relations)
		{
			const int32 SpillOver = FMath::RoundToInt(static_cast<float>(Amount) * Relation.Multiplier);
			if (Relation.Faction.IsValid() && SpillOver != 0)
			{
				SetReputationInternal(Relation.Faction, GetReputation(Relation.Faction) + SpillOver);
			}
		}
	}
}

void URPGReputationComponent::SetReputation(FGameplayTag Faction, int32 NewReputation)
{
	if (Faction.IsValid())
	{
		SetReputationInternal(Faction, NewReputation);
	}
}

void URPGReputationComponent::SetReputationInternal(const FGameplayTag& Faction, int32 NewReputation)
{
	const URPGFactionSettings* Settings = GetDefault<URPGFactionSettings>();
	NewReputation = FMath::Clamp(NewReputation, Settings->MinReputation, Settings->MaxReputation);

	const int32 OldReputation = GetReputation(Faction);
	if (OldReputation == NewReputation && Reputation.Contains(Faction))
	{
		return;
	}

	const URPGFactionDefinition* Definition = URPGFactionSettings::FindFaction(Faction);
	const int32 OldRank = Definition ? Definition->GetRankIndex(OldReputation) : INDEX_NONE;
	const int32 NewRank = Definition ? Definition->GetRankIndex(NewReputation) : INDEX_NONE;

	// State first, then events.
	Reputation.Add(Faction, NewReputation);

	if (OldRank != NewRank)
	{
		PushRankTags();
	}

	OnReputationChanged.Broadcast(Faction, NewReputation, NewReputation - OldReputation);

	if (OldRank != NewRank)
	{
		OnRankChanged.Broadcast(Faction, NewRank, OldRank);
	}
}

// ===================================================================== Rank tags

AActor* URPGReputationComponent::GetTargetActor() const
{
	if (const APlayerState* PlayerState = Cast<APlayerState>(GetOwner()))
	{
		return PlayerState->GetPawn();
	}
	return GetOwner();
}

void URPGReputationComponent::HandlePawnSet(APlayerState* Player, APawn* NewPawn, APawn* OldPawn)
{
	// A respawned pawn starts without our tags.
	PushRankTags();
}

void URPGReputationComponent::PushRankTags()
{
	IRPGTagReceiver* Receiver = URPGCoreStatics::FindTagReceiver(GetTargetActor());
	if (!Receiver)
	{
		return;
	}

	FGameplayTagContainer RankTags;
	for (const TPair<FGameplayTag, int32>& Pair : Reputation)
	{
		const URPGFactionDefinition* Definition = URPGFactionSettings::FindFaction(Pair.Key);
		const int32 RankIndex = Definition ? Definition->GetRankIndex(Pair.Value) : INDEX_NONE;
		if (RankIndex != INDEX_NONE && Definition->Ranks[RankIndex].GrantedTag.IsValid())
		{
			RankTags.AddTag(Definition->Ranks[RankIndex].GrantedTag);
		}
	}

	// Snapshot keyed by source: replaces whatever this component granted before.
	Receiver->SetGrantedTags(this, RankTags);
}

// ===================================================================== Save

void URPGReputationComponent::OnLoadedFromSave()
{
	PushRankTags();

	for (const TPair<FGameplayTag, int32>& Pair : Reputation)
	{
		OnReputationChanged.Broadcast(Pair.Key, Pair.Value, 0);
	}
}
