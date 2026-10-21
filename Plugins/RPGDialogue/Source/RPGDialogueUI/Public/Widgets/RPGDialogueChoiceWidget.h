// RPGDialogueChoiceWidget.h
// One answer button in the dialogue screen.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RPGDialogueTypes.h"
#include "RPGDialogueChoiceWidget.generated.h"

class UButton;
class UTextBlock;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRPGOnDialogueChoiceClickedSignature, int32, ChoiceIndex);

UCLASS(Abstract)
class RPGDIALOGUEUI_API URPGDialogueChoiceWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/** Reports FRPGDialogueChoiceView::Index (the index inside the node, not the row number). */
	UPROPERTY(BlueprintAssignable, Category = "RPG|Dialogue")
	FRPGOnDialogueChoiceClickedSignature OnChoiceClicked;

	/** DisplayNumber: 1-based row number shown as "1." (keyboard shortcut). */
	void Setup(const FRPGDialogueChoiceView& InView, int32 DisplayNumber);

	const FRPGDialogueChoiceView& GetView() const { return View; }

protected:
	UPROPERTY(BlueprintReadOnly, Category = "RPG|Dialogue", meta = (BindWidget))
	TObjectPtr<UButton> ChoiceButton;

	UPROPERTY(BlueprintReadOnly, Category = "RPG|Dialogue", meta = (BindWidget))
	TObjectPtr<UTextBlock> ChoiceText;

	UPROPERTY(BlueprintReadOnly, Category = "RPG|Dialogue", meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> NumberText;

	UPROPERTY(EditDefaultsOnly, Category = "RPG|Dialogue")
	FSlateColor AvailableColor = FSlateColor(FLinearColor(0.95f, 0.92f, 0.85f, 1.f));

	UPROPERTY(EditDefaultsOnly, Category = "RPG|Dialogue")
	FSlateColor UnavailableColor = FSlateColor(FLinearColor(0.45f, 0.45f, 0.45f, 1.f));

	/** For styling (e.g. a lock icon on unavailable choices). */
	UFUNCTION(BlueprintImplementableEvent, Category = "RPG|Dialogue")
	void BP_OnSetup(const FRPGDialogueChoiceView& InView);

	virtual void NativeOnInitialized() override;

private:
	UFUNCTION()
	void HandleClicked();

	FRPGDialogueChoiceView View;
};
