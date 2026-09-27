// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RPGInteractionPromptWidget.generated.h"


class UTextBlock; class UProgressBar; class URPGInteractorComponent;
/**
 * 
 */
UCLASS(Abstract)
class RPGINTERACTIONUI_API URPGInteractionPromptWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	
	void SetInteractor(URPGInteractorComponent* NewInteractor);
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Prompt")
	void BP_OnPromptShown();
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Prompt")
	void BP_OnPromptHidden();
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Prompt")
	void BP_OnHoldEnded(bool bCompleted);
	
private:
	TWeakObjectPtr<URPGInteractorComponent> Interactor;
	TWeakObjectPtr<AActor> TargetActor;
	FVector AnchorOffset = FVector::ZeroVector;
	
	UFUNCTION()
	void HandleFocusChanged(UObject* NewFocus, UObject* OldFocus);
	
	UFUNCTION()
	void HandleHoldStarted(UObject* Target, float Duration);
	
	UFUNCTION()
	void HandleHoldEnded(UObject* Target, bool bCompleted);
	
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> PromptText;
	
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UProgressBar> HoldProgressBar;
	
	UPROPERTY(EditAnywhere, Category = "Prompt")
	float VerticalOffset = 20.f;
	
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& Geometry, float InDeltaTime) override;
};
