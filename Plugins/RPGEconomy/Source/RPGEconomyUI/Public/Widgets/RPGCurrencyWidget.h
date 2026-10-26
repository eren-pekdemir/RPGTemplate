// RPGCurrencyWidget.h
// HUD: the possessed pawn's amount of one currency ("120 Gold").

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayTagContainer.h"
#include "RPGCurrencyWidget.generated.h"

class APawn;
class UImage;
class UTextBlock;
class URPGWalletComponent;

UCLASS(Abstract)
class RPGECONOMYUI_API URPGCurrencyWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/** Empty = default currency. Set per instance. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RPG|Economy", meta = (Categories = "Currency"))
	FGameplayTag Currency;

	/** Bind manually (e.g. a merchant's purse); nullptr empties the widget. Done automatically for the possessed pawn. */
	UFUNCTION(BlueprintCallable, Category = "RPG|Economy")
	void SetWallet(URPGWalletComponent* NewWallet);

protected:
	UPROPERTY(BlueprintReadOnly, Category = "RPG|Economy", meta = (BindWidget))
	TObjectPtr<UTextBlock> AmountText;

	UPROPERTY(BlueprintReadOnly, Category = "RPG|Economy", meta = (BindWidgetOptional))
	TObjectPtr<UImage> IconImage;

	/** Show the currency name after the number ("120 Gold") or just the number. */
	UPROPERTY(EditAnywhere, Category = "RPG|Economy")
	bool bShowCurrencyName = false;

	/** Designer hook: "+25" popup, coin sound. Delta is 0 on the first refresh. */
	UFUNCTION(BlueprintImplementableEvent, Category = "RPG|Economy")
	void BP_OnAmountChanged(int64 NewAmount, int64 Delta);

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

private:
	void Refresh();

	UFUNCTION()
	void HandlePossessedPawnChanged(APawn* OldPawn, APawn* NewPawn);

	UFUNCTION()
	void HandleCurrencyChanged(FGameplayTag ChangedCurrency, int64 NewAmount, int64 Delta);

	UPROPERTY()
	TObjectPtr<URPGWalletComponent> Wallet;
};
