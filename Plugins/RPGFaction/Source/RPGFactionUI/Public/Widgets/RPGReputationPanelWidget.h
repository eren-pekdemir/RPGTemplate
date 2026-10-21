// RPGReputationPanelWidget.h
// List of factions with the owning player's standing. Display only (no input mode change needed).

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayTagContainer.h"
#include "RPGReputationPanelWidget.generated.h"

class UPanelWidget;
class URPGReputationComponent;
class URPGReputationEntryWidget;

UCLASS(Abstract)
class RPGFACTIONUI_API URPGReputationPanelWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/** Bound automatically to the owning player's PlayerState; nullptr empties the panel. */
	UFUNCTION(BlueprintCallable, Category = "RPG|Reputation")
	void SetReputation(URPGReputationComponent* NewReputation);

protected:
	UPROPERTY(BlueprintReadOnly, Category = "RPG|Reputation", meta = (BindWidget))
	TObjectPtr<UPanelWidget> FactionList;

	UPROPERTY(EditDefaultsOnly, Category = "RPG|Reputation")
	TSubclassOf<URPGReputationEntryWidget> EntryClass;

	/** Designer hook: toast "+25 Village". Only fires while the panel exists. */
	UFUNCTION(BlueprintImplementableEvent, Category = "RPG|Reputation")
	void BP_OnReputationChanged(FGameplayTag Faction, int32 NewReputation, int32 Delta);

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

private:
	void Rebuild();

	UFUNCTION()
	void HandleReputationChanged(FGameplayTag Faction, int32 NewReputation, int32 Delta);

	UPROPERTY()
	TObjectPtr<URPGReputationComponent> Reputation;
};
