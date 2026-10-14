#include "Widgets/RPGSkillTreeWidget.h"

#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/RPGProgressionComponent.h"
#include "Components/TextBlock.h"
#include "GameFramework/PlayerState.h"
#include "Rendering/DrawElements.h"
#include "RPGProgression.h"
#include "Skills/RPGSkillDefinition.h"
#include "Skills/RPGSkillTreeDefinition.h"
#include "Widgets/RPGSkillNodeWidget.h"

#define LOCTEXT_NAMESPACE "RPGSkillTree"

namespace
{
    /** Node rectangle in the tree widget's local space. */
    bool GetNodeRect(const URPGSkillNodeWidget* Node, const FGeometry& Space, FVector2D& OutCenter, FVector2D& OutHalf)
    {
        const FGeometry& NodeGeometry = Node->GetCachedGeometry();
        const FVector2D TopLeft = Space.AbsoluteToLocal(NodeGeometry.GetAbsolutePositionAtCoordinates(FVector2D(0.0, 0.0)));
        const FVector2D BottomRight = Space.AbsoluteToLocal(NodeGeometry.GetAbsolutePositionAtCoordinates(FVector2D(1.0, 1.0)));

        OutCenter = (TopLeft + BottomRight) * 0.5;
        OutHalf = (BottomRight - TopLeft) * 0.5;
        return !OutHalf.IsNearlyZero();
    }

    /** Walks from the centre along Dir until it hits the rectangle's border. */
    FVector2D EdgePoint(const FVector2D& Center, const FVector2D& Half, const FVector2D& Dir)
    {
        const double ScaleX = FMath::Abs(Dir.X) > KINDA_SMALL_NUMBER ? Half.X / FMath::Abs(Dir.X) : BIG_NUMBER;
        const double ScaleY = FMath::Abs(Dir.Y) > KINDA_SMALL_NUMBER ? Half.Y / FMath::Abs(Dir.Y) : BIG_NUMBER;
        return Center + Dir * FMath::Min(ScaleX, ScaleY);
    }
}

void URPGSkillTreeWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (!Progression)
    {
        if (const APlayerState* PlayerState = GetOwningPlayerState())
        {
            SetProgression(PlayerState->FindComponentByClass<URPGProgressionComponent>());
        }
    }
}

void URPGSkillTreeWidget::NativeDestruct()
{
    SetProgression(nullptr);
    Super::NativeDestruct();
}

void URPGSkillTreeWidget::SetProgression(URPGProgressionComponent* NewProgression)
{
    // Unbind -> assign -> bind -> refresh
    if (Progression)
    {
        Progression->OnSkillUnlocked.RemoveDynamic(this, &ThisClass::HandleSkillUnlocked);
        Progression->OnSkillPointsChanged.RemoveDynamic(this, &ThisClass::HandleSkillPointsChanged);
        Progression->OnLevelUp.RemoveDynamic(this, &ThisClass::HandleLevelUp);
    }

    Progression = NewProgression;

    if (Progression)
    {
        Progression->OnSkillUnlocked.AddDynamic(this, &ThisClass::HandleSkillUnlocked);
        Progression->OnSkillPointsChanged.AddDynamic(this, &ThisClass::HandleSkillPointsChanged);
        Progression->OnLevelUp.AddDynamic(this, &ThisClass::HandleLevelUp);
    }

    RebuildNodes();
    RefreshStates();
}

void URPGSkillTreeWidget::RebuildNodes()
{
    NodeWidgets.Reset();

    if (!NodeCanvas)
    {
        return;
    }

    NodeCanvas->ClearChildren();

    const URPGSkillTreeDefinition* Tree = Progression ? Progression->GetSkillTree() : nullptr;
    if (!Tree)
    {
        return;
    }

    if (!NodeClass)
    {
        UE_LOG(LogRPGProgression, Warning, TEXT("%s: NodeClass is not set."), *GetName());
        return;
    }

    for (const FRPGSkillTreeNode& TreeNode : Tree->Nodes)
    {
        if (!TreeNode.Skill)
        {
            continue;
        }

        URPGSkillNodeWidget* Node = CreateWidget<URPGSkillNodeWidget>(this, NodeClass);
        Node->SetSkill(TreeNode.Skill);
        Node->OnClicked.AddDynamic(this, &ThisClass::HandleNodeClicked);

        // "CanvasSlot", not "Slot": UWidget already has a member called Slot.
        if (UCanvasPanelSlot* CanvasSlot = NodeCanvas->AddChildToCanvas(Node))
        {
            CanvasSlot->SetAutoSize(true);
            CanvasSlot->SetPosition(TreeNode.Position);
        }

        NodeWidgets.Add(Node);
    }
}

void URPGSkillTreeWidget::RefreshStates()
{
    if (SkillPointsText)
    {
        SkillPointsText->SetText(Progression
            ? FText::Format(LOCTEXT("SkillPoints", "Skill Points: {0}"), FText::AsNumber(Progression->GetSkillPoints()))
            : FText::GetEmpty());
    }

    if (!Progression)
    {
        return;
    }

    for (URPGSkillNodeWidget* Node : NodeWidgets)
    {
        const URPGSkillDefinition* Skill = Node->GetSkill();

        ERPGSkillNodeState NodeState = ERPGSkillNodeState::Locked;
        if (Progression->IsSkillUnlocked(Skill))
        {
            NodeState = ERPGSkillNodeState::Unlocked;
        }
        else if (Progression->CanUnlockSkill(Skill) == ERPGSkillUnlockResult::Success)
        {
            NodeState = ERPGSkillNodeState::Available;
        }

        Node->SetState(NodeState);
    }
}

URPGSkillNodeWidget* URPGSkillTreeWidget::FindNode(const URPGSkillDefinition* Skill) const
{
    for (URPGSkillNodeWidget* Node : NodeWidgets)
    {
        if (Node && Node->GetSkill() == Skill)
        {
            return Node;
        }
    }
    return nullptr;
}

void URPGSkillTreeWidget::HandleNodeClicked(URPGSkillNodeWidget* Node)
{
    if (!Progression || !Node)
    {
        return;
    }

    // On success the component broadcasts OnSkillUnlocked -> HandleSkillUnlocked refreshes.
    const ERPGSkillUnlockResult Result = Progression->UnlockSkill(Node->GetSkill());
    if (Result != ERPGSkillUnlockResult::Success)
    {
        BP_OnUnlockFailed(Node, Result);
    }
}

void URPGSkillTreeWidget::HandleSkillUnlocked(const URPGSkillDefinition* Skill)
{
    RefreshStates();

    if (URPGSkillNodeWidget* Node = FindNode(Skill))
    {
        BP_OnSkillUnlocked(Node);
    }
}

void URPGSkillTreeWidget::HandleSkillPointsChanged(int32 SkillPoints)
{
    RefreshStates();
}

void URPGSkillTreeWidget::HandleLevelUp(int32 NewLevel, int32 LevelsGained)
{
    RefreshStates();
}

FReply URPGSkillTreeWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    // Modal screen: clicks on empty space must not reach the game (e.g. trigger an attack).
    Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
    return FReply::Handled();
}

int32 URPGSkillTreeWidget::NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry,
    const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements,
    int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
    LayerId = Super::NativePaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);

    if (!Progression)
    {
        return LayerId;
    }

    const int32 LineLayer = LayerId + 1;

    for (const URPGSkillNodeWidget* Node : NodeWidgets)
    {
        const URPGSkillDefinition* Skill = Node ? Node->GetSkill() : nullptr;

        FVector2D ToCenter, ToHalf;
        if (!Skill || !GetNodeRect(Node, AllottedGeometry, ToCenter, ToHalf))
        {
            continue;
        }

        for (const URPGSkillDefinition* Prerequisite : Skill->Prerequisites)
        {
            const URPGSkillNodeWidget* FromNode = FindNode(Prerequisite);

            FVector2D FromCenter, FromHalf;
            if (!FromNode || !GetNodeRect(FromNode, AllottedGeometry, FromCenter, FromHalf))
            {
                continue; // prerequisite is not on this tree
            }

            const FVector2D Dir = (ToCenter - FromCenter).GetSafeNormal();
            if (Dir.IsNearlyZero())
            {
                continue;
            }

            // Edge to edge, so the line never crosses the icons.
            TArray<FVector2f> Points;
            Points.Add(FVector2f(EdgePoint(FromCenter, FromHalf, Dir)));
            Points.Add(FVector2f(EdgePoint(ToCenter, ToHalf, -Dir)));

            const FLinearColor Color = Progression->IsSkillUnlocked(Prerequisite) ? ActiveLineColor : InactiveLineColor;

            FSlateDrawElement::MakeLines(OutDrawElements, LineLayer, AllottedGeometry.ToPaintGeometry(), Points,
                ESlateDrawEffect::None, Color * InWidgetStyle.GetColorAndOpacityTint(), true, LineThickness);
        }
    }

    return LineLayer;
}

#undef LOCTEXT_NAMESPACE