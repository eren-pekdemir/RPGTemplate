// RPGDialogueChoiceWidget.cpp

#include "Widgets/RPGDialogueChoiceWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"

void URPGDialogueChoiceWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (ChoiceButton)
	{
		ChoiceButton->OnClicked.AddDynamic(this, &ThisClass::HandleClicked);
	}
}

void URPGDialogueChoiceWidget::Setup(const FRPGDialogueChoiceView& InView, int32 DisplayNumber)
{
	View = InView;

	if (ChoiceText)
	{
		ChoiceText->SetText(View.Text);
		ChoiceText->SetColorAndOpacity(View.bAvailable ? AvailableColor : UnavailableColor);
	}

	if (NumberText)
	{
		NumberText->SetText(FText::Format(NSLOCTEXT("RPGDialogueChoice", "Number", "{0}."), FText::AsNumber(DisplayNumber)));
	}

	if (ChoiceButton)
	{
		ChoiceButton->SetIsEnabled(View.bAvailable);
	}

	BP_OnSetup(View);
}

void URPGDialogueChoiceWidget::HandleClicked()
{
	if (View.bAvailable)
	{
		OnChoiceClicked.Broadcast(View.Index);
	}
}
