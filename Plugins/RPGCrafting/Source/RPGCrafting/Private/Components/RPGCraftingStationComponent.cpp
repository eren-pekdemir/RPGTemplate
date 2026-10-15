// RPGCraftingStationComponent.cpp

#include "Components/RPGCraftingStationComponent.h"

#include "Components/RPGCraftingComponent.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "RPGCrafting.h"
#include "TimerManager.h"

URPGCraftingStationComponent::URPGCraftingStationComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

bool URPGCraftingStationComponent::BeginUse(AActor* User)
{
	URPGCraftingComponent* Crafting = User ? User->FindComponentByClass<URPGCraftingComponent>() : nullptr;
	if (!Crafting || !StationTag.IsValid())
	{
		UE_LOG(LogRPGCrafting, Warning, TEXT("%s: cannot be used by %s (no crafting component or no StationTag)."),
			*GetNameSafe(GetOwner()), *GetNameSafe(User));
		return false;
	}

	// One user at a time.
	if (IsInUse() && CurrentUser.Get() != User)
	{
		EndUse();
	}

	CurrentUser = User;
	UserCrafting = Crafting;

	// Source = this: lets us tell our forge apart from another forge with the same tag.
	Crafting->SetActiveStation(StationTag, this);

	GetWorld()->GetTimerManager().SetTimer(CheckTimer, this, &ThisClass::CheckUser, 0.25f, true);
	OnUseStarted.Broadcast(User);
	return true;
}

void URPGCraftingStationComponent::EndUse()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(CheckTimer);
	}

	AActor* OldUser = CurrentUser.Get();
	URPGCraftingComponent* Crafting = UserCrafting.Get();

	// State first.
	CurrentUser.Reset();
	UserCrafting.Reset();

	// Only clear the station if the user is still at THIS one.
	if (Crafting && Crafting->GetActiveStationSource() == this)
	{
		Crafting->ClearActiveStation();
	}

	if (OldUser)
	{
		OnUseEnded.Broadcast(OldUser);
	}
}

void URPGCraftingStationComponent::CheckUser()
{
	const AActor* User = CurrentUser.Get();
	const URPGCraftingComponent* Crafting = UserCrafting.Get();

	// Gone, closed the screen, or moved to another station.
	if (!User || !Crafting || Crafting->GetActiveStationSource() != this)
	{
		EndUse();
		return;
	}

	if (MaxUseDistance > 0.f &&
		FVector::DistSquared(User->GetActorLocation(), GetOwner()->GetActorLocation()) > FMath::Square(MaxUseDistance))
	{
		EndUse();
	}
}

void URPGCraftingStationComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	EndUse();
	Super::EndPlay(EndPlayReason);
}
