// RPGCurrencyWidget.cpp

#include "Widgets/RPGCurrencyWidget.h"

#include "Components/Image.h"
#include "Components/RPGWalletComponent.h"
#include "Components/TextBlock.h"
#include "Engine/Texture2D.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "RPGEconomyLibrary.h"
#include "RPGEconomyUIHelpers.h"

void URPGCurrencyWidget::NativeConstruct()
{
	Super::NativeConstruct();

	const FRPGCurrencyInfo Info = URPGEconomyLibrary::GetCurrencyInfo(Currency);
	if (IconImage)
	{
		if (!Info.Icon.IsNull())
		{
			IconImage->SetBrushFromSoftTexture(Info.Icon);
			IconImage->SetVisibility(ESlateVisibility::HitTestInvisible);
		}
		else
		{
			IconImage->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
	if (AmountText)
	{
		AmountText->SetColorAndOpacity(FSlateColor(Info.Color));
	}

	if (APlayerController* PlayerController = GetOwningPlayer())
	{
		PlayerController->OnPossessedPawnChanged.AddUniqueDynamic(this, &ThisClass::HandlePossessedPawnChanged);
		HandlePossessedPawnChanged(nullptr, PlayerController->GetPawn());   // sync on subscribe
	}
}

void URPGCurrencyWidget::NativeDestruct()
{
	if (APlayerController* PlayerController = GetOwningPlayer())
	{
		PlayerController->OnPossessedPawnChanged.RemoveDynamic(this, &ThisClass::HandlePossessedPawnChanged);
	}

	SetWallet(nullptr);
	Super::NativeDestruct();
}

void URPGCurrencyWidget::HandlePossessedPawnChanged(APawn* OldPawn, APawn* NewPawn)
{
	SetWallet(NewPawn ? NewPawn->FindComponentByClass<URPGWalletComponent>() : nullptr);
}

void URPGCurrencyWidget::SetWallet(URPGWalletComponent* NewWallet)
{
	// Unbind -> assign -> bind -> refresh
	if (Wallet)
	{
		Wallet->OnCurrencyChanged.RemoveDynamic(this, &ThisClass::HandleCurrencyChanged);
	}

	Wallet = NewWallet;

	if (Wallet)
	{
		Wallet->OnCurrencyChanged.AddDynamic(this, &ThisClass::HandleCurrencyChanged);
	}

	Refresh();
}

void URPGCurrencyWidget::HandleCurrencyChanged(FGameplayTag ChangedCurrency, int64 NewAmount, int64 Delta)
{
	if (ChangedCurrency == URPGEconomyLibrary::ResolveCurrency(Currency))
	{
		Refresh();
		BP_OnAmountChanged(NewAmount, Delta);
	}
}

void URPGCurrencyWidget::Refresh()
{
	if (!AmountText)
	{
		return;
	}

	const int64 Amount = Wallet ? Wallet->GetAmount(Currency) : 0;
	AmountText->SetText(bShowCurrencyName
		? RPGEconomyUI::FormatCurrency(Amount, Currency)
		: FText::AsNumber(Amount));
}
