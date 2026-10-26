// RPGTradeUIComponent.cpp

#include "Components/RPGTradeUIComponent.h"

#include "Blueprint/UserWidget.h"
#include "Components/RPGMerchantComponent.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "RPGEconomy.h"
#include "Widgets/RPGTradeScreenWidget.h"

URPGTradeUIComponent::URPGTradeUIComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

APlayerController* URPGTradeUIComponent::GetPlayerController() const
{
	return Cast<APlayerController>(GetOwner());
}

bool URPGTradeUIComponent::OpenTradeFor(AActor* Customer, URPGMerchantComponent* Merchant)
{
	const APawn* Pawn = Cast<APawn>(Customer);
	APlayerController* PlayerController = Pawn ? Cast<APlayerController>(Pawn->GetController()) : nullptr;
	URPGTradeUIComponent* TradeUI = PlayerController ? PlayerController->FindComponentByClass<URPGTradeUIComponent>() : nullptr;

	if (!TradeUI)
	{
		UE_LOG(LogRPGEconomy, Warning, TEXT("OpenTradeFor: %s is not a player pawn with a RPGTradeUIComponent on its PlayerController."),
			*GetNameSafe(Customer));
		return false;
	}
	return TradeUI->OpenTrade(Merchant);
}

bool URPGTradeUIComponent::OpenTrade(URPGMerchantComponent* Merchant)
{
	if (bIsOpen)
	{
		return false;
	}

	APlayerController* PlayerController = GetPlayerController();
	if (!PlayerController || !PlayerController->IsLocalController())
	{
		UE_LOG(LogRPGEconomy, Warning, TEXT("TradeUI: owner is not a local PlayerController (%s)."), *GetNameSafe(GetOwner()));
		return false;
	}

	if (!ScreenWidgetClass || !Merchant)
	{
		UE_LOG(LogRPGEconomy, Warning, TEXT("TradeUI: ScreenWidgetClass or Merchant is missing."));
		return false;
	}

	APawn* Customer = PlayerController->GetPawn();
	if (!Customer || !Merchant->BeginTrade(Customer))
	{
		return false;
	}

	ActiveMerchant = Merchant;
	ActiveMerchant->OnTradeEnded.AddDynamic(this, &ThisClass::HandleTradeEnded);

	if (!ScreenWidget)
	{
		ScreenWidget = CreateWidget<URPGTradeScreenWidget>(PlayerController, ScreenWidgetClass);
	}

	bIsOpen = true;

	ScreenWidget->SetTrade(ActiveMerchant, Customer);
	ScreenWidget->AddToViewport(ZOrder);

	FInputModeGameAndUI InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputMode.SetHideCursorDuringCapture(false);
	PlayerController->SetInputMode(InputMode);
	PlayerController->SetShowMouseCursor(true);
	PlayerController->SetIgnoreLookInput(true);
	PlayerController->SetIgnoreMoveInput(true);

	OnTradeToggled.Broadcast(true);
	return true;
}

void URPGTradeUIComponent::CloseTrade()
{
	if (!bIsOpen)
	{
		return;
	}

	// State first: ending the trade below fires HandleTradeEnded, which must find us closed.
	bIsOpen = false;

	if (ScreenWidget)
	{
		ScreenWidget->SetTrade(nullptr, nullptr);
		ScreenWidget->RemoveFromParent();
	}

	APlayerController* PlayerController = GetPlayerController();
	if (PlayerController)
	{
		PlayerController->SetInputMode(FInputModeGameOnly());
		PlayerController->SetShowMouseCursor(false);
		PlayerController->ResetIgnoreLookInput();
		PlayerController->ResetIgnoreMoveInput();
	}

	if (URPGMerchantComponent* Merchant = ActiveMerchant)
	{
		ActiveMerchant = nullptr;
		Merchant->OnTradeEnded.RemoveDynamic(this, &ThisClass::HandleTradeEnded);

		// End the merchant's session only if it is still with our pawn.
		if (PlayerController && Merchant->GetCustomer() == PlayerController->GetPawn())
		{
			Merchant->EndTrade();
		}
	}

	OnTradeToggled.Broadcast(false);
}

void URPGTradeUIComponent::HandleTradeEnded(AActor* Customer)
{
	// Walked away, merchant destroyed, or another customer took over.
	CloseTrade();
}

void URPGTradeUIComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	CloseTrade();
	Super::EndPlay(EndPlayReason);
}
