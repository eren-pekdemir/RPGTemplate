// RPGInteractableComponent.h

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interfaces/RPGInteractable.h"
#include "Interfaces/RPGSaveable.h"
#include "RPGInteractionTypes.h"
#include "RPGInteractableComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRPGOnInteractedSignature, AActor*, InteractionInstigator);

/**
 * Makes its actor usable by the player's interactor (press or hold the interact key).
 * The component only reports the interaction: the actor's Blueprint reacts in OnInteracted (open the chest,
 * start the dialogue). Actors that need custom rules implement IRPGInteractable directly instead.
 */
UCLASS(ClassGroup=(RPG), meta=(BlueprintSpawnableComponent))
class RPGINTERACTION_API URPGInteractableComponent : public UActorComponent , public IRPGInteractable, public IRPGSaveable
{
	GENERATED_BODY()

public:
	URPGInteractableComponent();
	
	/** Fired when an interaction completes (after the hold, if any). */
	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FRPGOnInteractedSignature OnInteracted;
	
	/** Verb shown in the prompt ("Open", "Talk"). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	FText InteractionPrompt = NSLOCTEXT("RPGInteraction", "DefaultPrompt", "Interact");
	
	/** 0 = press to use; above 0 the key must be held this long (a progress bar fills in the prompt). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction", meta = (ClampMin = "0.0", Units = "s"))
	float HoldDuration = 0.f;
	
	/** Saved (with a RPGSaveableActorComponent on the actor): a used single-use object stays used after loading. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, SaveGame, Category = "Interaction")
	bool bEnabled = true;
	
	/** Disables itself after the first use (a looted chest). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	bool bSingleUse = false;
	
	// IRPGInteractable: usable while bEnabled; Interact disables a single-use object before broadcasting.
	virtual bool CanInteract_Implementation(AActor* InteractionInstigator) const override;
	virtual FText GetInteractionPrompt_Implementation(AActor* InteractionInstigator) const override;
	virtual float GetInteractionDuration_Implementation() const override;
	virtual void Interact_Implementation(AActor* InteractionInstigator)  override;
	
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void SetInteractionEnabled(bool bNewEnabled);
	
	/** Outline colour the highlighter draws while this object has focus. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
	ERPGHighlightStyle HighlightStyle = ERPGHighlightStyle::Default;
};