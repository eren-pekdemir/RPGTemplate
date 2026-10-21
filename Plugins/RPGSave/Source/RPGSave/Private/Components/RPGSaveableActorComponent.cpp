// RPGSaveableActorComponent.cpp

#include "Components/RPGSaveableActorComponent.h"

#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "RPGSave.h"
#include "RPGSaveSubsystem.h"

URPGSaveableActorComponent::URPGSaveableActorComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void URPGSaveableActorComponent::BeginPlay()
{
	Super::BeginPlay();

	const AActor* Owner = GetOwner();
	const UWorld* World = GetWorld();

	// Level actors begin play while the world starts (or while their streaming level loads);
	// anything spawned later has a generated name that is not stable between sessions.
	bPlacedInLevel = Owner && World &&
		(Owner->IsNetStartupActor() || !World->HasBegunPlay() || Owner->IsActorBeginningPlayFromLevelStreaming());

	if (!bPlacedInLevel)
	{
		UE_LOG(LogRPGSave, Verbose, TEXT("%s was spawned at runtime and will not be saved."), *GetNameSafe(Owner));
	}
}

void URPGSaveableActorComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (EndPlayReason == EEndPlayReason::Destroyed && bSaveDestruction && bPlacedInLevel)
	{
		const UGameInstance* GameInstance = GetWorld() ? GetWorld()->GetGameInstance() : nullptr;
		if (URPGSaveSubsystem* SaveSubsystem = GameInstance ? GameInstance->GetSubsystem<URPGSaveSubsystem>() : nullptr)
		{
			SaveSubsystem->RegisterDestroyedActor(GetWorld(), GetSaveKey());
		}
	}

	Super::EndPlay(EndPlayReason);
}

FString URPGSaveableActorComponent::GetSaveKey() const
{
	return GetOwner() ? UWorld::RemovePIEPrefix(GetOwner()->GetPathName()) : FString();
}
