// RPGTradeUIComponent.h
// Lives on the PlayerController. Opens the trade screen for a merchant and closes it when the trade ends.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RPGTradeUIComponent.generated.h"

class APlayerController;
class URPGMerchantComponent;
class URPGTradeScreenWidget;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRPGOnTradeToggledSignature, bool, bIsOpen);

UCLASS(ClassGroup = (RPG), meta = (BlueprintSpawnableComponent))
class RPGECONOMYUI_API URPGTradeUIComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	URPGTradeUIComponent();

	UPROPERTY(EditAnywhere, Category = "RPG|Trade")
	TSubclassOf<URPGTradeScreenWidget> ScreenWidgetClass;

	UPROPERTY(EditAnywhere, Category = "RPG|Trade")
	int32 ZOrder = 20;

	UPROPERTY(BlueprintAssignable, Category = "RPG|Trade")
	FRPGOnTradeToggledSignature OnTradeToggled;

	/** Starts a trade between the possessed pawn and the merchant and shows the screen. */
	UFUNCTION(BlueprintCallable, Category = "RPG|Trade")
	bool OpenTrade(URPGMerchantComponent* Merchant);

	/** Closes the screen and ends the trade. */
	UFUNCTION(BlueprintCallable, Category = "RPG|Trade")
	void CloseTrade();

	UFUNCTION(BlueprintPure, Category = "RPG|Trade")
	bool IsTradeOpen() const { return bIsOpen; }

	/**
	 * Game-layer helper for an interaction event: finds Customer's PlayerController and its trade UI,
	 * then opens the trade. False if Customer is not a player pawn.
	 */
	UFUNCTION(BlueprintCallable, Category = "RPG|Trade")
	static bool OpenTradeFor(AActor* Customer, URPGMerchantComponent* Merchant);

protected:
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	APlayerController* GetPlayerController() const;

	UFUNCTION()
	void HandleTradeEnded(AActor* Customer);

	UPROPERTY()
	TObjectPtr<URPGTradeScreenWidget> ScreenWidget;

	UPROPERTY()
	TObjectPtr<URPGMerchantComponent> ActiveMerchant;

	bool bIsOpen = false;
};
