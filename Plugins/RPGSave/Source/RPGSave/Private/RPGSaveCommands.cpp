// RPGSaveCommands.cpp
// rpg.Save [Slot]   rpg.Load [Slot]   (default slot "Quick")

#include "Engine/World.h"
#include "HAL/IConsoleManager.h"
#include "RPGSave.h"
#include "RPGSaveSubsystem.h"

namespace
{
	FString GetSlotArg(const TArray<FString>& Args)
	{
		return Args.Num() > 0 && !Args[0].IsEmpty() ? Args[0] : FString(TEXT("Quick"));
	}

	FAutoConsoleCommandWithWorldAndArgs SaveCommand(
		TEXT("rpg.Save"),
		TEXT("Saves the game. Usage: rpg.Save [SlotName]"),
		FConsoleCommandWithWorldAndArgsDelegate::CreateLambda([](const TArray<FString>& Args, UWorld* World)
		{
			if (URPGSaveSubsystem* SaveSubsystem = URPGSaveSubsystem::Get(World))
			{
				SaveSubsystem->SaveToSlot(GetSlotArg(Args));
			}
		}));

	FAutoConsoleCommandWithWorldAndArgs LoadCommand(
		TEXT("rpg.Load"),
		TEXT("Loads a save. Usage: rpg.Load [SlotName]"),
		FConsoleCommandWithWorldAndArgsDelegate::CreateLambda([](const TArray<FString>& Args, UWorld* World)
		{
			if (URPGSaveSubsystem* SaveSubsystem = URPGSaveSubsystem::Get(World))
			{
				SaveSubsystem->LoadFromSlot(GetSlotArg(Args));
			}
		}));
}
