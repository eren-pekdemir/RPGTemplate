// RPGSaveSubsystem.cpp

#include "RPGSaveSubsystem.h"

#include "Components/RPGSaveableActorComponent.h"
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerState.h"
#include "Interfaces/RPGSaveable.h"
#include "Kismet/GameplayStatics.h"
#include "RPGSave.h"
#include "Serialization/MemoryReader.h"
#include "Serialization/MemoryWriter.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"
#include "TimerManager.h"
#include "UObject/UObjectGlobals.h"

// ===================================================================== Lifetime

void URPGSaveSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	PostLoadMapHandle = FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &ThisClass::HandlePostLoadMap);

	if (GEngine)
	{
		TravelFailureHandle = GEngine->OnTravelFailure().AddUObject(this, &ThisClass::HandleTravelFailure);
	}
}

void URPGSaveSubsystem::Deinitialize()
{
	FCoreUObjectDelegates::PostLoadMapWithWorld.Remove(PostLoadMapHandle);

	if (GEngine)
	{
		GEngine->OnTravelFailure().Remove(TravelFailureHandle);
	}

	PendingLoad = nullptr;
	Super::Deinitialize();
}

URPGSaveSubsystem* URPGSaveSubsystem::Get(const UObject* WorldContextObject)
{
	const UWorld* World = GEngine ? GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::ReturnNull) : nullptr;
	const UGameInstance* GameInstance = World ? World->GetGameInstance() : nullptr;
	return GameInstance ? GameInstance->GetSubsystem<URPGSaveSubsystem>() : nullptr;
}

UWorld* URPGSaveSubsystem::GetGameWorld() const
{
	const UGameInstance* GameInstance = GetGameInstance();
	return GameInstance ? GameInstance->GetWorld() : nullptr;
}

FString URPGSaveSubsystem::GetMapName(const UWorld* World)
{
	return World ? UGameplayStatics::GetCurrentLevelName(World, true) : FString();
}

// ===================================================================== Serialization

TArray<uint8> URPGSaveSubsystem::SerializeObject(UObject* Object)
{
	TArray<uint8> Bytes;
	FMemoryWriter MemoryWriter(Bytes, true);

	// Object references are written as paths (item definitions, recipes) and loaded back by path.
	FObjectAndNameAsStringProxyArchive Archive(MemoryWriter, false);
	Archive.ArIsSaveGame = true;   // only UPROPERTY(SaveGame), including members of saved structs
	Archive.ArNoDelta = true;      // also write values equal to the defaults (an emptied inventory must stay empty)

	Object->Serialize(Archive);
	return Bytes;
}

void URPGSaveSubsystem::DeserializeObject(UObject* Object, const TArray<uint8>& Data)
{
	FMemoryReader MemoryReader(Data, true);

	FObjectAndNameAsStringProxyArchive Archive(MemoryReader, true);
	Archive.ArIsSaveGame = true;
	Archive.ArNoDelta = true;

	Object->Serialize(Archive);
}

// ===================================================================== Targets

void URPGSaveSubsystem::AddActorTargets(AActor* Actor, const FString& Prefix, TArray<FSaveTarget>& OutTargets)
{
	if (!IsValid(Actor))
	{
		return;
	}

	// The actor itself: Blueprint variables with "SaveGame" ticked.
	OutTargets.Add({ Prefix + TEXT("@Actor"), Actor });

	TInlineComponentArray<UActorComponent*> Components(Actor);
	for (UActorComponent* Component : Components)
	{
		if (Cast<IRPGSaveable>(Component))
		{
			OutTargets.Add({ Prefix + Component->GetName(), Component });
		}
	}
}

void URPGSaveSubsystem::CollectTargets(UWorld* World, TArray<FSaveTarget>& OutTargets) const
{
	APlayerController* PlayerController = World->GetFirstPlayerController();
	APawn* Pawn = PlayerController ? PlayerController->GetPawn() : nullptr;
	APlayerState* PlayerState = PlayerController ? PlayerController->PlayerState.Get() : nullptr;

	AddActorTargets(PlayerController, TEXT("Controller/"), OutTargets);
	AddActorTargets(Pawn, TEXT("Pawn/"), OutTargets);
	AddActorTargets(PlayerState, TEXT("PlayerState/"), OutTargets);

	for (TActorIterator<AActor> It(World); It; ++It)
	{
		AActor* Actor = *It;
		if (!IsValid(Actor) || Actor == PlayerController || Actor == Pawn || Actor == PlayerState)
		{
			continue;
		}

		const URPGSaveableActorComponent* Saveable = Actor->FindComponentByClass<URPGSaveableActorComponent>();
		if (Saveable && Saveable->IsPlacedInLevel())
		{
			AddActorTargets(Actor, TEXT("World/") + Saveable->GetSaveKey() + TEXT("/"), OutTargets);
		}
	}
}

// ===================================================================== Save

bool URPGSaveSubsystem::SaveToSlot(const FString& SlotName)
{
	UWorld* World = GetGameWorld();
	if (!World || SlotName.IsEmpty())
	{
		UE_LOG(LogRPGSave, Warning, TEXT("SaveToSlot: no world or empty slot name."));
		OnSaved.Broadcast(SlotName, false);
		return false;
	}

	if (IsLoading())
	{
		UE_LOG(LogRPGSave, Warning, TEXT("SaveToSlot: a load is in progress."));
		OnSaved.Broadcast(SlotName, false);
		return false;
	}

	SyncTrackedMap(World);

	URPGSaveGame* Save = Cast<URPGSaveGame>(UGameplayStatics::CreateSaveGameObject(URPGSaveGame::StaticClass()));
	Save->SaveVersion = URPGSaveGame::CurrentVersion;
	Save->Timestamp = FDateTime::Now();
	Save->MapName = GetMapName(World);

	if (const APlayerController* PlayerController = World->GetFirstPlayerController())
	{
		Save->ControlRotation = PlayerController->GetControlRotation();
		if (const APawn* Pawn = PlayerController->GetPawn())
		{
			Save->bHasPlayerTransform = true;
			Save->PlayerTransform = Pawn->GetActorTransform();
		}
		else
		{
			// Not refused (a death screen may autosave), but the pawn's inventory, equipment and crafting are missing.
			UE_LOG(LogRPGSave, Warning, TEXT("SaveToSlot '%s': the player has no pawn; its components are not saved."), *SlotName);
		}
	}

	TArray<FSaveTarget> Targets;
	CollectTargets(World, Targets);

	for (const FSaveTarget& Target : Targets)
	{
		if (IRPGSaveable* Saveable = Cast<IRPGSaveable>(Target.Object))
		{
			Saveable->OnBeforeSave();
		}
	}

	for (const FSaveTarget& Target : Targets)
	{
		FRPGSavedObject& Saved = Save->Objects.AddDefaulted_GetRef();
		Saved.Key = Target.Key;
		Saved.Data = SerializeObject(Target.Object);
	}

	for (TActorIterator<AActor> It(World); It; ++It)
	{
		const URPGSaveableActorComponent* Saveable = It->FindComponentByClass<URPGSaveableActorComponent>();
		if (Saveable && Saveable->bSaveTransform && Saveable->IsPlacedInLevel())
		{
			Save->ActorTransforms.Add(Saveable->GetSaveKey(), It->GetActorTransform());
		}
	}

	Save->DestroyedActors = DestroyedActors.Array();

	const bool bSuccess = UGameplayStatics::SaveGameToSlot(Save, SlotName, UserIndex);
	UE_LOG(LogRPGSave, Log, TEXT("Save '%s': %s (%d objects, %d destroyed actors, map %s)."),
		*SlotName, bSuccess ? TEXT("OK") : TEXT("FAILED"), Save->Objects.Num(), Save->DestroyedActors.Num(), *Save->MapName);

	OnSaved.Broadcast(SlotName, bSuccess);
	return bSuccess;
}

// ===================================================================== Load

bool URPGSaveSubsystem::LoadFromSlot(const FString& SlotName)
{
	auto Fail = [this, &SlotName](const TCHAR* Reason)
	{
		UE_LOG(LogRPGSave, Warning, TEXT("LoadFromSlot '%s': %s"), *SlotName, Reason);
		OnLoaded.Broadcast(SlotName, false);
		return false;
	};

	if (!UGameplayStatics::DoesSaveGameExist(SlotName, UserIndex))
	{
		return Fail(TEXT("no such save."));
	}

	URPGSaveGame* Save = Cast<URPGSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName, UserIndex));
	if (!Save)
	{
		return Fail(TEXT("the file could not be read (or is not an RPG save)."));
	}

	if (Save->SaveVersion > URPGSaveGame::CurrentVersion)
	{
		return Fail(TEXT("the save was made by a newer version."));
	}

	if (Save->MapName.IsEmpty())
	{
		return Fail(TEXT("the save has no map."));
	}

	// Older versions would be migrated here (Save->SaveVersion < CurrentVersion).

	PendingLoad = Save;
	PendingSlot = SlotName;
	ApplyAttempts = 0;

	UE_LOG(LogRPGSave, Log, TEXT("Load '%s': opening %s..."), *SlotName, *Save->MapName);

	// A fresh map is the only clean way to undo everything that happened after the save.
	UGameplayStatics::OpenLevel(GetGameWorld(), FName(*Save->MapName));
	return true;
}

void URPGSaveSubsystem::HandleTravelFailure(UWorld* World, ETravelFailure::Type FailureType, const FString& ErrorString)
{
	if (PendingLoad)
	{
		UE_LOG(LogRPGSave, Warning, TEXT("Load '%s' failed: %s"), *PendingSlot, *ErrorString);
		PendingLoad = nullptr;
		OnLoaded.Broadcast(PendingSlot, false);
	}
}

void URPGSaveSubsystem::HandlePostLoadMap(UWorld* LoadedWorld)
{
	if (!LoadedWorld || LoadedWorld->GetGameInstance() != GetGameInstance())
	{
		return;   // another PIE instance
	}

	if (PendingLoad)
	{
		// The player pawn and every BeginPlay must exist before we overwrite their state.
		LoadedWorld->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &ThisClass::ApplyPendingLoad));
	}
	else
	{
		SyncTrackedMap(LoadedWorld);
	}
}

void URPGSaveSubsystem::ApplyPendingLoad()
{
	URPGSaveGame* Save = PendingLoad;
	UWorld* World = GetGameWorld();
	if (!Save || !World)
	{
		PendingLoad = nullptr;
		return;
	}

	APlayerController* PlayerController = World->GetFirstPlayerController();
	if ((!PlayerController || !PlayerController->GetPawn()) && ApplyAttempts++ < 60)
	{
		World->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &ThisClass::ApplyPendingLoad));
		return;
	}

	// State first.
	PendingLoad = nullptr;
	TrackedMap = GetMapName(World);
	DestroyedActors = TSet<FString>(Save->DestroyedActors);

	// 1. Placed actors: destroy the ones that were gone, move the ones that were moved.
	TArray<AActor*> ToDestroy;
	for (TActorIterator<AActor> It(World); It; ++It)
	{
		const URPGSaveableActorComponent* Saveable = It->FindComponentByClass<URPGSaveableActorComponent>();
		if (!Saveable || !Saveable->IsPlacedInLevel())
		{
			continue;
		}

		const FString Key = Saveable->GetSaveKey();
		if (DestroyedActors.Contains(Key))
		{
			ToDestroy.Add(*It);
		}
		else if (Saveable->bSaveTransform)
		{
			if (const FTransform* Transform = Save->ActorTransforms.Find(Key))
			{
				It->SetActorTransform(*Transform, false, nullptr, ETeleportType::TeleportPhysics);
			}
		}
	}

	for (AActor* Actor : ToDestroy)
	{
		Actor->Destroy();
	}

	// 2. Player position.
	if (PlayerController)
	{
		if (APawn* Pawn = PlayerController->GetPawn(); Pawn && Save->bHasPlayerTransform)
		{
			Pawn->SetActorLocationAndRotation(Save->PlayerTransform.GetLocation(), Save->PlayerTransform.GetRotation(),
				false, nullptr, ETeleportType::TeleportPhysics);
		}
		PlayerController->SetControlRotation(Save->ControlRotation);
	}

	// 3. Restore every object first...
	TMap<FString, const TArray<uint8>*> DataByKey;
	for (const FRPGSavedObject& Saved : Save->Objects)
	{
		DataByKey.Add(Saved.Key, &Saved.Data);
	}

	TArray<FSaveTarget> Targets;
	CollectTargets(World, Targets);

	int32 Restored = 0;
	for (const FSaveTarget& Target : Targets)
	{
		if (const TArray<uint8>* const* Data = DataByKey.Find(Target.Key))
		{
			DeserializeObject(Target.Object, **Data);
			++Restored;
		}
	}

	// 4. ...then notify, so cross-component refreshes see the final data.
	for (const FSaveTarget& Target : Targets)
	{
		if (IRPGSaveable* Saveable = Cast<IRPGSaveable>(Target.Object))
		{
			Saveable->OnLoadedFromSave();
		}
	}

	for (TActorIterator<AActor> It(World); It; ++It)
	{
		if (URPGSaveableActorComponent* Saveable = It->FindComponentByClass<URPGSaveableActorComponent>())
		{
			if (Saveable->IsPlacedInLevel())
			{
				Saveable->NotifyLoaded();
			}
		}
	}

	UE_LOG(LogRPGSave, Log, TEXT("Load '%s': restored %d of %d saved objects, destroyed %d actors."),
		*PendingSlot, Restored, Save->Objects.Num(), ToDestroy.Num());

	OnLoaded.Broadcast(PendingSlot, true);
}

// ===================================================================== Slots

bool URPGSaveSubsystem::DoesSaveExist(const FString& SlotName) const
{
	return UGameplayStatics::DoesSaveGameExist(SlotName, UserIndex);
}

bool URPGSaveSubsystem::DeleteSave(const FString& SlotName)
{
	return UGameplayStatics::DeleteGameInSlot(SlotName, UserIndex);
}

bool URPGSaveSubsystem::GetSaveInfo(const FString& SlotName, FRPGSaveInfo& OutInfo) const
{
	const URPGSaveGame* Save = DoesSaveExist(SlotName)
		? Cast<URPGSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName, UserIndex))
		: nullptr;

	if (!Save)
	{
		return false;
	}

	OutInfo.SlotName = SlotName;
	OutInfo.MapName = Save->MapName;
	OutInfo.Timestamp = Save->Timestamp;
	OutInfo.SaveVersion = Save->SaveVersion;
	return true;
}

// ===================================================================== Destroyed actors

void URPGSaveSubsystem::SyncTrackedMap(const UWorld* World)
{
	const FString MapName = GetMapName(World);
	if (MapName != TrackedMap)
	{
		// Different map without a load (new game, travel): old destroyed keys do not apply.
		DestroyedActors.Reset();
		TrackedMap = MapName;
	}
}

void URPGSaveSubsystem::RegisterDestroyedActor(const UWorld* World, const FString& Key)
{
	if (Key.IsEmpty() || IsLoading())
	{
		return;   // during a load the old world is being torn down
	}

	SyncTrackedMap(World);
	DestroyedActors.Add(Key);
}

void URPGSaveSubsystem::ResetWorldState()
{
	DestroyedActors.Reset();
	TrackedMap.Reset();
}
