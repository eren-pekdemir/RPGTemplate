#include "Widgets/RPGSkillNodeWidget.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "InputCoreTypes.h"
#include "Skills/RPGSkillDefinition.h"

#define LOCTEXT_NAMESPACE "RPGSkillNode"

void URPGSkillNodeWidget::SetSkill(const URPGSkillDefinition* NewSkill)
{
    Skill = NewSkill;

    // Every visual state is set explicitly: the widget may be reused for another skill.
    if (IconImage)
    {
        if (Skill && !Skill->Icon.IsNull())
        {
            IconImage->SetBrushFromSoftTexture(Skill->Icon);
            IconImage->SetVisibility(ESlateVisibility::HitTestInvisible);
        }
        else
        {
            IconImage->SetVisibility(ESlateVisibility::Hidden);
        }
    }

    if (NameText)
    {
        NameText->SetText(Skill ? Skill->DisplayName : FText::GetEmpty());
    }

    UpdateTooltip();
}

void URPGSkillNodeWidget::SetState(ERPGSkillNodeState NewState)
{
    State = NewState;
    BP_OnStateChanged(State);
}

void URPGSkillNodeWidget::UpdateTooltip()
{
    if (!Skill)
    {
        SetToolTipText(FText::GetEmpty());
        return;
    }

    // "Toughness
    //  +25 max health.
    //  Cost: 1  ·  Requires level 2"
    SetToolTipText(FText::Format(
        LOCTEXT("TooltipFormat", "{0}\n{1}\nCost: {2}  ·  Requires level {3}"),
        Skill->DisplayName,
        Skill->Description,
        FText::AsNumber(Skill->SkillPointCost),
        FText::AsNumber(Skill->RequiredLevel)));
}

FReply URPGSkillNodeWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton && Skill)
    {
        OnClicked.Broadcast(this);
        return FReply::Handled();
    }

    return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

void URPGSkillNodeWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
    BP_OnHoverChanged(true);
}

void URPGSkillNodeWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
    Super::NativeOnMouseLeave(InMouseEvent);
    BP_OnHoverChanged(false);
}

#undef LOCTEXT_NAMESPACE