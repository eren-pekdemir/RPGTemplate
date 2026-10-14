#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RPGExperienceBarWidget.generated.h"

class UProgressBar;
class UTextBlock;
class URPGProgressionComponent;

/** Level and XP of the owning player's progression component (on the PlayerState). */
UCLASS(Abstract)
class RPGPROGRESSIONUI_API URPGExperienceBarWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/** nullptr clears the bar. Called automatically from the owning player's PlayerState. */
	UFUNCTION(BlueprintCallable, Category = "RPG|Progression")
	void SetProgression(URPGProgressionComponent* NewProgression);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> ExperienceBar;

	/** "Lv 5" */
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> LevelText;

	/** "40 / 225" */
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> ExperienceText;

	/** Designer hook: level-up flash, sound. */
	UFUNCTION(BlueprintImplementableEvent, Category = "RPG|Progression")
	void BP_OnLevelUp(int32 NewLevel, int32 LevelsGained);

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

private:
	UFUNCTION()
	void HandleExperienceChanged(int32 CurrentExperience, int32 RequiredExperience);

	UFUNCTION()
	void HandleLevelUp(int32 NewLevel, int32 LevelsGained);

	void Refresh();

	TWeakObjectPtr<URPGProgressionComponent> Progression;
};