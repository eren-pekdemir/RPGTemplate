// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RPGInteractionTypes.h"
#include "RPGHighlighterComponent.generated.h"

class URPGInteractorComponent;

UCLASS(ClassGroup=(RPG), meta=(BlueprintSpawnableComponent))
class RPGINTERACTION_API URPGHighlighterComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	URPGHighlighterComponent();
	
	UPROPERTY(EditAnywhere, Category = "Highlight")
	FName HighlightTag = "Highlight";
	
	UPROPERTY(EditAnywhere, Category = "Highlight")
	ERPGHighlightStyle DefaultStyle = ERPGHighlightStyle::Default;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	UFUNCTION()
	void HandleFocusChanged(UObject* NewFocus, UObject* OldFocus);
	void StartHighlight(UObject* Focus);
	void StopHighlight();
	
	AActor* GetActorFromFocus(UObject* Focus) const;


private:
	TArray<TWeakObjectPtr<UPrimitiveComponent>> HighlightedComponents;
	
	TWeakObjectPtr<URPGInteractorComponent> InteractorComponent;
	
	
};


