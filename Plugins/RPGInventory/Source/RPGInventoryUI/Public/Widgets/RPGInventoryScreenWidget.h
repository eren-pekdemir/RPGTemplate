// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RPGInventoryScreenWidget.generated.h"

class URPGInventoryGridWidget;
class URPGInventoryComponent;
class URPGInventoryScreenWidget;


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
	
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<URPGInventoryGridWidget> InventoryGrid;
	
	
};
