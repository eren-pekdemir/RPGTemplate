#include "Widgets/RPGAttributeBarWidget.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"

#define LOCTEXT_NAMESPACE "RPGAttributeBar"

void URPGAttributeBarWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (!bAutoBindToOwningPawn) return;

    if (APlayerController* PC = GetOwningPlayer())
    {
        // AddUnique: NativeConstruct runs again every time the widget is re-added to the screen.
        PC->OnPossessedPawnChanged.AddUniqueDynamic(this, &ThisClass::HandlePossessedPawnChanged);

        // Sync on subscribe: the pawn may already be possessed.
        BindToPawn(PC->GetPawn());
    }
}

void URPGAttributeBarWidget::NativeDestruct()
{
    if (APlayerController* PC = GetOwningPlayer())
    {
        PC->OnPossessedPawnChanged.RemoveDynamic(this, &ThisClass::HandlePossessedPawnChanged);
    }

    SetAbilitySystem(nullptr);

    Super::NativeDestruct();
}

void URPGAttributeBarWidget::HandlePossessedPawnChanged(APawn* OldPawn, APawn* NewPawn)
{
    BindToPawn(NewPawn);
}

void URPGAttributeBarWidget::BindToPawn(const APawn* Pawn)
{
    SetAbilitySystem(Pawn ? UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Pawn) : nullptr);
}

void URPGAttributeBarWidget::SetAbilitySystem(UAbilitySystemComponent* NewAbilitySystem)
{
    // Unbind from the old ability system.
    if (AbilitySystem.IsValid())
    {
        if (ValueAttribute.IsValid())
        {
            AbilitySystem->GetGameplayAttributeValueChangeDelegate(ValueAttribute).Remove(ValueChangedHandle);
        }
        if (MaxAttribute.IsValid())
        {
            AbilitySystem->GetGameplayAttributeValueChangeDelegate(MaxAttribute).Remove(MaxChangedHandle);
        }
    }
    ValueChangedHandle.Reset();
    MaxChangedHandle.Reset();

    AbilitySystem = NewAbilitySystem;

    // Bind to the new one. nullptr is a command: the bar becomes empty.
    if (NewAbilitySystem)
    {
        if (ValueAttribute.IsValid())
        {
            ValueChangedHandle = NewAbilitySystem->GetGameplayAttributeValueChangeDelegate(ValueAttribute)
                .AddUObject(this, &ThisClass::HandleAttributeChanged);
        }
        if (MaxAttribute.IsValid())
        {
            MaxChangedHandle = NewAbilitySystem->GetGameplayAttributeValueChangeDelegate(MaxAttribute)
                .AddUObject(this, &ThisClass::HandleAttributeChanged);
        }
    }

    Refresh();
}

void URPGAttributeBarWidget::HandleAttributeChanged(const FOnAttributeChangeData& Data)
{
    Refresh();
}

float URPGAttributeBarWidget::GetValue() const
{
    return AbilitySystem.IsValid() && ValueAttribute.IsValid()
        ? AbilitySystem->GetNumericAttribute(ValueAttribute)
        : 0.f;
}

float URPGAttributeBarWidget::GetMaxValue() const
{
    return AbilitySystem.IsValid() && MaxAttribute.IsValid()
        ? AbilitySystem->GetNumericAttribute(MaxAttribute)
        : 0.f;
}

void URPGAttributeBarWidget::Refresh()
{
    const float Value = GetValue();
    const float MaxValue = GetMaxValue();

    if (Bar)
    {
        Bar->SetPercent(MaxValue > 0.f ? FMath::Clamp(Value / MaxValue, 0.f, 1.f) : 0.f);
    }

    if (ValueText)
    {
        FNumberFormattingOptions Options;
        Options.SetMaximumFractionalDigits(0);
        Options.SetUseGrouping(false);

        ValueText->SetText(FText::Format(
            LOCTEXT("ValueFormat", "{0} / {1}"),
            FText::AsNumber(Value, &Options),
            FText::AsNumber(MaxValue, &Options)));
    }

    BP_OnValueChanged(Value, MaxValue);
}

#undef LOCTEXT_NAMESPACE