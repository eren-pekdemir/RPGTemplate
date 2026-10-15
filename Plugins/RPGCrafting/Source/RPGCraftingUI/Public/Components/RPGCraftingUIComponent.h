// RPGCraftingUIComponent.h
// Lives on the PlayerController. Opens the crafting screen when a station is used, closes it when the station is left,
// and can be toggled with a key for station-less recipes.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "RPGCraftingUIComponent.generated.h"

class APawn;
class APlayerController;
class URPGCraftingComponent;
class URPGCraftingScreenWidget;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRPGOnCraftingToggledSignature, bool, bIsOpen);

UCLASS(ClassGroup = (RPG), meta = (BlueprintSpawnableComponent))
class RPGCRAFTINGUI_API URPGCraftingUIComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	URPGCraftingUIComponent();

	UPROPERTY(EditAnywhere, Category = "RPG|Crafting")
	TSubclassOf<URPGCraftingScreenWidget> ScreenWidgetClass;

	UPROPERTY(EditAnywhere, Category = "RPG|Crafting")
	int32 ZOrder = 20;

	/** For game-layer reactions (hide interaction prompts, sounds). */
	UPROPERTY(BlueprintAssignable, Category = "RPG|Crafting")
	FRPGOnCraftingToggledSignature OnCraftingToggled;

	UFUNCTION(BlueprintCallable, Category = "RPG|Crafting")
	void OpenCrafting();

	/** Closing the screen also leaves the station (and cancels a craft that needs it). */
	UFUNCTION(BlueprintCallable, Category = "RPG|Crafting")
	void CloseCrafting();

	UFUNCTION(BlueprintCallable, Category = "RPG|Crafting")
	void ToggleCrafting();

	UFUNCTION(BlueprintPure, Category = "RPG|Crafting")
	bool IsCraftingOpen() const { return bIsOpen; }

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	APlayerController* GetPlayerController() const;
	void BindToPawn(APawn* Pawn);

	UFUNCTION()
	void HandlePossessedPawnChanged(APawn* OldPawn, APawn* NewPawn);

	UFUNCTION()
	void HandleActiveStationChanged(FGameplayTag Station);

	UPROPERTY()
	TObjectPtr<URPGCraftingScreenWidget> ScreenWidget;

	UPROPERTY()
	TObjectPtr<URPGCraftingComponent> Crafting;

	bool bIsOpen = false;
};
