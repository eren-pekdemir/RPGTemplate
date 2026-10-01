// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RPGInteractionPromptComponent.generated.h"

class URPGInteractionPromptWidget;

UCLASS(ClassGroup=(RPG), meta=(BlueprintSpawnableComponent))
class RPGINTERACTIONUI_API URPGInteractionPromptComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	URPGInteractionPromptComponent();
	
	UPROPERTY(EditAnywhere, Category = "Prompt")
	TSubclassOf<URPGInteractionPromptWidget> PromptWidgetClass;
	
	UPROPERTY(EditAnywhere, Category = "Prompt")
	int32 ZOrder = 0;

	UFUNCTION(BlueprintCallable, Category = "Interaction|UI")
	void SetPromptSuppressed(bool bSuppressed);
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	UFUNCTION()
	void HandlePossessedPawnChanged(APawn* OldPawn, APawn* NewPawn);
	
	UPROPERTY()
	TObjectPtr<URPGInteractionPromptWidget> PromptWidget;
};
