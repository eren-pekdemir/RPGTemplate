// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RPGInventoryScreenWidget.generated.h"

class URPGInventoryGridWidget;
class URPGInventoryComponent;
class URPGInventoryScreenWidget;

class UTextBlock;
class UProgressBar;


/**
 * 
 */
UCLASS(Abstract)
class RPGINVENTORYUI_API URPGInventoryScreenWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	void SetInventory(URPGInventoryComponent* NewInventory);
	
	UFUNCTION(BlueprintImplementableEvent)
	void BP_OnOpened();
	
	UFUNCTION(BlueprintImplementableEvent)
	void BP_OnClosed();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor NormalColor;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor OverweightColor;
	
	UFUNCTION(BlueprintCallable)
	void SetFilter(FGameplayTag NewFilter);
	
	UFUNCTION(BlueprintImplementableEvent)
	void BP_OnFilterChanged(FGameplayTag NewFilter);
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<URPGInventoryGridWidget> InventoryGrid;
	
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UProgressBar> WeightBar;
	
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> WeightText;
	
	virtual void NativeDestruct() override;
	
private:
	
	TWeakObjectPtr<URPGInventoryComponent> Inventory;
	
	UFUNCTION()
	void HandleWeightChanged(float NewWeight, float  MaxWeight);
	
	void UpdateWeight(float Current, float Max);
	
	
};
