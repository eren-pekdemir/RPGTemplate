// RPGCombatLibrary.cpp

#include "RPGCombatLibrary.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Engine/OverlapResult.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "RPGCoreTags.h"

AActor* URPGCombatLibrary::FindMeleeTarget(AActor* Attacker, float Radius, float MaxAngle)
{
	UWorld* World = Attacker ? Attacker->GetWorld() : nullptr;
	if (!World || Radius <= 0.f) return nullptr;

	const FVector Origin = Attacker->GetActorLocation();
	const FVector Forward = Attacker->GetActorForwardVector().GetSafeNormal2D();
	const float MinDot = FMath::Cos(FMath::DegreesToRadians(FMath::Clamp(MaxAngle, 0.f, 180.f)));

	FCollisionQueryParams Params(SCENE_QUERY_STAT(RPGFindMeleeTarget), false, Attacker);
	TArray<FOverlapResult> Overlaps;
	World->OverlapMultiByObjectType(Overlaps, Origin, FQuat::Identity, FCollisionObjectQueryParams(ECC_Pawn),
		FCollisionShape::MakeSphere(Radius), Params);

	AActor* Best = nullptr;
	float BestDistSq = TNumericLimits<float>::Max();

	for (const FOverlapResult& Overlap : Overlaps)
	{
		AActor* Candidate = Overlap.GetActor();
		if (!Candidate || Candidate == Attacker) continue;

		// Only combatants: an ability system that is not dead.
		const UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Candidate);
		if (!ASC || ASC->HasMatchingGameplayTag(RPGTags::State_Dead)) continue;

		const FVector ToCandidate = (Candidate->GetActorLocation() - Origin).GetSafeNormal2D();
		if (!Forward.IsNearlyZero() && FVector::DotProduct(Forward, ToCandidate) < MinDot) continue;

		const float DistSq = FVector::DistSquared2D(Origin, Candidate->GetActorLocation());
		if (DistSq < BestDistSq)
		{
			BestDistSq = DistSq;
			Best = Candidate;
		}
	}

	return Best;
}

bool URPGCombatLibrary::FaceMeleeTarget(AActor* Attacker, float Radius, float MaxAngle)
{
	if (!Attacker) return false;

	FVector Direction = FVector::ZeroVector;

	AActor* Target = FindMeleeTarget(Attacker, Radius, MaxAngle);
	if (Target)
	{
		Direction = Target->GetActorLocation() - Attacker->GetActorLocation();
	}
	else if (const APawn* Pawn = Cast<APawn>(Attacker))
	{
		// No target: swing where the player is steering.
		Direction = Pawn->GetLastMovementInputVector();
	}

	Direction.Z = 0.f;
	if (!Direction.IsNearlyZero())
	{
		Attacker->SetActorRotation(FRotator(0.f, Direction.Rotation().Yaw, 0.f));
	}

	return Target != nullptr;
}
