// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/RPGInteractionPromptComponent.h"

#include "Blueprint/UserWidget.h"
#include "Widgets/RPGInteractionPromptWidget.h"
#include "Components/RPGInteractorComponent.h"

URPGInteractionPromptComponent::URPGInteractionPromptComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void URPGInteractionPromptComponent::BeginPlay()
{
	Super::BeginPlay();
	
	APlayerController* PC = Cast<APlayerController>(GetOwner());
	if (!PC)
	{
		UE_LOG(LogTemp,Warning, TEXT("must be added to a PlayerController"));
		return;
	}
	if (!PC->IsLocalController()) return;
	
	if (!PromptWidgetClass)
	{
		UE_LOG(LogTemp,Warning, TEXT("add PromptWidgetClass"));
		return;
	}
	
	PromptWidget = CreateWidget<URPGInteractionPromptWidget>(PC, PromptWidgetClass);
	if (PromptWidget)
	{
		PromptWidget->AddToViewport(ZOrder);
	}
	
	PC->OnPossessedPawnChanged.AddDynamic(this , &ThisClass::HandlePossessedPawnChanged);
	
	HandlePossessedPawnChanged(nullptr, PC->GetPawn());
	
}

void URPGInteractionPromptComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	APlayerController* PC = Cast<APlayerController>(GetOwner());
	if (PC)
	{
		PC->OnPossessedPawnChanged.RemoveDynamic(this, &ThisClass::HandlePossessedPawnChanged);
	}
	
	if (PromptWidget)
	{
		PromptWidget->RemoveFromParent();
		PromptWidget = nullptr;
	}
	
	Super::EndPlay(EndPlayReason);
}


void URPGInteractionPromptComponent::HandlePossessedPawnChanged(APawn* OldPawn, APawn* NewPawn)
{
	if (!PromptWidget) return;
	
	URPGInteractorComponent* NewInteractor = NewPawn ? NewPawn->FindComponentByClass<URPGInteractorComponent>() : nullptr;
	PromptWidget->SetInteractor(NewInteractor);
}

