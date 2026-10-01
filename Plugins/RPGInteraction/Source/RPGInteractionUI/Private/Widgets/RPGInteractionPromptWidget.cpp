// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/RPGInteractionPromptWidget.h"

#include "RPGInteractionLibrary.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/ProgressBar.h"
#include "Components/RPGInteractorComponent.h"
#include "Components/TextBlock.h"
#include "Interfaces/RPGInteractable.h"

void URPGInteractionPromptWidget::SetInteractor(URPGInteractorComponent* NewInteractor)
{
	if (Interactor.Get())
	{
		Interactor->OnFocusChanged.RemoveDynamic(this, &URPGInteractionPromptWidget::HandleFocusChanged);
		Interactor->OnHoldStarted.RemoveDynamic(this, &URPGInteractionPromptWidget::HandleHoldStarted);
		Interactor->OnHoldEnded.RemoveDynamic(this, &URPGInteractionPromptWidget::HandleHoldEnded);
		Interactor->OnPromptChanged.RemoveDynamic(this, &URPGInteractionPromptWidget::HandlePromptChanged);
	}
	
	Interactor = NewInteractor;
	
	if(NewInteractor)
	{
		Interactor->OnFocusChanged.AddDynamic(this, &URPGInteractionPromptWidget::HandleFocusChanged);
		Interactor->OnHoldStarted.AddDynamic(this, &URPGInteractionPromptWidget::HandleHoldStarted);
		Interactor->OnHoldEnded.AddDynamic(this, &URPGInteractionPromptWidget::HandleHoldEnded);
		Interactor->OnPromptChanged.AddDynamic(this, &URPGInteractionPromptWidget::HandlePromptChanged);
	}
	
	HandleFocusChanged(NewInteractor ? NewInteractor->GetFocusedInteractable() : nullptr, nullptr);
}

void URPGInteractionPromptWidget::SetSuppressed(bool bInSuppressed)
{
	if (bSuppressed == bInSuppressed)
	{
		return;
	}

	bSuppressed = bInSuppressed;

	if (bSuppressed)
	{
		TargetActor.Reset();
		SetVisibility(ESlateVisibility::Collapsed);
		BP_OnPromptHidden();
	}
	else
	{
		HandleFocusChanged(Interactor.IsValid() ? Interactor->GetFocusedInteractable() : nullptr, nullptr);
	}
}

void URPGInteractionPromptWidget::HandlePromptChanged(const FText& NewPrompt)
{
	PromptText->SetText(NewPrompt);
}

void URPGInteractionPromptWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	SetVisibility(ESlateVisibility::Collapsed);
	SetAlignmentInViewport(FVector2D(0.5f,1.f));
}

void URPGInteractionPromptWidget::NativeDestruct()
{
	SetInteractor(nullptr);
	
	Super::NativeDestruct();
}

void URPGInteractionPromptWidget::NativeTick(const FGeometry& Geometry, float InDeltaTime)
{
	Super::NativeTick(Geometry, InDeltaTime);
	
	AActor* Actor = TargetActor.Get();
	if (Actor == nullptr)
	{
		TargetActor.Reset();
		SetVisibility(ESlateVisibility::Collapsed);
		BP_OnPromptHidden();
		return;
	}
	
	FVector2D ScreenPosition;
	const bool bOnScreen = UWidgetLayoutLibrary::ProjectWorldLocationToWidgetPosition(
		GetOwningPlayer(), Actor->GetActorLocation() + AnchorOffset, ScreenPosition, false);
	
	if (bOnScreen)
	{
		SetPositionInViewport(ScreenPosition, false);
		SetRenderOpacity(1.f);
	}
	else
	{
		SetRenderOpacity(0.f);
	}
	
	if (Interactor.Get() && Interactor->IsHolding() && HoldProgressBar)
	{
		HoldProgressBar->SetPercent(Interactor->GetHoldProgress());
	}
}

void URPGInteractionPromptWidget::HandleFocusChanged(UObject* NewFocus, UObject* OldFocus)
{
	
	if (bSuppressed)
	{
		TargetActor.Reset();
		SetVisibility(ESlateVisibility::Collapsed);
		return;
	}
	
	if (NewFocus == nullptr)
	{
		TargetActor.Reset();
		SetVisibility(ESlateVisibility::Collapsed);
		BP_OnPromptHidden();
		return;
	}
	
	AActor* Actor = URPGInteractionLibrary::GetActorFromInteractable(NewFocus);
	if (Actor == nullptr)
	{
		TargetActor.Reset();
		SetVisibility(ESlateVisibility::Collapsed);
		BP_OnPromptHidden();
		return;
	}

	PromptText->SetText(Interactor.IsValid() ? Interactor->GetFocusPrompt() : FText::GetEmpty());
	
	FVector BoundsOrigin;
	FVector BoundsExtent;
	Actor->GetActorBounds(true, BoundsOrigin, BoundsExtent);
	
	AnchorOffset = (BoundsOrigin - Actor->GetActorLocation()) + FVector(0,0,BoundsExtent.Z + VerticalOffset);
	
	TargetActor =Actor;
	if (HoldProgressBar)
	{
		HoldProgressBar->SetVisibility(ESlateVisibility::Collapsed);
	}
	
	SetVisibility(ESlateVisibility::HitTestInvisible);
	BP_OnPromptShown();
}

void URPGInteractionPromptWidget::HandleHoldStarted(UObject* Target, float Duration)
{
	if (HoldProgressBar)
	{
		HoldProgressBar->SetPercent(0.f);
		HoldProgressBar->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
}

void URPGInteractionPromptWidget::HandleHoldEnded(UObject* Target, bool bCompleted)
{
	if (HoldProgressBar)
	{
		HoldProgressBar->SetVisibility(ESlateVisibility::Collapsed);
	}
	BP_OnHoldEnded(bCompleted);
}
