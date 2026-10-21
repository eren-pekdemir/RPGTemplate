// RPGDialogueUIComponent.h
// Lives on the PlayerController next to RPGDialogueComponent. Shows the dialogue screen while a conversation runs.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RPGDialogueUIComponent.generated.h"

class APlayerController;
class URPGDialogueComponent;
class URPGDialogueScreenWidget;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRPGOnDialogueToggledSignature, bool, bIsOpen);

UCLASS(ClassGroup = (RPG), meta = (BlueprintSpawnableComponent))
class RPGDIALOGUEUI_API URPGDialogueUIComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	URPGDialogueUIComponent();

	UPROPERTY(EditAnywhere, Category = "RPG|Dialogue")
	TSubclassOf<URPGDialogueScreenWidget> ScreenWidgetClass;

	UPROPERTY(EditAnywhere, Category = "RPG|Dialogue")
	int32 ZOrder = 30;

	/** For game-layer reactions (hide the HUD, interaction prompts). */
	UPROPERTY(BlueprintAssignable, Category = "RPG|Dialogue")
	FRPGOnDialogueToggledSignature OnDialogueToggled;

	UFUNCTION(BlueprintPure, Category = "RPG|Dialogue")
	bool IsDialogueOpen() const { return bIsOpen; }

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	APlayerController* GetPlayerController() const;

	UFUNCTION()
	void HandleDialogueStarted(AActor* Speaker);

	UFUNCTION()
	void HandleDialogueEnded(AActor* Speaker);

	void OpenScreen();
	void CloseScreen();

	UPROPERTY()
	TObjectPtr<URPGDialogueComponent> Dialogue;

	UPROPERTY()
	TObjectPtr<URPGDialogueScreenWidget> ScreenWidget;

	bool bIsOpen = false;
};
