// RPGReputationEntryWidget.h
// One faction row: icon, name, rank (in its colour), progress to the next rank.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RPGReputationEntryWidget.generated.h"

class UImage;
class UProgressBar;
class UTextBlock;
class URPGFactionDefinition;

UCLASS(Abstract)
class RPGFACTIONUI_API URPGReputationEntryWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void Setup(const URPGFactionDefinition* InFaction, int32 Reputation);

	const URPGFactionDefinition* GetFaction() const { return Faction; }

protected:
	UPROPERTY(BlueprintReadOnly, Category = "RPG|Reputation", meta = (BindWidgetOptional))
	TObjectPtr<UImage> IconImage;

	UPROPERTY(BlueprintReadOnly, Category = "RPG|Reputation", meta = (BindWidget))
	TObjectPtr<UTextBlock> NameText;

	UPROPERTY(BlueprintReadOnly, Category = "RPG|Reputation", meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> RankText;

	/** "120 / 500" towards the next rank, or the plain value at the top rank. */
	UPROPERTY(BlueprintReadOnly, Category = "RPG|Reputation", meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> ValueText;

	/** Progress inside the current rank, in the rank colour. */
	UPROPERTY(BlueprintReadOnly, Category = "RPG|Reputation", meta = (BindWidgetOptional))
	TObjectPtr<UProgressBar> RankProgressBar;

private:
	UPROPERTY()
	TObjectPtr<const URPGFactionDefinition> Faction;
};
