// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/RPGInteractableComponent.h"


// Sets default values for this component's properties
URPGInteractableComponent::URPGInteractableComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

bool URPGInteractableComponent::CanInteract_Implementation(AActor* InteractionInstigator) const
{
	return bEnabled;
}

FText URPGInteractableComponent::GetInteractionPrompt_Implementation(AActor* InteractionInstigator) const
{
	return InteractionPrompt;
}

float URPGInteractableComponent::GetInteractionDuration_Implementation() const
{
	return HoldDuration;
}

void URPGInteractableComponent::Interact_Implementation(AActor* InteractionInstigator)
{
	if (IRPGInteractable::Execute_CanInteract(this, InteractionInstigator))
	{
		OnInteracted.Broadcast(InteractionInstigator);
		
		if (bSingleUse)
		{
			SetInteractionEnabled(false);
		}
	}
}

void URPGInteractableComponent::SetInteractionEnabled(bool bNewEnabled)
{
	bEnabled = bNewEnabled;
}

