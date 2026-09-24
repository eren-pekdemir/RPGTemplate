// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interfaces/RPGInteractable.h"
#include "RPGInteractableComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRPGOnInteractedSignature, AActor*, InteractionInstigator);

UCLASS(ClassGroup=(RPG), meta=(BlueprintSpawnableComponent))
class RPGINTERACTION_API URPGInteractableComponent : public UActorComponent , public IRPGInteractable
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	URPGInteractableComponent();
	
	//Delegate
	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FRPGOnInteractedSignature OnInteracted;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	FText InteractionPrompt = NSLOCTEXT("RPGInteraction", "DefaultPrompt", "Interact");
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction", meta = (ClampMin = "0.0", Units = "s"))
	float HoldDuration = 0.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
	bool bEnabled = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	bool bSingleUse = false;
	
	//Interface Functions
	virtual bool CanInteract_Implementation(AActor* InteractionInstigator) const override;
	virtual FText GetInteractionPrompt_Implementation(AActor* InteractionInstigator) const override;
	virtual float GetInteractionDuration_Implementation() const override;
	virtual void Interact_Implementation(AActor* InteractionInstigator)  override;
	
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void SetInteractionEnabled(bool bNewEnabled);
};