// RPGDialogueUIComponent.cpp

#include "Components/RPGDialogueUIComponent.h"

#include "Components/RPGDialogueComponent.h"
#include "GameFramework/PlayerController.h"
#include "Widgets/RPGDialogueScreenWidget.h"

URPGDialogueUIComponent::URPGDialogueUIComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

APlayerController* URPGDialogueUIComponent::GetPlayerController() const
{
	return Cast<APlayerController>(GetOwner());
}

void URPGDialogueUIComponent::BeginPlay()
{
	Super::BeginPlay();

	Dialogue = GetOwner() ? GetOwner()->FindComponentByClass<URPGDialogueComponent>() : nullptr;
	if (!Dialogue)
	{
		UE_LOG(LogTemp, Warning, TEXT("DialogueUI: %s has no RPGDialogueComponent. Put both on the PlayerController."), *GetNameSafe(GetOwner()));
		return;
	}

	Dialogue->OnDialogueStarted.AddDynamic(this, &ThisClass::HandleDialogueStarted);
	Dialogue->OnDialogueEnded.AddDynamic(this, &ThisClass::HandleDialogueEnded);

	// Sync on subscribe.
	if (Dialogue->IsInDialogue())
	{
		OpenScreen();
	}
}

void URPGDialogueUIComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (Dialogue)
	{
		Dialogue->OnDialogueStarted.RemoveDynamic(this, &ThisClass::HandleDialogueStarted);
		Dialogue->OnDialogueEnded.RemoveDynamic(this, &ThisClass::HandleDialogueEnded);
	}

	CloseScreen();
	Super::EndPlay(EndPlayReason);
}

void URPGDialogueUIComponent::HandleDialogueStarted(AActor* Speaker)
{
	OpenScreen();
}

void URPGDialogueUIComponent::HandleDialogueEnded(AActor* Speaker)
{
	CloseScreen();
}

void URPGDialogueUIComponent::OpenScreen()
{
	if (bIsOpen)
	{
		return;
	}

	APlayerController* PlayerController = GetPlayerController();
	if (!PlayerController || !PlayerController->IsLocalController())
	{
		UE_LOG(LogTemp, Warning, TEXT("DialogueUI: owner is not a local PlayerController (%s)."), *GetNameSafe(GetOwner()));
		return;
	}

	if (!ScreenWidgetClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("DialogueUI: ScreenWidgetClass is not set."));
		return;
	}

	if (!ScreenWidget)
	{
		ScreenWidget = CreateWidget<URPGDialogueScreenWidget>(PlayerController, ScreenWidgetClass);
	}

	bIsOpen = true;

	ScreenWidget->AddToViewport(ZOrder);
	ScreenWidget->SetDialogue(Dialogue);

	FInputModeGameAndUI InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputMode.SetHideCursorDuringCapture(false);
	InputMode.SetWidgetToFocus(ScreenWidget->TakeWidget());   // number keys / Space / Escape
	PlayerController->SetInputMode(InputMode);
	PlayerController->SetShowMouseCursor(true);
	PlayerController->SetIgnoreLookInput(true);
	PlayerController->SetIgnoreMoveInput(true);

	OnDialogueToggled.Broadcast(true);
}

void URPGDialogueUIComponent::CloseScreen()
{
	if (!bIsOpen)
	{
		return;
	}

	// State first.
	bIsOpen = false;

	if (ScreenWidget)
	{
		ScreenWidget->SetDialogue(nullptr);
		ScreenWidget->RemoveFromParent();
	}

	if (APlayerController* PlayerController = GetPlayerController())
	{
		// The ignore flags are counters shared by every screen (inventory, crafting...): undo only our own +1,
		// and give control back to the game only when no other screen is still open.
		PlayerController->SetIgnoreLookInput(false);
		PlayerController->SetIgnoreMoveInput(false);
		if (!PlayerController->IsLookInputIgnored())
		{
			PlayerController->SetInputMode(FInputModeGameOnly());
			PlayerController->SetShowMouseCursor(false);
		}
	}

	OnDialogueToggled.Broadcast(false);
}
