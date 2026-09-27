// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "RPGInteractionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class RPGINTERACTION_API URPGInteractionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	
	UFUNCTION(BlueprintPure, Category = "Interaction")
	static AActor* GetActorFromInteractable(UObject* Interactable);
};
