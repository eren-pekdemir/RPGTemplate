// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/RPGInteractorComponent.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Controller.h"
#include "TimerManager.h"
#include "Interfaces/RPGInteractable.h"

URPGInteractorComponent::URPGInteractorComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void URPGInteractorComponent::BeginPlay()
{
	Super::BeginPlay();

	GetWorld()->GetTimerManager().SetTimer(ScanTimerHandle,this, &ThisClass::ScanForInteractables, ScanInterval, true);
}

void URPGInteractorComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (IsHolding())
	{
		CancelHold();
	}
	
	GetWorld()->GetTimerManager().ClearTimer(ScanTimerHandle);
	Super::EndPlay(EndPlayReason);
}

void URPGInteractorComponent::TickComponent(float DeltaTime, ELevelTick TickType,FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (!IsHolding()) return;
	UObject* Target = HoldTarget.Get();
	if (!Target)
	{
		CancelHold();
		return;
	}
	
	if (!IRPGInteractable::Execute_CanInteract(Target,GetOwner()))
	{
		CancelHold();
		return;
	}
	
	ElapsedHoldTime += DeltaTime;
	if (ElapsedHoldTime >= RequiredHoldDuration)
	{
		CompleteHold();
	}
	
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
	if (IsHolding() && NewFocus != HoldTarget.Get())
	{
		CancelHold();
	}
	UObject* OldFocus= FocusedInteractable.Get();
	FocusedInteractable = NewFocus;
	OnFocusChanged.Broadcast(NewFocus, OldFocus);
}

void URPGInteractorComponent::StartInteraction()
{
	if (IsHolding()) return;
	UObject* Focus = FocusedInteractable.Get();
	if (!Focus) return;
	
	if(!IRPGInteractable::Execute_CanInteract(Focus,GetOwner())) return;
	
	float Duration = IRPGInteractable::Execute_GetInteractionDuration(Focus);
	
	if (Duration <= 0.0f)
	{
		IRPGInteractable::Execute_Interact(Focus,GetOwner());
	}
	else
	{
		StartHold(Focus,Duration);
	}
	
}

void URPGInteractorComponent::StopInteraction()
{
	if (IsHolding())
	{
		CancelHold();
	}
}

float URPGInteractorComponent::GetHoldProgress() const
{
	if (!IsHolding() || RequiredHoldDuration <= 0.0f) return 0.0f;
	
	return FMath::Clamp(ElapsedHoldTime / RequiredHoldDuration , 0.f , 1.f);
}

bool URPGInteractorComponent::IsHolding() const
{
	return bIsHolding;
}

void URPGInteractorComponent::StartHold(UObject* Target, float Duration)
{
	HoldTarget = Target;
	RequiredHoldDuration = Duration;
	ElapsedHoldTime = 0.0f;
	bIsHolding = true;
	SetComponentTickEnabled(true);
	OnHoldStarted.Broadcast(Target, Duration);
	
}

void URPGInteractorComponent::CompleteHold()
{
	UObject* Target = HoldTarget.Get();
	if (Target && IRPGInteractable::Execute_CanInteract(Target,GetOwner()))
	{
		IRPGInteractable::Execute_Interact(Target,GetOwner());
	}
	else
	{
		CancelHold();
		return;
	}
	
	EndHold(true);
}

void URPGInteractorComponent::CancelHold()
{
	EndHold(false);
}

void URPGInteractorComponent::EndHold(bool bCompleted)
{
	if (!IsHolding()) return;
	UObject* EndedTarget = HoldTarget.Get();
	SetComponentTickEnabled(false);
	bIsHolding = false;
	HoldTarget.Reset();
	ElapsedHoldTime = 0.0f;
	RequiredHoldDuration = 0.0f;
	OnHoldEnded.Broadcast(EndedTarget, bCompleted);
}

