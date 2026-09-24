// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "GameFramework/Actor.h"
#include "RPGInteractionDetector.generated.h"

USTRUCT(BlueprintType)
struct RPGINTERACTION_API FRPGInteractionQuery
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite, Category = "Interaction")
	TObjectPtr<AActor> InstigatorActor = nullptr;
	
	UPROPERTY(BlueprintReadWrite, Category = "Interaction")
	FVector ViewLocation = FVector::ZeroVector;
	
	UPROPERTY(BlueprintReadWrite, Category = "Interaction")
	FRotator ViewRotation = FRotator::ZeroRotator;
};

/**
 * 
 */
UCLASS(Abstract, EditInlineNew, DefaultToInstanced, BlueprintType)
class RPGINTERACTION_API URPGInteractionDetector : public UObject
{
	GENERATED_BODY()
public:
	virtual UObject* FindBestInteractable(const FRPGInteractionQuery& Query) const;
	
protected:
	
	UObject* ResolveInteractable(AActor* HitActor, AActor* InteractionInstigator) const;
	
};
