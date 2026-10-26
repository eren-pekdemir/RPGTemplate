// RPGTradeScreenWidget.h
// Merchant stock on one side, the customer's sellable items on the other. Click = 1, Shift+click = as many as possible.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayTagContainer.h"
#include "RPGEconomyTypes.h"
#include "RPGTradeScreenWidget.generated.h"

class UButton;
class UPanelWidget;
class UTextBlock;
class URPGItemDefinition;
class URPGMerchantComponent;
class URPGTradeEntryWidget;
class URPGWalletComponent;

UCLASS(Abstract)
class RPGECONOMYUI_API URPGTradeScreenWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/** nullptr / nullptr clears the screen. Called by URPGTradeUIComponent. */
	UFUNCTION(BlueprintCallable, Category = "RPG|Trade")
	void SetTrade(URPGMerchantComponent* InMerchant, AActor* InCustomer);

protected:
	/** Merchant stock: click to buy. */
	UPROPERTY(BlueprintReadOnly, Category = "RPG|Trade", meta = (BindWidget))
	TObjectPtr<UPanelWidget> BuyList;

	/** Customer items the merchant accepts: click to sell. */
	UPROPERTY(BlueprintReadOnly, Category = "RPG|Trade", meta = (BindWidget))
	TObjectPtr<UPanelWidget> SellList;

	UPROPERTY(EditDefaultsOnly, Category = "RPG|Trade")
	TSubclassOf<URPGTradeEntryWidget> EntryClass;

	UPROPERTY(BlueprintReadOnly, Category = "RPG|Trade", meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> MerchantNameText;

	/** "120 Gold" */
	UPROPERTY(BlueprintReadOnly, Category = "RPG|Trade", meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> CustomerCurrencyText;

	/** Merchant purse; collapsed for merchants with infinite funds. */
	UPROPERTY(BlueprintReadOnly, Category = "RPG|Trade", meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> MerchantCurrencyText;

	UPROPERTY(BlueprintReadOnly, Category = "RPG|Trade", meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> StatusText;

	UPROPERTY(BlueprintReadOnly, Category = "RPG|Trade", meta = (BindWidgetOptional))
	TObjectPtr<UButton> CloseButton;

	UPROPERTY(EditDefaultsOnly, Category = "RPG|Trade", meta = (ClampMin = "0", Units = "s"))
	float MessageDuration = 2.5f;

	/** Designer hooks: coin sound, error buzz. */
	UFUNCTION(BlueprintImplementableEvent, Category = "RPG|Trade")
	void BP_OnTradeCompleted(const URPGItemDefinition* Item, int32 Quantity, int64 TotalPrice, bool bBought);

	UFUNCTION(BlueprintImplementableEvent, Category = "RPG|Trade")
	void BP_OnTradeFailed(ERPGTradeResult Result);

	virtual void NativeOnInitialized() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

private:
	void Rebuild();
	void RefreshCurrency();
	void RefreshStatus();
	void ShowMessage(const FText& InMessage);

	UFUNCTION() void HandleEntryClicked(URPGTradeEntryWidget* Entry, bool bAll);
	UFUNCTION() void HandleStockChanged();
	UFUNCTION() void HandleItemTraded(const URPGItemDefinition* Item, int32 Quantity, int64 TotalPrice, bool bCustomerBought);
	UFUNCTION() void HandleCurrencyChanged(FGameplayTag Currency, int64 NewAmount, int64 Delta);
	UFUNCTION() void HandleCloseClicked();

	UPROPERTY()
	TObjectPtr<URPGMerchantComponent> Merchant;

	UPROPERTY()
	TObjectPtr<URPGWalletComponent> CustomerWallet;

	TWeakObjectPtr<AActor> Customer;

	FText Message;
	float MessageTimeLeft = 0.f;
};
