// RPGFactionDebugCommands.cpp
// rpg.Reputation.Add <FactionTag> <Amount>   e.g. rpg.Reputation.Add Faction.Village 250
// rpg.Reputation.Print

#include "Components/RPGReputationComponent.h"
#include "Engine/World.h"
#include "Factions/RPGFactionDefinition.h"
#include "GameFramework/PlayerController.h"
#include "HAL/IConsoleManager.h"
#include "RPGFaction.h"
#include "RPGFactionSettings.h"

namespace
{
	URPGReputationComponent* FindPlayerReputation(UWorld* World)
	{
		APlayerController* PlayerController = World ? World->GetFirstPlayerController() : nullptr;
		URPGReputationComponent* Reputation = URPGReputationComponent::FindReputationComponent(PlayerController);
		if (!Reputation)
		{
			UE_LOG(LogRPGFaction, Warning, TEXT("No RPGReputationComponent on the player's PlayerState."));
		}
		return Reputation;
	}

	FAutoConsoleCommandWithWorldAndArgs AddReputationCommand(
		TEXT("rpg.Reputation.Add"),
		TEXT("Changes reputation (relations applied). Usage: rpg.Reputation.Add Faction.Village 50"),
		FConsoleCommandWithWorldAndArgsDelegate::CreateLambda([](const TArray<FString>& Args, UWorld* World)
		{
			if (Args.Num() < 2)
			{
				UE_LOG(LogRPGFaction, Warning, TEXT("Usage: rpg.Reputation.Add <FactionTag> <Amount>"));
				return;
			}

			const FGameplayTag Faction = FGameplayTag::RequestGameplayTag(FName(*Args[0]), false);
			if (!Faction.IsValid())
			{
				UE_LOG(LogRPGFaction, Warning, TEXT("Unknown tag '%s'."), *Args[0]);
				return;
			}

			if (URPGReputationComponent* Reputation = FindPlayerReputation(World))
			{
				Reputation->ChangeReputation(Faction, FCString::Atoi(*Args[1]), true);
				UE_LOG(LogRPGFaction, Log, TEXT("%s = %d"), *Faction.ToString(), Reputation->GetReputation(Faction));
			}
		}));

	FAutoConsoleCommandWithWorldAndArgs PrintReputationCommand(
		TEXT("rpg.Reputation.Print"),
		TEXT("Logs the player's reputation with every faction."),
		FConsoleCommandWithWorldAndArgsDelegate::CreateLambda([](const TArray<FString>& Args, UWorld* World)
		{
			URPGReputationComponent* Reputation = FindPlayerReputation(World);
			if (!Reputation)
			{
				return;
			}

			for (const URPGFactionDefinition* Faction : URPGFactionSettings::GetAllFactions())
			{
				FRPGReputationRank Rank;
				const bool bHasRank = Reputation->GetRank(Faction->FactionTag, Rank);
				UE_LOG(LogRPGFaction, Log, TEXT("%s: %d (%s)"), *Faction->FactionTag.ToString(),
					Reputation->GetReputation(Faction->FactionTag), bHasRank ? *Rank.DisplayName.ToString() : TEXT("no rank"));
			}
		}));
}
