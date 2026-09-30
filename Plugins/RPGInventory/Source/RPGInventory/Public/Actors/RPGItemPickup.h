// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/RPGInteractable.h"
#include "RPGItemPickup.generated.h"

class URPGItemDefinition;
class UStaticMeshComponent;

UCLASS()
class RPGINVENTORY_API ARPGItemPickup : public AActor, public IRPGInteractable
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ARPGItemPickup();
	
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pickup")
	TObjectPtr<UStaticMeshComponent> Mesh;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pickup", meta = (ExposeOnSpawn = true))
	TObjectPtr<const URPGItemDefinition> Item = nullptr;;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pickup", meta = (ExposeOnSpawn = true,ClampMin = 1))
	int32 Quantity = 1;
	
	virtual void OnConstruction(const FTransform& Transform) override;
	
	virtual bool CanInteract_Implementation(AActor* InteractionInstigator) const override;

	virtual FText GetInteractionPrompt_Implementation(AActor* InteractionInstigator) const override;
	
	virtual float GetInteractionDuration_Implementation() const override;
	
	virtual void Interact_Implementation(AActor* InteractionInstigator) override;
	
	
};
