// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Detectors/RPGInteractionDetector.h"
#include "RPGInteractorComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FRPGOnFocusChangedSignature, UObject*, NewFocus, UObject*, OldFocus);

UCLASS(ClassGroup=(RPG), meta=(BlueprintSpawnableComponent))
class RPGINTERACTION_API URPGInteractorComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	URPGInteractorComponent();
	
	UPROPERTY(EditAnywhere, Instanced, Category = "Interaction")
	TObjectPtr<URPGInteractionDetector> Detector;
	
	UPROPERTY(EditAnywhere, Category = "Interaction",meta = (ClampMin = "0.01", Units = "s"))
	float  ScanInterval = 0.1f;
	
	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FRPGOnFocusChangedSignature OnFocusChanged;
	
	UFUNCTION(BlueprintCallable, Category="Interaction")
	void StartInteraction();
	
	UFUNCTION(BlueprintCallable, Category="Interaction")
	void StopInteraction();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	
	TWeakObjectPtr<UObject> FocusedInteractable;
	
	FTimerHandle ScanTimerHandle;
	
	void ScanForInteractables();
	
	void SetFocus(UObject* NewFocus);
};
