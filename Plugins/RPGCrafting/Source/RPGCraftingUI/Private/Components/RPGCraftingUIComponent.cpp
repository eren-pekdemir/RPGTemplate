// RPGCraftingUIComponent.cpp

#include "Components/RPGCraftingUIComponent.h"

#include "Blueprint/UserWidget.h"
#include "Components/RPGCraftingComponent.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "Widgets/RPGCraftingScreenWidget.h"

URPGCraftingUIComponent::URPGCraftingUIComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

APlayerController* URPGCraftingUIComponent::GetPlayerController() const
{
	return Cast<APlayerController>(GetOwner());
}

void URPGCraftingUIComponent::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PlayerController = GetPlayerController())
	{
		PlayerController->OnPossessedPawnChanged.AddUniqueDynamic(this, &ThisClass::HandlePossessedPawnChanged);
		BindToPawn(PlayerController->GetPawn());   // sync on subscribe
	}
}

void URPGCraftingUIComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (APlayerController* PlayerController = GetPlayerController())
	{
		PlayerController->OnPossessedPawnChanged.RemoveDynamic(this, &ThisClass::HandlePossessedPawnChanged);
	}

	BindToPawn(nullptr);
	Super::EndPlay(EndPlayReason);
}

void URPGCraftingUIComponent::HandlePossessedPawnChanged(APawn* OldPawn, APawn* NewPawn)
{
	BindToPawn(NewPawn);
}

void URPGCraftingUIComponent::BindToPawn(APawn* Pawn)
{
	// Unbind -> assign -> bind
	if (Crafting)
	{
		Crafting->OnActiveStationChanged.RemoveDynamic(this, &ThisClass::HandleActiveStationChanged);
	}

	Crafting = Pawn ? Pawn->FindComponentByClass<URPGCraftingComponent>() : nullptr;

	if (Crafting)
	{
		Crafting->OnActiveStationChanged.AddDynamic(this, &ThisClass::HandleActiveStationChanged);
	}
	else if (bIsOpen)
	{
		CloseCrafting();   // new pawn cannot craft
	}

	if (bIsOpen && ScreenWidget)
	{
		ScreenWidget->SetCrafting(Crafting);
	}
}

void URPGCraftingUIComponent::HandleActiveStationChanged(FGameplayTag Station)
{
	// Using a station opens the screen; leaving it closes the screen.
	if (Station.IsValid())
	{
		OpenCrafting();
	}
	else
	{
		CloseCrafting();
	}
}

void URPGCraftingUIComponent::OpenCrafting()
{
	if (bIsOpen)
	{
		return;
	}

	APlayerController* PlayerController = GetPlayerController();
	if (!PlayerController || !PlayerController->IsLocalController())
	{
		UE_LOG(LogTemp, Warning, TEXT("CraftingUI: owner is not a local PlayerController (%s)."), *GetNameSafe(GetOwner()));
		return;
	}

	if (!ScreenWidgetClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("CraftingUI: ScreenWidgetClass is not set on %s."), *GetNameSafe(GetOwner()));
		return;
	}

	// Possession may have happened before we subscribed: resolve now.
	if (!Crafting)
	{
		BindToPawn(PlayerController->GetPawn());
	}

	if (!Crafting)
	{
		UE_LOG(LogTemp, Warning, TEXT("CraftingUI: pawn %s has no RPGCraftingComponent."), *GetNameSafe(PlayerController->GetPawn()));
		return;
	}

	if (!ScreenWidget)
	{
		ScreenWidget = CreateWidget<URPGCraftingScreenWidget>(PlayerController, ScreenWidgetClass);
	}

	bIsOpen = true;

	ScreenWidget->SetCrafting(Crafting);
	ScreenWidget->AddToViewport(ZOrder);

	FInputModeGameAndUI InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputMode.SetHideCursorDuringCapture(false);
	PlayerController->SetInputMode(InputMode);
	PlayerController->SetShowMouseCursor(true);
	PlayerController->SetIgnoreLookInput(true);
	PlayerController->SetIgnoreMoveInput(true);

	OnCraftingToggled.Broadcast(true);
}

void URPGCraftingUIComponent::CloseCrafting()
{
	if (!bIsOpen)
	{
		return;
	}

	// State first: clearing the station below fires HandleActiveStationChanged, which must find us closed.
	bIsOpen = false;

	if (ScreenWidget)
	{
		ScreenWidget->RemoveFromParent();
	}

	if (APlayerController* PlayerController = GetPlayerController())
	{
		PlayerController->SetInputMode(FInputModeGameOnly());
		PlayerController->SetShowMouseCursor(false);
		PlayerController->ResetIgnoreLookInput();
		PlayerController->ResetIgnoreMoveInput();
	}

	// Leaving the screen = leaving the station.
	if (Crafting && Crafting->GetActiveStation().IsValid())
	{
		Crafting->ClearActiveStation();
	}

	OnCraftingToggled.Broadcast(false);
}

void URPGCraftingUIComponent::ToggleCrafting()
{
	if (bIsOpen)
	{
		CloseCrafting();
	}
	else
	{
		OpenCrafting();
	}
}
