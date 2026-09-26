// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RPGInteractionDetector.h"
#include "RPGTraceInteractionDetector.generated.h"


UCLASS(meta = (DisplayName = "Trace Detector"))
class RPGINTERACTION_API URPGTraceInteractionDetector : public URPGInteractionDetector
{
	GENERATED_BODY()
public:
	
	UPROPERTY(EditAnywhere, Category = "Trace")
	TEnumAsByte<ECollisionChannel> TraceChannel = ECC_Visibility;
	
	UPROPERTY(EditAnywhere, Category = "Trace")
	float TraceDistance = 800.f;
	
	UPROPERTY(EditAnywhere, Category = "Trace")
	float SphereRadius = 15.f;
	
	UPROPERTY(EditAnywhere, Category = "Trace")
	float MaxReachFromInstigator = 250.f;
	
	virtual UObject* FindBestInteractable(const FRPGInteractionQuery& Query) const override;
};
