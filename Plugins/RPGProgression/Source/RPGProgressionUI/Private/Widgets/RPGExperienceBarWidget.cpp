// Fill out your copyright notice in the Description page of Project Settings.

#include "Widgets/RPGExperienceBarWidget.h"

#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/RPGProgressionComponent.h"
#include "GameFramework/PlayerState.h"

#define LOCTEXT_NAMESPACE "RPGExperienceBar"

void URPGExperienceBarWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // Progression lives on the PlayerState, which survives respawns: bind once, stay bound.
    APlayerState* PlayerState = GetOwningPlayerState();
    SetProgression(PlayerState ? PlayerState->FindComponentByClass<URPGProgressionComponent>() : nullptr);
}

void URPGExperienceBarWidget::NativeDestruct()
{
    SetProgression(nullptr);

    Super::NativeDestruct();
}

void URPGExperienceBarWidget::SetProgression(URPGProgressionComponent* NewProgression)
{
    // Unbind from the old component.
    if (Progression.IsValid())
    {
        Progression->OnExperienceChanged.RemoveDynamic(this, &ThisClass::HandleExperienceChanged);
        Progression->OnLevelUp.RemoveDynamic(this, &ThisClass::HandleLevelUp);
    }

    Progression = NewProgression;

    // Bind to the new one. nullptr is a command: the bar becomes empty.
    if (NewProgression)
    {
        NewProgression->OnExperienceChanged.AddDynamic(this, &ThisClass::HandleExperienceChanged);
        NewProgression->OnLevelUp.AddDynamic(this, &ThisClass::HandleLevelUp);
    }

    // Sync on subscribe.
    Refresh();
}

void URPGExperienceBarWidget::HandleExperienceChanged(int32 CurrentExperience, int32 RequiredExperience)
{
    Refresh();
}

void URPGExperienceBarWidget::HandleLevelUp(int32 NewLevel, int32 LevelsGained)
{
    Refresh();
    BP_OnLevelUp(NewLevel, LevelsGained);
}

void URPGExperienceBarWidget::Refresh()
{
    if (!Progression.IsValid())
    {
        if (ExperienceBar) ExperienceBar->SetPercent(0.f);
        if (LevelText) LevelText->SetText(FText::GetEmpty());
        if (ExperienceText) ExperienceText->SetText(FText::GetEmpty());
        return;
    }

    if (ExperienceBar)
    {
        ExperienceBar->SetPercent(Progression->GetExperienceNormalized());
    }

    if (LevelText)
    {
        LevelText->SetText(FText::Format(LOCTEXT("LevelFormat", "Lv {0}"),
            FText::AsNumber(Progression->GetLevel())));
    }

    if (ExperienceText)
    {
        if (Progression->IsMaxLevel())
        {
            ExperienceText->SetText(LOCTEXT("MaxLevel", "MAX"));
        }
        else
        {
            ExperienceText->SetText(FText::Format(LOCTEXT("ExperienceFormat", "{0} / {1}"),
                FText::AsNumber(Progression->GetExperience()),
                FText::AsNumber(Progression->GetExperienceToNextLevel())));
        }
    }
}

#undef LOCTEXT_NAMESPACE