// RPGDialogueScreenWidget.h
// Speaker name, text (with optional typewriter effect), portrait and answers. Modal: clicks never reach the game.
// Keys: 1-9 pick an answer, Space/Enter continue (or finish the typing), Escape leaves the conversation.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RPGDialogueScreenWidget.generated.h"

class UButton;
class UImage;
class UPanelWidget;
class UTextBlock;
class URPGDialogueChoiceWidget;
class URPGDialogueComponent;

UCLASS(Abstract)
class RPGDIALOGUEUI_API URPGDialogueScreenWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/** nullptr unbinds. */
	void SetDialogue(URPGDialogueComponent* InDialogue);

	UFUNCTION(BlueprintCallable, Category = "RPG|Dialogue")
	void Refresh();

	/** Show the whole line immediately. */
	UFUNCTION(BlueprintCallable, Category = "RPG|Dialogue")
	void FinishTyping();

	UFUNCTION(BlueprintPure, Category = "RPG|Dialogue")
	bool IsTyping() const;

protected:
	UPROPERTY(BlueprintReadOnly, Category = "RPG|Dialogue", meta = (BindWidget))
	TObjectPtr<UTextBlock> SpeakerNameText;

	UPROPERTY(BlueprintReadOnly, Category = "RPG|Dialogue", meta = (BindWidget))
	TObjectPtr<UTextBlock> DialogueText;

	/** Vertical Box (or any panel) the answers are added to. */
	UPROPERTY(BlueprintReadOnly, Category = "RPG|Dialogue", meta = (BindWidget))
	TObjectPtr<UPanelWidget> ChoiceList;

	/** Shown for lines without answers. */
	UPROPERTY(BlueprintReadOnly, Category = "RPG|Dialogue", meta = (BindWidgetOptional))
	TObjectPtr<UButton> ContinueButton;

	UPROPERTY(BlueprintReadOnly, Category = "RPG|Dialogue", meta = (BindWidgetOptional))
	TObjectPtr<UButton> CloseButton;

	UPROPERTY(BlueprintReadOnly, Category = "RPG|Dialogue", meta = (BindWidgetOptional))
	TObjectPtr<UImage> PortraitImage;

	UPROPERTY(EditDefaultsOnly, Category = "RPG|Dialogue")
	TSubclassOf<URPGDialogueChoiceWidget> ChoiceClass;

	/** Typewriter speed. 0 = whole line at once. */
	UPROPERTY(EditDefaultsOnly, Category = "RPG|Dialogue", meta = (ClampMin = "0"))
	float CharactersPerSecond = 45.f;

	UPROPERTY(EditDefaultsOnly, Category = "RPG|Dialogue")
	bool bCloseWithEscape = true;

	UPROPERTY(BlueprintReadOnly, Category = "RPG|Dialogue")
	TObjectPtr<URPGDialogueComponent> Dialogue;

	/** After a new line was set up (sounds, animations). */
	UFUNCTION(BlueprintImplementableEvent, Category = "RPG|Dialogue")
	void BP_OnNodeShown();

	virtual void NativeOnInitialized() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

private:
	UFUNCTION()
	void HandleNodeChanged(FName NodeId);

	UFUNCTION()
	void HandleChoiceClicked(int32 ChoiceIndex);

	UFUNCTION()
	void HandleContinueClicked();

	UFUNCTION()
	void HandleCloseClicked();

	void RebuildChoices();
	void UpdateVisibleText(bool bForce);

	FString FullText;
	float TypedCharacters = 0.f;
	int32 ShownCharacters = INDEX_NONE;

	/** Row (0-based, as displayed) -> choice index inside the node. For number keys. */
	TArray<int32> RowToChoiceIndex;
};
