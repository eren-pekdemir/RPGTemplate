// RPGDialogueChoiceWidget.cpp

#include "Widgets/RPGDialogueChoiceWidget.h"

#include "Components/Button.h"
#include "Components/ButtonSlot.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/TextBlock.h"

void URPGDialogueChoiceWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (ChoiceButton)
	{
		ChoiceButton->OnClicked.AddDynamic(this, &ThisClass::HandleClicked);

		// A button centres its content by default; answers of different lengths then start at different x.
		if (UWidget* Content = ChoiceButton->GetContent())
		{
			if (UButtonSlot* ContentSlot = Cast<UButtonSlot>(Content->Slot))
			{
				// Left means "fill and read from the left": the row spans the button so the text can use its width.
				ContentSlot->SetHorizontalAlignment(ContentAlignment == HAlign_Left ? HAlign_Fill : ContentAlignment.GetValue());
			}
		}
	}

	if (ChoiceText && ContentAlignment == HAlign_Left)
	{
		ChoiceText->SetJustification(ETextJustify::Left);

		// An auto-sized slot gives a wrapping text no width of its own, so long answers broke after a few
		// words. Let the text take the rest of the row; it then wraps only at the button's edge.
		if (UHorizontalBoxSlot* TextSlot = Cast<UHorizontalBoxSlot>(ChoiceText->Slot))
		{
			TextSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
		}
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
