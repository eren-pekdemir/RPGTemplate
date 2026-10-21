// RPGDialogueScreenWidget.cpp

#include "Widgets/RPGDialogueScreenWidget.h"

#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/PanelWidget.h"
#include "Components/RPGDialogueComponent.h"
#include "Components/TextBlock.h"
#include "Engine/Texture2D.h"
#include "InputCoreTypes.h"
#include "Widgets/RPGDialogueChoiceWidget.h"

void URPGDialogueScreenWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	SetIsFocusable(true);   // receives number / Space / Escape keys

	if (ContinueButton)
	{
		ContinueButton->OnClicked.AddDynamic(this, &ThisClass::HandleContinueClicked);
	}
	if (CloseButton)
	{
		CloseButton->OnClicked.AddDynamic(this, &ThisClass::HandleCloseClicked);
	}
}

void URPGDialogueScreenWidget::NativeDestruct()
{
	SetDialogue(nullptr);
	Super::NativeDestruct();
}

void URPGDialogueScreenWidget::SetDialogue(URPGDialogueComponent* InDialogue)
{
	// Unbind -> assign -> bind -> refresh
	if (Dialogue)
	{
		Dialogue->OnNodeChanged.RemoveDynamic(this, &ThisClass::HandleNodeChanged);
	}

	Dialogue = InDialogue;

	if (Dialogue)
	{
		Dialogue->OnNodeChanged.AddDynamic(this, &ThisClass::HandleNodeChanged);
		Refresh();
	}
}

void URPGDialogueScreenWidget::HandleNodeChanged(FName NodeId)
{
	Refresh();
}

void URPGDialogueScreenWidget::Refresh()
{
	if (!Dialogue || !Dialogue->IsInDialogue())
	{
		return;
	}

	if (SpeakerNameText)
	{
		SpeakerNameText->SetText(Dialogue->GetCurrentSpeakerName());
	}

	if (PortraitImage)
	{
		UTexture2D* Portrait = Dialogue->GetCurrentPortrait().LoadSynchronous();
		if (Portrait)
		{
			PortraitImage->SetBrushFromTexture(Portrait);
		}
		PortraitImage->SetVisibility(Portrait ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
	}

	FullText = Dialogue->GetCurrentText().ToString();
	TypedCharacters = CharactersPerSecond > 0.f ? 0.f : static_cast<float>(FullText.Len());

	RebuildChoices();
	UpdateVisibleText(true);
	BP_OnNodeShown();
}

void URPGDialogueScreenWidget::RebuildChoices()
{
	RowToChoiceIndex.Reset();

	if (!ChoiceList)
	{
		return;
	}
	ChoiceList->ClearChildren();

	const TArray<FRPGDialogueChoiceView> Choices = Dialogue ? Dialogue->GetChoices() : TArray<FRPGDialogueChoiceView>();
	if (!Choices.IsEmpty() && !ChoiceClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("DialogueScreen: ChoiceClass is not set, answers cannot be shown."));
		return;
	}

	for (const FRPGDialogueChoiceView& View : Choices)
	{
		URPGDialogueChoiceWidget* ChoiceWidget = CreateWidget<URPGDialogueChoiceWidget>(this, ChoiceClass);
		ChoiceWidget->Setup(View, RowToChoiceIndex.Num() + 1);
		ChoiceWidget->OnChoiceClicked.AddDynamic(this, &ThisClass::HandleChoiceClicked);
		ChoiceList->AddChild(ChoiceWidget);
		RowToChoiceIndex.Add(View.Index);
	}
}

// ===================================================================== Typewriter

bool URPGDialogueScreenWidget::IsTyping() const
{
	return FMath::FloorToInt(TypedCharacters) < FullText.Len();
}

void URPGDialogueScreenWidget::FinishTyping()
{
	TypedCharacters = static_cast<float>(FullText.Len());
	UpdateVisibleText(false);
}

void URPGDialogueScreenWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (IsTyping())
	{
		TypedCharacters += InDeltaTime * CharactersPerSecond;
		UpdateVisibleText(false);
	}
}

void URPGDialogueScreenWidget::UpdateVisibleText(bool bForce)
{
	const int32 Count = FMath::Clamp(FMath::FloorToInt(TypedCharacters), 0, FullText.Len());
	if (!bForce && Count == ShownCharacters)
	{
		return;
	}
	ShownCharacters = Count;

	if (DialogueText)
	{
		DialogueText->SetText(FText::FromString(FullText.Left(Count)));
	}

	const bool bTyping = Count < FullText.Len();
	const bool bHasChoices = !RowToChoiceIndex.IsEmpty();

	// Answers appear when the line is complete; Hidden keeps the layout from jumping.
	if (ChoiceList)
	{
		ChoiceList->SetVisibility(bTyping ? ESlateVisibility::Hidden : ESlateVisibility::Visible);
	}

	// Continue is also the "skip typing" button.
	if (ContinueButton)
	{
		ContinueButton->SetVisibility(bHasChoices && !bTyping ? ESlateVisibility::Collapsed : ESlateVisibility::Visible);
	}
}

// ===================================================================== Input

void URPGDialogueScreenWidget::HandleChoiceClicked(int32 ChoiceIndex)
{
	if (Dialogue && !IsTyping())
	{
		Dialogue->SelectChoice(ChoiceIndex);
	}
}

void URPGDialogueScreenWidget::HandleContinueClicked()
{
	if (IsTyping())
	{
		FinishTyping();
		return;
	}

	if (Dialogue)
	{
		Dialogue->Continue();
	}
}

void URPGDialogueScreenWidget::HandleCloseClicked()
{
	if (Dialogue)
	{
		Dialogue->EndDialogue();   // the UI component closes the screen on OnDialogueEnded
	}
}

FReply URPGDialogueScreenWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	// Clicking the text box skips the typing. Always handled: clicks must not reach the game (attacks).
	if (IsTyping())
	{
		FinishTyping();
	}
	return FReply::Handled();
}

FReply URPGDialogueScreenWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	const FKey Key = InKeyEvent.GetKey();

	static const FKey NumberKeys[] =
	{
		EKeys::One, EKeys::Two, EKeys::Three, EKeys::Four, EKeys::Five,
		EKeys::Six, EKeys::Seven, EKeys::Eight, EKeys::Nine
	};
	static const FKey NumPadKeys[] =
	{
		EKeys::NumPadOne, EKeys::NumPadTwo, EKeys::NumPadThree, EKeys::NumPadFour, EKeys::NumPadFive,
		EKeys::NumPadSix, EKeys::NumPadSeven, EKeys::NumPadEight, EKeys::NumPadNine
	};

	for (int32 Row = 0; Row < UE_ARRAY_COUNT(NumberKeys); ++Row)
	{
		if (Key == NumberKeys[Row] || Key == NumPadKeys[Row])
		{
			if (RowToChoiceIndex.IsValidIndex(Row))
			{
				HandleChoiceClicked(RowToChoiceIndex[Row]);
			}
			return FReply::Handled();
		}
	}

	if (Key == EKeys::SpaceBar || Key == EKeys::Enter)
	{
		if (IsTyping() || RowToChoiceIndex.IsEmpty())
		{
			HandleContinueClicked();
		}
		return FReply::Handled();
	}

	if (Key == EKeys::Escape && bCloseWithEscape)
	{
		HandleCloseClicked();
		return FReply::Handled();
	}

	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}
