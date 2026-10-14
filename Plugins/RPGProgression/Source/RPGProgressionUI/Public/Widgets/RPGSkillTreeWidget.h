#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RPGProgressionTypes.h"
#include "RPGSkillTreeWidget.generated.h"

class UCanvasPanel;
class UTextBlock;
class URPGProgressionComponent;
class URPGSkillDefinition;
class URPGSkillNodeWidget;

/**
 * Lays out the progression component's skill tree, colours each node by state,
 * draws prerequisite lines and unlocks skills on click.
 */
UCLASS(Abstract)
class RPGPROGRESSIONUI_API URPGSkillTreeWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    /** nullptr clears the screen. Called automatically from the owning player state on construct. */
    UFUNCTION(BlueprintCallable, Category = "RPG|Skill Tree")
    void SetProgression(URPGProgressionComponent* NewProgression);

protected:
    UPROPERTY(BlueprintReadOnly, Category = "RPG|Skill Tree", meta = (BindWidget))
    TObjectPtr<UCanvasPanel> NodeCanvas;

    UPROPERTY(BlueprintReadOnly, Category = "RPG|Skill Tree", meta = (BindWidgetOptional))
    TObjectPtr<UTextBlock> SkillPointsText;

    UPROPERTY(EditDefaultsOnly, Category = "RPG|Skill Tree")
    TSubclassOf<URPGSkillNodeWidget> NodeClass;

    /** Line colour when the prerequisite is unlocked. */
    UPROPERTY(EditDefaultsOnly, Category = "RPG|Skill Tree|Lines")
    FLinearColor ActiveLineColor = FLinearColor(0.1f, 0.6f, 0.2f, 1.f);

    UPROPERTY(EditDefaultsOnly, Category = "RPG|Skill Tree|Lines")
    FLinearColor InactiveLineColor = FLinearColor(0.3f, 0.3f, 0.3f, 0.6f);

    UPROPERTY(EditDefaultsOnly, Category = "RPG|Skill Tree|Lines", meta = (ClampMin = "1"))
    float LineThickness = 3.f;

    /** Feedback hook: sound, shake, message ("Not enough points"). */
    UFUNCTION(BlueprintImplementableEvent, Category = "RPG|Skill Tree")
    void BP_OnUnlockFailed(URPGSkillNodeWidget* Node, ERPGSkillUnlockResult Result);

    /** Fires for any unlock (click or otherwise): sound, flash. */
    UFUNCTION(BlueprintImplementableEvent, Category = "RPG|Skill Tree")
    void BP_OnSkillUnlocked(URPGSkillNodeWidget* Node);

    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;
    virtual int32 NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry,
        const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements,
        int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;
    virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

private:
    void RebuildNodes();
    void RefreshStates();
    URPGSkillNodeWidget* FindNode(const URPGSkillDefinition* Skill) const;

    UFUNCTION()
    void HandleNodeClicked(URPGSkillNodeWidget* Node);

    UFUNCTION()
    void HandleSkillUnlocked(const URPGSkillDefinition* Skill);

    UFUNCTION()
    void HandleSkillPointsChanged(int32 SkillPoints);

    UFUNCTION()
    void HandleLevelUp(int32 NewLevel, int32 LevelsGained);

    UPROPERTY()
    TObjectPtr<URPGProgressionComponent> Progression;

    UPROPERTY()
    TArray<TObjectPtr<URPGSkillNodeWidget>> NodeWidgets;
};