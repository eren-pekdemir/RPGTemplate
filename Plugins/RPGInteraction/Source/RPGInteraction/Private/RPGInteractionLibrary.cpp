// Fill out your copyright notice in the Description page of Project Settings.


#include "RPGInteractionLibrary.h"

AActor* URPGInteractionLibrary::GetActorFromInteractable(UObject* Interactable)
{
	if (!Interactable) return nullptr;
	
	AActor* Actor = Cast<AActor>(Interactable);
	if (Actor)
	{
		return Actor;
	}
	
	UActorComponent* FocusComponent = Cast<UActorComponent>(Interactable);
	if (FocusComponent)
	{
		return FocusComponent->GetOwner();
	}
	
	return nullptr;
}
