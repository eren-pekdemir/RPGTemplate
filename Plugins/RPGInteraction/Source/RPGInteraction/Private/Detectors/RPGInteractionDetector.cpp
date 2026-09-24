// Fill out your copyright notice in the Description page of Project Settings.


#include "Detectors/RPGInteractionDetector.h"

#include "Interfaces/RPGInteractable.h"

UObject* URPGInteractionDetector::FindBestInteractable(const FRPGInteractionQuery& Query) const
{
	return nullptr;
}

UObject* URPGInteractionDetector::ResolveInteractable(AActor* HitActor, AActor* InteractionInstigator) const
{
	if (!HitActor) return nullptr;
	
	UObject* Candidate = nullptr;
	
	if (HitActor->Implements<URPGInteractable>())
		Candidate = HitActor;
	else
	{
		Candidate = HitActor->FindComponentByInterface(URPGInteractable::StaticClass());
	}
	
	if (!Candidate) return nullptr;
	
	if (IRPGInteractable::Execute_CanInteract(Candidate, InteractionInstigator))
	{
		return Candidate;
	}
	
	return nullptr;
		
}
