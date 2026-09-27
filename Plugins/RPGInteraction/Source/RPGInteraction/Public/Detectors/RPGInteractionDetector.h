// RPGInteractionDetector.h

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "RPGInteractionDetector.generated.h"

class AActor;

/** Everything a detector needs to know to find the best interactable. */
USTRUCT(BlueprintType)
struct RPGINTERACTION_API FRPGInteractionQuery
{
	GENERATED_BODY()

	/** The actor looking for something to interact with (usually the player pawn). */
	UPROPERTY(BlueprintReadWrite, Category = "Interaction")
	TObjectPtr<AActor> InstigatorActor = nullptr;

	/** Camera location. */
	UPROPERTY(BlueprintReadWrite, Category = "Interaction")
	FVector ViewLocation = FVector::ZeroVector;

	/** Camera rotation. */
	UPROPERTY(BlueprintReadWrite, Category = "Interaction")
	FRotator ViewRotation = FRotator::ZeroRotator;

	/**
	 * How often this query is repeated, in seconds (0 = one-off).
	 * Detectors use it to keep debug shapes on screen until the next scan.
	 */
	UPROPERTY(BlueprintReadWrite, Category = "Interaction")
	float QueryInterval = 0.f;
};

/**
 * Strategy base class: "Which interactable is the instigator looking at?"
 * Designers pick a subclass on the Interactor component.
 */
UCLASS(Abstract, EditInlineNew, DefaultToInstanced, BlueprintType)
class RPGINTERACTION_API URPGInteractionDetector : public UObject
{
	GENERATED_BODY()

public:
	virtual UObject* FindBestInteractable(const FRPGInteractionQuery& Query) const;

protected:
	/** Returns the object implementing IRPGInteractable on HitActor (the actor itself or one of its components), if it can be interacted with. */
	UObject* ResolveInteractable(AActor* HitActor, AActor* InteractionInstigator) const;
};
