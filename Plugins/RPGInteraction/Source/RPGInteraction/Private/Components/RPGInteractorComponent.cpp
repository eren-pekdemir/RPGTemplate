// RPGInteractorComponent.cpp

#include "Components/RPGInteractorComponent.h"

#include "Engine/World.h"
#include "GameFramework/Controller.h"
#include "GameFramework/Pawn.h"
#include "Interfaces/RPGInteractable.h"
#include "RPGInteractionDebug.h"
#include "TimerManager.h"

#if ENABLE_DRAW_DEBUG
#include "Engine/Engine.h"
#endif

URPGInteractorComponent::URPGInteractorComponent()
{
	// Tick is only switched on while a hold-to-interact is in progress.
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void URPGInteractorComponent::BeginPlay()
{
	Super::BeginPlay();

	GetWorld()->GetTimerManager().SetTimer(ScanTimerHandle, this, &ThisClass::ScanForInteractables, ScanInterval, true);
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

void URPGInteractorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!IsHolding())
	{
		return;
	}

	UObject* Target = HoldTarget.Get();
	if (!Target)
	{
		CancelHold();
		return;
	}

	if (!IRPGInteractable::Execute_CanInteract(Target, GetOwner()))
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
	if (!Pawn || !Pawn->IsLocallyControlled() || !Pawn->GetController())
	{
		return;
	}

	if (!Detector)
	{
		return;
	}

	FRPGInteractionQuery Query;
	Query.InstigatorActor = Pawn;
	Query.QueryInterval = ScanInterval; // lets the detector keep its debug shapes until the next scan
	Pawn->GetController()->GetPlayerViewPoint(Query.ViewLocation, Query.ViewRotation);

	UObject* NewFocus = Detector->FindBestInteractable(Query);
	Query.CurrentFocus = FocusedInteractable.Get();	

	if (NewFocus != FocusedInteractable.Get())
	{
		SetFocus(NewFocus);
	}

#if ENABLE_DRAW_DEBUG
	if (RPGInteractionDebug::IsEnabled())
	{
		DrawDebugState();
	}
#endif
}

void URPGInteractorComponent::SetFocus(UObject* NewFocus)
{
	if (IsHolding() && NewFocus != HoldTarget.Get())
	{
		CancelHold();
	}

	UObject* OldFocus = FocusedInteractable.Get();
	FocusedInteractable = NewFocus;
	OnFocusChanged.Broadcast(NewFocus, OldFocus);
}

void URPGInteractorComponent::StartInteraction()
{
	if (IsHolding())
	{
		return;
	}

	UObject* Focus = FocusedInteractable.Get();
	if (!Focus)
	{
		return;
	}

	if (!IRPGInteractable::Execute_CanInteract(Focus, GetOwner()))
	{
		return;
	}

	const float Duration = IRPGInteractable::Execute_GetInteractionDuration(Focus);

	if (Duration <= 0.f)
	{
		IRPGInteractable::Execute_Interact(Focus, GetOwner());
	}
	else
	{
		StartHold(Focus, Duration);
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
	if (!IsHolding() || RequiredHoldDuration <= 0.f)
	{
		return 0.f;
	}

	return FMath::Clamp(ElapsedHoldTime / RequiredHoldDuration, 0.f, 1.f);
}

bool URPGInteractorComponent::IsHolding() const
{
	return bIsHolding;
}

UObject* URPGInteractorComponent::GetFocusedInteractable() const
{
	return FocusedInteractable.Get();
}

void URPGInteractorComponent::StartHold(UObject* Target, float Duration)
{
	HoldTarget = Target;
	RequiredHoldDuration = Duration;
	ElapsedHoldTime = 0.f;
	bIsHolding = true;
	SetComponentTickEnabled(true);
	OnHoldStarted.Broadcast(Target, Duration);
}

void URPGInteractorComponent::CompleteHold()
{
	UObject* Target = HoldTarget.Get();
	if (!Target || !IRPGInteractable::Execute_CanInteract(Target, GetOwner()))
	{
		CancelHold();
		return;
	}

	IRPGInteractable::Execute_Interact(Target, GetOwner());
	EndHold(true);
}

void URPGInteractorComponent::CancelHold()
{
	EndHold(false);
}

void URPGInteractorComponent::EndHold(bool bCompleted)
{
	if (!IsHolding())
	{
		return;
	}

	// Keep the target locally: we clear the state BEFORE broadcasting (re-entrancy safety).
	UObject* EndedTarget = HoldTarget.Get();

	SetComponentTickEnabled(false);
	bIsHolding = false;
	HoldTarget.Reset();
	ElapsedHoldTime = 0.f;
	RequiredHoldDuration = 0.f;

	OnHoldEnded.Broadcast(EndedTarget, bCompleted);
}

void URPGInteractorComponent::DrawDebugState() const
{
	// Declared unconditionally in the header (Unreal's header tool is picky about custom #if blocks
	// inside a UCLASS body); the body compiles to nothing in builds without debug drawing.
#if ENABLE_DRAW_DEBUG
	if (!GEngine)
	{
		return;
	}

	const UObject* Focus = FocusedInteractable.Get();
	FString Text = FString::Printf(TEXT("[Interaction] %s | Focus: %s"),
		*GetNameSafe(GetOwner()), *GetNameSafe(Focus));

	if (IsHolding())
	{
		Text += FString::Printf(TEXT(" | Hold: %.0f%% (%.2f / %.2f s)"),
			GetHoldProgress() * 100.f, ElapsedHoldTime, RequiredHoldDuration);
	}

	// A fixed key per component: every new message REPLACES the previous line instead of adding a new one.
	const uint64 MessageKey = static_cast<uint64>(GetUniqueID());
	const FColor Color = Focus ? FColor::Green : FColor::White;

	GEngine->AddOnScreenDebugMessage(MessageKey, RPGInteractionDebug::GetDrawLifetime(ScanInterval), Color, Text);
#endif
}
