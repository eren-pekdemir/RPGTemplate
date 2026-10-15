#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RPGItemNotificationEntryWidget.generated.h"

class UImage;
class UTextBlock;
class URPGItemDefinition;

/** One "+6 Arrow" line. Fades out and removes itself after DisplayTime. */
UCLASS(Abstract)
class RPGINVENTORYUI_API URPGItemNotificationEntryWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void Setup(const URPGItemDefinition* InItem, int32 InQuantity);

	/** Same item again while visible: add to the count and restart the timer. */
	void AddQuantity(int32 Amount);

	const URPGItemDefinition* GetItem() const { return Item; }

protected:
	UPROPERTY(BlueprintReadOnly, Category = "RPG|Notification", meta = (BindWidget))
	TObjectPtr<UTextBlock> MessageText;

	UPROPERTY(BlueprintReadOnly, Category = "RPG|Notification", meta = (BindWidgetOptional))
	TObjectPtr<UImage> IconImage;

	UPROPERTY(EditDefaultsOnly, Category = "RPG|Notification", meta = (ClampMin = "0.1", Units = "s"))
	float DisplayTime = 3.f;

	/** Last part of DisplayTime during which the line fades out. */
	UPROPERTY(EditDefaultsOnly, Category = "RPG|Notification", meta = (ClampMin = "0", Units = "s"))
	float FadeTime = 0.5f;

	/** Designer hook: pulse when the count grows. */
	UFUNCTION(BlueprintImplementableEvent, Category = "RPG|Notification")
	void BP_OnQuantityChanged(int32 NewQuantity);

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
	void UpdateText();

	UPROPERTY()
	TObjectPtr<const URPGItemDefinition> Item;

	int32 Quantity = 0;
	float Age = 0.f;
};