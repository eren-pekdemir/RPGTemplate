// RPGInteractable.h
// Contract for anything the player can interact with (doors, chests, NPCs, pickups).
// Blueprint-implementable so designers can create interactables without C++.
// Call from C++ with IRPGInteractable::Execute_Interact(Object, Instigator).

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "RPGInteractable.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class URPGInteractable : public UInterface
{
	GENERATED_BODY()
};



class RPGCORE_API IRPGInteractable
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "RPG|Interaction")
	bool CanInteract(AActor* InteractionInstigator) const;

	/** Text shown in the UI, e.g. "Open Chest", "Talk to Merchant". */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "RPG|Interaction")
	FText GetInteractionPrompt(AActor* InteractionInstigator) const;

	/** 0 = instant, > 0 = hold-to-interact duration in seconds. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "RPG|Interaction")
	float GetInteractionDuration() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "RPG|Interaction")
	void Interact(AActor* InteractionInstigator);
};
