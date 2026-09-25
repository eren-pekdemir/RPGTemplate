// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Detectors/RPGInteractionDetector.h"
#include "RPGInteractorComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FRPGOnFocusChangedSignature, UObject*, NewFocus, UObject*, OldFocus);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FRPGOnHoldStartedSignature,UObject*, Target, float, Duration);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FRPGOnHoldEndedSignature, UObject*, Target, bool, bCompleted);

UCLASS(ClassGroup=(RPG), meta=(BlueprintSpawnableComponent))
class RPGINTERACTION_API URPGInteractorComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	URPGInteractorComponent();
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	UPROPERTY(EditAnywhere, Instanced, Category = "Interaction")
	TObjectPtr<URPGInteractionDetector> Detector;
	
	UPROPERTY(EditAnywhere, Category = "Interaction",meta = (ClampMin = "0.01", Units = "s"))
	float  ScanInterval = 0.1f;
	
	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FRPGOnFocusChangedSignature OnFocusChanged;
	
	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FRPGOnHoldStartedSignature OnHoldStarted;
	
	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FRPGOnHoldEndedSignature OnHoldEnded;
	
	UFUNCTION(BlueprintCallable, Category="Interaction")
	void StartInteraction();
	
	UFUNCTION(BlueprintCallable, Category="Interaction")
	void StopInteraction();
	
	UFUNCTION(BlueprintPure, Category = "Interaction")
	float GetHoldProgress() const;
	
	UFUNCTION(BlueprintPure, Category = "Interaction")
	bool IsHolding() const;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	
	TWeakObjectPtr<UObject> FocusedInteractable;
	
	FTimerHandle ScanTimerHandle;
	
	void ScanForInteractables();
	
	void SetFocus(UObject* NewFocus);
	
	TWeakObjectPtr<UObject> HoldTarget;
	
	float RequiredHoldDuration = 0.f;
	
	float ElapsedHoldTime = 0.f;
	
	bool bIsHolding = false;	
	
	void StartHold(UObject* Target, float Duration);
	
	void CompleteHold();
	
	void CancelHold();
	
	void EndHold(bool bCompleted);
	
	
};
