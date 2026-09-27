// RPGTraceInteractionDetector.cpp

#include "Detectors/RPGTraceInteractionDetector.h"

#include "CollisionQueryParams.h"
#include "Engine/HitResult.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "RPGInteractionDebug.h"

#if ENABLE_DRAW_DEBUG
#include "DrawDebugHelpers.h"
#endif

#if ENABLE_DRAW_DEBUG
namespace
{
	/**
	 * Draws the result of one interaction trace.
	 *   Red    = sweep hit nothing
	 *   Yellow = sweep hit something, but it was rejected (too far / not interactable / CanInteract false)
	 *   Green  = accepted, this object becomes the focus
	 *   Cyan circle = MaxReachFromInstigator around the character
	 */
	void DrawTraceDebug(
		const UWorld* World,
		const FRPGInteractionQuery& Query,
		const FVector& TraceStart,
		const FVector& TraceEnd,
		float SphereRadius,
		float MaxReach,
		bool bHit,
		const FHitResult& Hit,
		bool bAccepted)
	{
		const float Lifetime = RPGInteractionDebug::GetDrawLifetime(Query.QueryInterval);

		const FColor Color = !bHit ? FColor::Red : (bAccepted ? FColor::Green : FColor::Yellow);

		// Hit.Location    = center of the sweep sphere at the moment of impact.
		// Hit.ImpactPoint = exact contact point on the surface.
		const FVector LineEnd = bHit ? Hit.Location : TraceEnd;

		DrawDebugLine(World, TraceStart, LineEnd, Color, false, Lifetime, 0, 1.f);

		if (bHit)
		{
			DrawDebugSphere(World, Hit.Location, SphereRadius, 12, Color, false, Lifetime);
			DrawDebugPoint(World, Hit.ImpactPoint, 10.f, Color, false, Lifetime);
		}

		if (const AActor* Instigator = Query.InstigatorActor)
		{
			// Horizontal circle: the circle plane is spanned by world X and world Y.
			DrawDebugCircle(World, Instigator->GetActorLocation(), MaxReach, 32, FColor::Cyan,
				false, Lifetime, 0, 1.f, FVector(1.f, 0.f, 0.f), FVector(0.f, 1.f, 0.f), false);
		}
	}
}
#endif

UObject* URPGTraceInteractionDetector::FindBestInteractable(const FRPGInteractionQuery& Query) const
{
	AActor* Instigator = Query.InstigatorActor;
	if (!Instigator)
	{
		return nullptr;
	}

	UWorld* World = Instigator->GetWorld();
	if (!World)
	{
		return nullptr;
	}

	const FVector TraceStart = Query.ViewLocation;
	const FVector TraceEnd = TraceStart + Query.ViewRotation.Vector() * TraceDistance;

	FCollisionQueryParams Params(SCENE_QUERY_STAT(RPGInteractionTrace), false);
	Params.AddIgnoredActor(Instigator);

	FHitResult Hit;
	const bool bHit = World->SweepSingleByChannel(
		Hit, TraceStart, TraceEnd, FQuat::Identity, TraceChannel,
		FCollisionShape::MakeSphere(SphereRadius), Params);

	// Single exit point: every outcome flows to the end of the function,
	// so the debug drawing below can show ALL of them (no hit / rejected / accepted).
	UObject* Result = nullptr;

	if (bHit)
	{
		const float DistSq = FVector::DistSquared(Hit.ImpactPoint, Instigator->GetActorLocation());
		if (DistSq <= FMath::Square(MaxReachFromInstigator))
		{
			Result = ResolveInteractable(Hit.GetActor(), Instigator);
		}
	}

#if ENABLE_DRAW_DEBUG
	if (RPGInteractionDebug::IsEnabled())
	{
		DrawTraceDebug(World, Query, TraceStart, TraceEnd, SphereRadius, MaxReachFromInstigator,
			bHit, Hit, Result != nullptr);
	}
#endif

	return Result;
}
