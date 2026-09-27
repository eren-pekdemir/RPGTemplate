// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Detectors/RPGInteractionDetector.h"
#include "RPGOverlapInteractionDetector.generated.h"

/**
 * 
 */
UCLASS(meta = (DisplayName = "Overlap Detector"))
class RPGINTERACTION_API URPGOverlapInteractionDetector : public URPGInteractionDetector
{
	GENERATED_BODY()
public:
	virtual UObject* FindBestInteractable(const FRPGInteractionQuery& Query) const override;
	
	UPROPERTY(EditAnywhere, Category = "Overlap")
	TEnumAsByte<ECollisionChannel>	OverlapChannel = ECC_Visibility;
	
	UPROPERTY(EditAnywhere, Category = "Overlap")
	float SearchRadius = 200.f;
	
	UPROPERTY(EditAnywhere, Category = "Overlap")
	float MaxAngle = 70.f;
	
	UPROPERTY(EditAnywhere, Category = "Overlap")
	float DistanceWeight = 1.f;
	
	UPROPERTY(EditAnywhere, Category = "Overlap")
	float AngleWeight = 1.f;
	
	UPROPERTY(EditAnywhere, Category = "Overlap")
	float CurrentFocusBonus = 0.15f;
	
	UPROPERTY(EditAnywhere, Category = "Overlap")
	bool bRequireLineOfSight = true;
	
	UPROPERTY(EditAnywhere, Category = "Overlap")
	TEnumAsByte<ECollisionChannel> LineOfSightChannel = ECC_Visibility;
	
protected:
	
	bool HasLineOfSight(const UWorld* World, const AActor* Instigator, const AActor* Target) const;
};
