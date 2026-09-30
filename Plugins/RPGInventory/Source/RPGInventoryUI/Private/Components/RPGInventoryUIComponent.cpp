// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/RPGInventoryUIComponent.h"
#include "Components/RPGInventoryComponent.h"
#include "Widgets/RPGInventoryScreenWidget.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"


URPGInventoryUIComponent::URPGInventoryUIComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void URPGInventoryUIComponent::BeginPlay()
{
	Super::BeginPlay();

	PC = Cast<APlayerController>(GetOwner());
	if (!PC || !PC->IsLocalController()) return;
	
	if (!ScreenWidgetClass)
	{
		UE_LOG(LogTemp,Warning, TEXT("Screen Widget Class is null"));
		return;
	}
	
	ScreenWidget = CreateWidget<URPGInventoryScreenWidget>(PC, ScreenWidgetClass);
	ScreenWidget->AddToViewport();
	ScreenWidget->SetVisibility(ESlateVisibility::Collapsed);
	
	PC->OnPossessedPawnChanged.AddDynamic(this, &ThisClass::HandlePossessedPawnChanged);
	
	HandlePossessedPawnChanged(nullptr, PC->GetPawn());
}

void URPGInventoryUIComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (PC)
	{
		PC->OnPossessedPawnChanged.RemoveDynamic(this, &ThisClass::HandlePossessedPawnChanged);
	}
	if (ScreenWidget)
	{
		CloseInventory();
		ScreenWidget->RemoveFromParent();  
	}
	Super::EndPlay(EndPlayReason);
}

void URPGInventoryUIComponent::HandlePossessedPawnChanged(APawn* OldPawn, APawn* NewPawn)
{
	URPGInventoryComponent* NewInventory = NewPawn ? NewPawn->FindComponentByClass<URPGInventoryComponent>() : nullptr;

	if (ScreenWidget)
	{
		ScreenWidget->SetInventory(NewInventory);
	}

	if (!NewInventory)
	{
		CloseInventory();
	}
}

void URPGInventoryUIComponent::ToggleInventory()
{
	bIsOpen ? CloseInventory() : OpenInventory();
}

void URPGInventoryUIComponent::OpenInventory()
{
	if (bIsOpen || !ScreenWidget) return;
	ScreenWidget->SetVisibility(ESlateVisibility::Visible);
	PC->SetShowMouseCursor(true);
	PC->SetInputMode(FInputModeGameAndUI());
	PC->SetIgnoreLookInput(true);
	PC->SetIgnoreMoveInput(true);
	bIsOpen = true;
	ScreenWidget->BP_OnOpened();
}

void URPGInventoryUIComponent::CloseInventory()
{
	if (!bIsOpen || !ScreenWidget) return;
	ScreenWidget->SetVisibility(ESlateVisibility::Collapsed);
	PC->SetShowMouseCursor(false);
	PC->SetInputMode(FInputModeGameOnly());
	PC->SetIgnoreLookInput(false);
	PC->SetIgnoreMoveInput(false);
	bIsOpen = false;
	ScreenWidget->BP_OnClosed();
}

bool URPGInventoryUIComponent::IsInventoryOpen() const
{
	return bIsOpen;
}
