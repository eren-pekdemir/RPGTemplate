// RPGTradeScreenWidget.cpp

#include "Widgets/RPGTradeScreenWidget.h"

#include "Components/Button.h"
#include "Components/PanelWidget.h"
#include "Components/RPGMerchantComponent.h"
#include "Components/RPGTradeUIComponent.h"
#include "Components/RPGWalletComponent.h"
#include "Components/TextBlock.h"
#include "GameFramework/PlayerController.h"
#include "Interfaces/RPGCurrencyHolder.h"
#include "Interfaces/RPGItemContainer.h"
#include "Items/RPGItemDefinition.h"
#include "Items/RPGItemQuantity.h"
#include "RPGCoreStatics.h"
#include "RPGEconomyLibrary.h"
#include "RPGEconomyUIHelpers.h"
#include "Widgets/RPGTradeEntryWidget.h"

#define LOCTEXT_NAMESPACE "RPGTradeScreen"

namespace
{
	FText GetTradeResultText(ERPGTradeResult Result)
	{
		switch (Result)
		{
		case ERPGTradeResult::Success:              return FText::GetEmpty();
		case ERPGTradeResult::InvalidItem:          return LOCTEXT("Invalid", "Invalid item");
		case ERPGTradeResult::NotInStock:           return LOCTEXT("NotInStock", "Not for sale");
		case ERPGTradeResult::NotEnoughStock:       return LOCTEXT("NotEnoughStock", "Not enough in stock");
		case ERPGTradeResult::CannotAfford:         return LOCTEXT("CannotAfford", "You cannot afford that");
		case ERPGTradeResult::NoRoom:               return LOCTEXT("NoRoom", "Not enough inventory space");
		case ERPGTradeResult::NoContainer:          return LOCTEXT("NoContainer", "No inventory");
		case ERPGTradeResult::NoWallet:             return LOCTEXT("NoWallet", "No wallet");
		case ERPGTradeResult::NotEnoughItems:       return LOCTEXT("NotEnoughItems", "You do not have that many");
		case ERPGTradeResult::ItemNotAccepted:      return LOCTEXT("NotAccepted", "The merchant does not buy that");
		case ERPGTradeResult::MerchantCannotAfford: return LOCTEXT("MerchantPoor", "The merchant cannot afford that");
		}
		return FText::GetEmpty();
	}
}

// ===================================================================== Setup

void URPGTradeScreenWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (CloseButton)
	{
		CloseButton->OnClicked.AddDynamic(this, &ThisClass::HandleCloseClicked);
	}
}

void URPGTradeScreenWidget::NativeDestruct()
{
	SetTrade(nullptr, nullptr);
	Super::NativeDestruct();
}

void URPGTradeScreenWidget::SetTrade(URPGMerchantComponent* InMerchant, AActor* InCustomer)
{
	// Unbind -> assign -> bind -> rebuild
	if (Merchant)
	{
		Merchant->OnStockChanged.RemoveDynamic(this, &ThisClass::HandleStockChanged);
		Merchant->OnItemTraded.RemoveDynamic(this, &ThisClass::HandleItemTraded);
	}
	if (CustomerWallet)
	{
		CustomerWallet->OnCurrencyChanged.RemoveDynamic(this, &ThisClass::HandleCurrencyChanged);
	}

	Merchant = InMerchant;
	Customer = InCustomer;
	CustomerWallet = InCustomer ? InCustomer->FindComponentByClass<URPGWalletComponent>() : nullptr;

	if (Merchant)
	{
		Merchant->OnStockChanged.AddDynamic(this, &ThisClass::HandleStockChanged);
		Merchant->OnItemTraded.AddDynamic(this, &ThisClass::HandleItemTraded);
	}
	if (CustomerWallet)
	{
		CustomerWallet->OnCurrencyChanged.AddDynamic(this, &ThisClass::HandleCurrencyChanged);
	}

	if (MerchantNameText)
	{
		MerchantNameText->SetText(Merchant && !Merchant->DisplayName.IsEmpty()
			? Merchant->DisplayName
			: LOCTEXT("Merchant", "Merchant"));
	}

	MessageTimeLeft = 0.f;
	Rebuild();
}

// ===================================================================== Lists

void URPGTradeScreenWidget::Rebuild()
{
	if (BuyList)
	{
		BuyList->ClearChildren();
	}
	if (SellList)
	{
		SellList->ClearChildren();
	}

	AActor* CustomerActor = Customer.Get();
	if (!Merchant || !CustomerActor || !EntryClass)
	{
		RefreshCurrency();
		RefreshStatus();
		return;
	}

	const FGameplayTag CurrencyTag = Merchant->GetCurrencyTag();
	const int64 Money = URPGCoreStatics::GetCurrency(CustomerActor, CurrencyTag);

	// Merchant stock.
	if (BuyList)
	{
		for (const FRPGMerchantStockEntry& StockEntry : Merchant->Stock)
		{
			if (!StockEntry.Item)
			{
				continue;
			}

			const int64 Price = Merchant->GetBuyPrice(StockEntry.Item);
			URPGTradeEntryWidget* Entry = CreateWidget<URPGTradeEntryWidget>(this, EntryClass);
			Entry->Setup(StockEntry.Item, StockEntry.bInfinite ? INDEX_NONE : StockEntry.Quantity, Price, true, Money >= Price);
			Entry->OnClicked.AddDynamic(this, &ThisClass::HandleEntryClicked);
			BuyList->AddChild(Entry);
		}
	}

	// Customer items the merchant would buy.
	if (SellList)
	{
		if (const IRPGItemContainer* Container = URPGCoreStatics::FindItemContainer(CustomerActor))
		{
			TArray<FRPGItemQuantity> OwnedItems;
			Container->GetItems(OwnedItems);

			for (const FRPGItemQuantity& Owned : OwnedItems)
			{
				if (!Owned.IsValid() || !Merchant->WillBuy(Owned.Item))
				{
					continue;
				}

				URPGTradeEntryWidget* Entry = CreateWidget<URPGTradeEntryWidget>(this, EntryClass);
				Entry->Setup(Owned.Item, Owned.Quantity, Merchant->GetSellPrice(Owned.Item), false, true);
				Entry->OnClicked.AddDynamic(this, &ThisClass::HandleEntryClicked);
				SellList->AddChild(Entry);
			}
		}
	}

	RefreshCurrency();
	RefreshStatus();
}

void URPGTradeScreenWidget::RefreshCurrency()
{
	AActor* CustomerActor = Customer.Get();
	const FGameplayTag CurrencyTag = Merchant ? Merchant->GetCurrencyTag() : RPGEconomyUI::GetDefaultCurrency();

	if (CustomerCurrencyText)
	{
		CustomerCurrencyText->SetText(RPGEconomyUI::FormatCurrency(
			URPGCoreStatics::GetCurrency(CustomerActor, CurrencyTag), CurrencyTag));
	}

	if (MerchantCurrencyText)
	{
		if (Merchant && !Merchant->bInfiniteFunds)
		{
			MerchantCurrencyText->SetText(RPGEconomyUI::FormatCurrency(
				URPGCoreStatics::GetCurrency(Merchant->GetOwner(), CurrencyTag), CurrencyTag));
			MerchantCurrencyText->SetVisibility(ESlateVisibility::HitTestInvisible);
		}
		else
		{
			MerchantCurrencyText->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

void URPGTradeScreenWidget::RefreshStatus()
{
	if (StatusText)
	{
		StatusText->SetText(MessageTimeLeft > 0.f
			? Message
			: LOCTEXT("Hint", "Click: 1   Shift + click: as many as possible"));
	}
}

void URPGTradeScreenWidget::ShowMessage(const FText& InMessage)
{
	Message = InMessage;
	MessageTimeLeft = MessageDuration;
	RefreshStatus();
}

// ===================================================================== Input

void URPGTradeScreenWidget::HandleEntryClicked(URPGTradeEntryWidget* Entry, bool bAll)
{
	AActor* CustomerActor = Customer.Get();
	if (!Merchant || !CustomerActor || !Entry || !Entry->GetItem())
	{
		return;
	}

	const URPGItemDefinition* Item = Entry->GetItem();
	ERPGTradeResult Result;

	if (Entry->IsBuyEntry())
	{
		int32 Quantity = 1;
		if (bAll)
		{
			// As many as: affordable, in stock, and one stack.
			const int64 Price = FMath::Max<int64>(1, Merchant->GetBuyPrice(Item));
			const int64 Affordable = URPGCoreStatics::GetCurrency(CustomerActor, Merchant->GetCurrencyTag()) / Price;
			const int64 InStock = Merchant->GetStockQuantity(Item);
			Quantity = static_cast<int32>(FMath::Clamp<int64>(FMath::Min(Affordable, InStock), 1, FMath::Max(Item->MaxStackSize, 1)));
		}
		Result = Merchant->BuyItem(CustomerActor, Item, Quantity);
	}
	else
	{
		const int32 Quantity = bAll ? FMath::Max(Entry->GetQuantity(), 1) : 1;
		Result = Merchant->SellItem(CustomerActor, Item, Quantity);
	}

	if (Result != ERPGTradeResult::Success)
	{
		ShowMessage(GetTradeResultText(Result));
		BP_OnTradeFailed(Result);
	}
}

void URPGTradeScreenWidget::HandleCloseClicked()
{
	if (APlayerController* PlayerController = GetOwningPlayer())
	{
		if (URPGTradeUIComponent* TradeUI = PlayerController->FindComponentByClass<URPGTradeUIComponent>())
		{
			TradeUI->CloseTrade();
		}
	}
}

FReply URPGTradeScreenWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	// Modal: clicks on empty space must not reach the game.
	Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
	return FReply::Handled();
}

void URPGTradeScreenWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (MessageTimeLeft > 0.f)
	{
		MessageTimeLeft -= InDeltaTime;
		if (MessageTimeLeft <= 0.f)
		{
			RefreshStatus();
		}
	}
}

// ===================================================================== Events

void URPGTradeScreenWidget::HandleStockChanged()
{
	Rebuild();
}

void URPGTradeScreenWidget::HandleItemTraded(const URPGItemDefinition* Item, int32 Quantity, int64 TotalPrice, bool bCustomerBought)
{
	const FText ItemName = Item ? Item->DisplayName : FText::GetEmpty();
	const FText Price = RPGEconomyUI::FormatCurrency(TotalPrice, Merchant ? Merchant->GetCurrencyTag() : FGameplayTag());

	ShowMessage(bCustomerBought
		? FText::Format(LOCTEXT("Bought", "Bought {0} x {1} for {2}"), FText::AsNumber(Quantity), ItemName, Price)
		: FText::Format(LOCTEXT("Sold", "Sold {0} x {1} for {2}"), FText::AsNumber(Quantity), ItemName, Price));

	BP_OnTradeCompleted(Item, Quantity, TotalPrice, bCustomerBought);
	Rebuild();
}

void URPGTradeScreenWidget::HandleCurrencyChanged(FGameplayTag Currency, int64 NewAmount, int64 Delta)
{
	Rebuild();   // affordability colours depend on the balance
}

#undef LOCTEXT_NAMESPACE
