// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RPGInventoryUIComponent.generated.h"

class URPGInventoryScreenWidget;
class UUserWidget;
class APlayerController;
class APawn;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRPGOnInventoryToggledSignature, bool, bIsOpen);

UCLASS(ClassGroup=(RPG), meta=(BlueprintSpawnableComponent))
class RPGINVENTORYUI_API URPGInventoryUIComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	URPGInventoryUIComponent();
	
	UPROPERTY(EditAnywhere, Category = "Inventory|UI")
	TSubclassOf<URPGInventoryScreenWidget> ScreenWidgetClass;
	
	UFUNCTION(BlueprintCallable, Category = "Inventory|UI")
	void ToggleInventory();
	
	UFUNCTION(BlueprintCallable, Category = "Inventory|UI")
	void OpenInventory();
	
	UFUNCTION(BlueprintCallable, Category = "Inventory|UI")
	void CloseInventory();
	
	UFUNCTION(BlueprintPure, Category = "Inventory|UI")
	bool IsInventoryOpen() const;
	
	UPROPERTY(BlueprintAssignable, Category = "Inventory|UI")
	FRPGOnInventoryToggledSignature OnInventoryToggled;

private:
	UPROPERTY()
	TObjectPtr<URPGInventoryScreenWidget> ScreenWidget;
	
	bool bIsOpen = false;
	
	UFUNCTION()
	void HandlePossessedPawnChanged(APawn* OldPawn, APawn* NewPawn);
	
	UPROPERTY()
	TObjectPtr<APlayerController> PC;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
};
