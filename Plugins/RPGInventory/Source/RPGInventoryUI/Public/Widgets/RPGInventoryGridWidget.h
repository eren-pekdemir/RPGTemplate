#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RPGInventoryTypes.h"                   
#include "GameplayTagContainer.h"
#include "RPGInventoryGridWidget.generated.h"

class UUniformGridPanel;
class URPGInventorySlotWidget;
class URPGInventoryComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRPGOnEntryActivatedSignature, const FRPGItemEntry&, Entry);

UCLASS(Abstract)
class RPGINVENTORYUI_API URPGInventoryGridWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	
	UPROPERTY(BlueprintAssignable, Category = "Inventory|Grid")
	FRPGOnEntryActivatedSignature OnEntryActivated;
	
	UFUNCTION(BlueprintCallable, Category = "Inventory|Grid")
	void SetInventory(URPGInventoryComponent* NewInventory);
	
	UFUNCTION(BlueprintCallable, Category = "Inventory|Grid")
	void SetFilter(FGameplayTag NewFilter);
	
	UFUNCTION(BlueprintPure)
	FGameplayTag GetFilter() const ; 

protected:                                      
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UUniformGridPanel> SlotGrid;

	UPROPERTY(EditAnywhere, Category = "Inventory|Grid")
	TSubclassOf<URPGInventorySlotWidget> SlotWidgetClass;

	UPROPERTY(EditAnywhere, Category = "Inventory|Grid", meta = (ClampMin = 1))
	int32 Columns = 6;

	virtual void NativeDestruct() override;

private:
	TWeakObjectPtr<URPGInventoryComponent> Inventory;

	UPROPERTY()                                
	TArray<TObjectPtr<URPGInventorySlotWidget>> SlotWidgets;

	void BuildSlots();
	void Refresh();

	UFUNCTION()
	void OnItemAddedHandler(const FRPGItemEntry& Entry, int32 AddedQuantity);

	UFUNCTION()
	void OnItemRemovedHandler(const FRPGItemEntry& Entry, int32 RemovedQuantity);

	UFUNCTION()
	void OnInventoryRefreshedHandler();
	
	UFUNCTION()
	void HandleSlotRightClicked(URPGInventorySlotWidget* SlotWidget, bool bWholeStack);
	
	UFUNCTION()
	void HandleSlotDoubleClicked(URPGInventorySlotWidget* SlotWidget);
	
	FGameplayTag FilterTag;
};