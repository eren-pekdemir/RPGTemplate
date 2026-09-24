// Fill out your copyright notice in the Description page of Project Settings.


#include "Detectors/RPGTraceInteractionDetector.h"

UObject* URPGTraceInteractionDetector::FindBestInteractable(const FRPGInteractionQuery& Query) const
{
	
	if (!Query.InstigatorActor) return nullptr;
	if (!Query.InstigatorActor->GetWorld()) return nullptr;
	
	FCollisionQueryParams Params(SCENE_QUERY_STAT(RPGInteractionTrace),false);
	Params.AddIgnoredActor(Query.InstigatorActor);
	
	FHitResult HitResult;
	
	FVector TraceEnd = Query.ViewLocation + Query.ViewRotation.Vector() * TraceDistance;
	
	Query.InstigatorActor->GetWorld()->SweepSingleByChannel(HitResult,Query.ViewLocation,TraceEnd, FQuat::Identity, TraceChannel,FCollisionShape::MakeSphere(SphereRadius), Params);
	
	if (!HitResult.bBlockingHit) return nullptr;
	
	if (FVector::Dist(HitResult.ImpactPoint, Query.InstigatorActor->GetActorLocation()) > MaxReachFromInstigator)
		return nullptr;
	
	return ResolveInteractable(HitResult.GetActor(), Query.InstigatorActor);
}
