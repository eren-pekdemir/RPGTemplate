#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RPGItemNotificationWidget.generated.h"

class APawn;
class UVerticalBox;
class URPGInventoryComponent;
class URPGItemDefinition;
class URPGItemNotificationEntryWidget;

/** "+6 Arrow" feed for the HUD. Follows the owning player's possessed pawn. */
UCLASS(Abstract)
class RPGINVENTORYUI_API URPGItemNotificationWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/** Bind manually; nullptr stops listening. Done automatically for the possessed pawn. */
	UFUNCTION(BlueprintCallable, Category = "RPG|Notification")
	void SetInventory(URPGInventoryComponent* NewInventory);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVerticalBox> EntryBox;

	UPROPERTY(EditDefaultsOnly, Category = "RPG|Notification")
	TSubclassOf<URPGItemNotificationEntryWidget> EntryClass;

	UPROPERTY(EditDefaultsOnly, Category = "RPG|Notification", meta = (ClampMin = "1"))
	int32 MaxEntries = 5;

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

private:
	UFUNCTION()
	void HandleItemsReceived(const URPGItemDefinition* Item, int32 Quantity);

	UFUNCTION()
	void HandlePossessedPawnChanged(APawn* OldPawn, APawn* NewPawn);

	UPROPERTY()
	TObjectPtr<URPGInventoryComponent> Inventory;
};