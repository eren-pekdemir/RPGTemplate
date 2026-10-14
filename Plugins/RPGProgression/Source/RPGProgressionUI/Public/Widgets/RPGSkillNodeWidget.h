#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RPGSkillNodeWidget.generated.h"

class UImage;
class UTextBlock;
class URPGSkillDefinition;
class URPGSkillNodeWidget;

UENUM(BlueprintType)
enum class ERPGSkillNodeState : uint8
{
    Locked,
    Available,
    Unlocked
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRPGOnSkillNodeClickedSignature, URPGSkillNodeWidget*, Node);

/**
 * One skill on the tree screen. It only displays and reports clicks;
 * the tree decides the state and what a click does.
 */
UCLASS(Abstract)
class RPGPROGRESSIONUI_API URPGSkillNodeWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintAssignable, Category = "RPG|Skill Node")
    FRPGOnSkillNodeClickedSignature OnClicked;

    UFUNCTION(BlueprintCallable, Category = "RPG|Skill Node")
    void SetSkill(const URPGSkillDefinition* NewSkill);

    UFUNCTION(BlueprintCallable, Category = "RPG|Skill Node")
    void SetState(ERPGSkillNodeState NewState);

    UFUNCTION(BlueprintPure, Category = "RPG|Skill Node")
    const URPGSkillDefinition* GetSkill() const { return Skill; }

    UFUNCTION(BlueprintPure, Category = "RPG|Skill Node")
    ERPGSkillNodeState GetState() const { return State; }

protected:
protected:
    UPROPERTY(BlueprintReadOnly, Category = "RPG|Skill Node", meta = (BindWidget))
    TObjectPtr<UImage> IconImage;

    UPROPERTY(BlueprintReadOnly, Category = "RPG|Skill Node", meta = (BindWidgetOptional))
    TObjectPtr<UTextBlock> NameText;

    /** Designer hook: colours, frame, glow per state. */
    UFUNCTION(BlueprintImplementableEvent, Category = "RPG|Skill Node")
    void BP_OnStateChanged(ERPGSkillNodeState NewState);

    UFUNCTION(BlueprintImplementableEvent, Category = "RPG|Skill Node")
    void BP_OnHoverChanged(bool bHovered);

    virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
    virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
    virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;

private:
    /** Name, description, cost and required level, shown on hover. */
    void UpdateTooltip();

    UPROPERTY()
    TObjectPtr<const URPGSkillDefinition> Skill;

    ERPGSkillNodeState State = ERPGSkillNodeState::Locked;
};