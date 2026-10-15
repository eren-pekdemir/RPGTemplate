#include "Widgets/RPGItemNotificationEntryWidget.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Items/RPGItemDefinition.h"
#include "RPGCoreStatics.h"

#define LOCTEXT_NAMESPACE "RPGItemNotification"

void URPGItemNotificationEntryWidget::Setup(const URPGItemDefinition* InItem, int32 InQuantity)
{
	Item = InItem;
	Quantity = InQuantity;
	Age = 0.f;
	SetRenderOpacity(1.f);

	if (MessageText)
	{
		MessageText->SetColorAndOpacity(FSlateColor(URPGCoreStatics::GetItemRarityInfo(Item).Color));
	}

	if (IconImage)
	{
		if (Item && !Item->Icon.IsNull())
		{
			IconImage->SetBrushFromSoftTexture(Item->Icon);
			IconImage->SetVisibility(ESlateVisibility::HitTestInvisible);
		}
		else
		{
			IconImage->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	UpdateText();
}

void URPGItemNotificationEntryWidget::AddQuantity(int32 Amount)
{
	Quantity += Amount;
	Age = 0.f;
	SetRenderOpacity(1.f);
	UpdateText();
	BP_OnQuantityChanged(Quantity);
}

void URPGItemNotificationEntryWidget::UpdateText()
{
	if (MessageText && Item)
	{
		MessageText->SetText(FText::Format(LOCTEXT("Received", "+{0} {1}"), FText::AsNumber(Quantity), Item->DisplayName));
	}
}

void URPGItemNotificationEntryWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	Age += InDeltaTime;
	const float Remaining = DisplayTime - Age;

	if (Remaining <= 0.f)
	{
		RemoveFromParent();
		return;
	}

	if (FadeTime > 0.f && Remaining < FadeTime)
	{
		SetRenderOpacity(Remaining / FadeTime);
	}
}

#undef LOCTEXT_NAMESPACE