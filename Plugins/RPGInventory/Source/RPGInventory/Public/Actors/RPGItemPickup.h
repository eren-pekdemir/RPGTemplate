// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Items/RPGWorldItem.h"
#include "Interfaces/RPGInteractable.h"
#include "Items/RPGRaritySettings.h" 
#include "RPGItemPickup.generated.h"

class URPGItemDefinition;
class UStaticMeshComponent;
class UPointLightComponent;

UCLASS()
class RPGINVENTORY_API ARPGItemPickup : public AActor, public IRPGInteractable, public IRPGWorldItem
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ARPGItemPickup();
	
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pickup")
	TObjectPtr<UStaticMeshComponent> Mesh;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pickup", meta = (ExposeOnSpawn = true))
	TObjectPtr<const URPGItemDefinition> Item = nullptr;

	/** SaveGame: a partly picked-up stack keeps its remainder after loading (needs a RPGSaveableActorComponent). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, SaveGame, Category = "Pickup", meta = (ExposeOnSpawn = true,ClampMin = 1))
	int32 Quantity = 1;
	
	/** Glow in the item's rarity colour. Hidden for low tiers. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pickup|Rarity")
	TObjectPtr<UPointLightComponent> RarityLight;

	/** Rarities from this tier up glow (Common 0, Uncommon 1, Rare 2, ...). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pickup|Rarity", meta = (ClampMin = "0"))
	int32 MinGlowTier = 2;

	/** Designer hook for richer effects (beam, particles). Called on BeginPlay. */
	UFUNCTION(BlueprintImplementableEvent, Category = "Pickup|Rarity")
	void BP_OnRarityApplied(const FRPGRarityInfo& RarityInfo, bool bHasRarity);
	
	virtual void OnConstruction(const FTransform& Transform) override;
	
	virtual bool CanInteract_Implementation(AActor* InteractionInstigator) const override;

	virtual FText GetInteractionPrompt_Implementation(AActor* InteractionInstigator) const override;
	
	virtual float GetInteractionDuration_Implementation() const override;
	
	virtual void Interact_Implementation(AActor* InteractionInstigator) override;
	
	virtual void InitializeWorldItem(const URPGItemDefinition* InItem, int32 InQuantity) override;
	
	virtual void BeginPlay() override; 
	
private:
	void ApplyRarityVisuals();
};
