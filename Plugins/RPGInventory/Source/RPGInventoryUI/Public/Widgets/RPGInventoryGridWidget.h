#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RPGInventoryTypes.h"                   
#include "RPGInventoryGridWidget.generated.h"

class UUniformGridPanel;
class URPGInventorySlotWidget;
class URPGInventoryComponent;

UCLASS(Abstract)
class RPGINVENTORYUI_API URPGInventoryGridWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Inventory|Grid")
	void SetInventory(URPGInventoryComponent* NewInventory);

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
};