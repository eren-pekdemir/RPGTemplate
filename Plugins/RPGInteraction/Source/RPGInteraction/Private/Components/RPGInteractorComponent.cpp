// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/RPGInteractorComponent.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Controller.h"
#include "TimerManager.h"
#include "Interfaces/RPGInteractable.h"


// Sets default values for this component's properties
URPGInteractorComponent::URPGInteractorComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

// Called when the game starts
void URPGInteractorComponent::BeginPlay()
{
	Super::BeginPlay();

	GetWorld()->GetTimerManager().SetTimer(ScanTimerHandle,this, &ThisClass::ScanForInteractables, ScanInterval, true);
	
}

void URPGInteractorComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorld()->GetTimerManager().ClearTimer(ScanTimerHandle);
	Super::EndPlay(EndPlayReason);
}

void URPGInteractorComponent::ScanForInteractables()
{
	APawn* Pawn = Cast<APawn>(GetOwner());
	if (!Pawn || !Pawn->IsLocallyControlled()|| !Pawn->GetController()) return;
	
	if (!Detector) return;
	
	FRPGInteractionQuery Query;
	Query.InstigatorActor = Pawn;
	Pawn->GetController()->GetPlayerViewPoint(Query.ViewLocation, Query.ViewRotation);
	
	UObject* NewFocus = Detector->FindBestInteractable(Query);
	
	if ( NewFocus != FocusedInteractable.Get())
	{
		SetFocus(NewFocus);
	}
}

void URPGInteractorComponent::SetFocus(UObject* NewFocus)
{
	UObject* OldFocus= FocusedInteractable.Get();
	FocusedInteractable = NewFocus;
	OnFocusChanged.Broadcast(NewFocus, OldFocus);
}

void URPGInteractorComponent::StartInteraction()
{
	UObject* Focus = FocusedInteractable.Get();
	if (!Focus) return;
	
	if(!IRPGInteractable::Execute_CanInteract(Focus,GetOwner())) return;
	
	IRPGInteractable::Execute_Interact(Focus,GetOwner());
		
}

void URPGInteractorComponent::StopInteraction()
{
}
