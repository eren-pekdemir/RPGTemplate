// Fill out your copyright notice in the Description page of Project Settings.


#include "Detectors/RPGOverlapInteractionDetector.h"
#include "Engine/OverlapResult.h"

namespace
{
	struct FInteractionCandidate
	{
		UObject* Interactable = nullptr;
		AActor* Actor = nullptr;
		float Score = 0.f;
	};
}

UObject* URPGOverlapInteractionDetector::FindBestInteractable(const FRPGInteractionQuery& Query) const
{
	if (Query.InstigatorActor == nullptr) return nullptr;
	const FVector Origin = Query.InstigatorActor->GetActorLocation();
	const FVector Forward = Query.InstigatorActor->GetActorForwardVector().GetSafeNormal2D();
	const float CosMaxAngle = FMath::Cos(FMath::DegreesToRadians(MaxAngle));
	
	FCollisionQueryParams CollisionParameters(SCENE_QUERY_STAT(RPGInteractionOverlap), false);
	CollisionParameters.AddIgnoredActor(Query.InstigatorActor);
	TArray<FOverlapResult> Overlaps;
	
	UWorld* World = Query.InstigatorActor->GetWorld();
	if (World == nullptr) return nullptr;
	
	World->OverlapMultiByChannel(Overlaps,Origin,FQuat::Identity ,OverlapChannel, FCollisionShape::MakeSphere(SearchRadius), CollisionParameters );
	
	TSet<const AActor*> VisitedActors;
	
	TArray<FInteractionCandidate> Candidates;
	
	for (const auto& Overlap : Overlaps)
	{
		AActor* Actor = Overlap.GetActor();
		if (!Actor) continue;
		if (VisitedActors.Contains(Actor)) continue;
		
		VisitedActors.Add(Actor);
		
		UObject* Interactable = ResolveInteractable(Actor, Query.InstigatorActor);
		if (!Interactable)  continue;
		
		FVector ToTarget = Actor->GetActorLocation() - Origin;
		const float Distance = ToTarget.Size();
		
		const float Dot = FVector::DotProduct(Forward, ToTarget.GetSafeNormal2D());
		if (Dot < CosMaxAngle) continue;
		
		float DistanceScore = FMath::Clamp(1 - (Distance / SearchRadius), 0.f, 1.f);
		float AngleScore = (Dot - CosMaxAngle) / (1 - CosMaxAngle);
		float Score = DistanceWeight * DistanceScore + AngleWeight * AngleScore;
		
		if(Interactable == Query.CurrentFocus)
		{
			Score += CurrentFocusBonus;
		}
		
		Candidates.Add(FInteractionCandidate(Interactable, Actor, Score));
	}
	
	Candidates.Sort([](const FInteractionCandidate& A, const FInteractionCandidate& B) { return A.Score > B.Score; });
		
	for (const auto& Candidate : Candidates)
	{
		if (!bRequireLineOfSight || HasLineOfSight(World, Query.InstigatorActor, Candidate.Actor))
		{
			return Candidate.Interactable;
		}
	}
	return nullptr;
}

bool URPGOverlapInteractionDetector::HasLineOfSight(const UWorld* World, const AActor* Instigator,
	const AActor* Target) const
{
	if (!World || !Target || !Instigator) return false;
	
	FVector EyeLocation;
	FRotator EyeRotation;
	FVector TargetLocation =Target->GetActorLocation();
	Instigator->GetActorEyesViewPoint(EyeLocation, EyeRotation);
	
	FCollisionQueryParams Params(SCENE_QUERY_STAT(RPGInteractionLineOfSight), false);
	Params.AddIgnoredActor(Instigator);
	
	FHitResult Hit;
	const bool bHit = World->LineTraceSingleByChannel(Hit, EyeLocation, TargetLocation, LineOfSightChannel, Params);
	
	return !bHit || Hit.GetActor() == Target;
}
