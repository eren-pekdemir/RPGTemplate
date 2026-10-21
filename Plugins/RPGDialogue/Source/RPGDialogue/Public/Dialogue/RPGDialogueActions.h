// RPGDialogueActions.h
// Instanced actions run by choices and nodes. Add your own in C++ or Blueprint (subclass "Blueprint Action").

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "RPGDialogueTypes.h"
#include "UObject/Object.h"
#include "RPGDialogueActions.generated.h"

class URPGItemDefinition;

UCLASS(Abstract, EditInlineNew, DefaultToInstanced, CollapseCategories)
class RPGDIALOGUE_API URPGDialogueAction : public UObject
{
	GENERATED_BODY()

public:
	virtual void Execute(const FRPGDialogueContext& Context) const {}
};

UCLASS(meta = (DisplayName = "Set Flags"))
class RPGDIALOGUE_API URPGDialogueAction_SetFlags : public URPGDialogueAction
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Action")
	FGameplayTagContainer AddFlags;

	UPROPERTY(EditAnywhere, Category = "Action")
	FGameplayTagContainer RemoveFlags;

	virtual void Execute(const FRPGDialogueContext& Context) const override;
};

/** Gives (positive) or takes (negative) items. Items that do not fit are lost: check room with a condition if needed. */
UCLASS(meta = (DisplayName = "Change Item"))
class RPGDIALOGUE_API URPGDialogueAction_ChangeItem : public URPGDialogueAction
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Action")
	TObjectPtr<const URPGItemDefinition> Item;

	/** Positive = give, negative = take. */
	UPROPERTY(EditAnywhere, Category = "Action")
	int32 Quantity = 1;

	virtual void Execute(const FRPGDialogueContext& Context) const override;
};

/** Gives (positive) or takes (negative) currency. Taking fails silently if the player cannot pay: guard with "Has Currency". */
UCLASS(meta = (DisplayName = "Change Currency"))
class RPGDIALOGUE_API URPGDialogueAction_ChangeCurrency : public URPGDialogueAction
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Action", meta = (Categories = "Currency"))
	FGameplayTag Currency;

	UPROPERTY(EditAnywhere, Category = "Action")
	int64 Amount = 10;

	virtual void Execute(const FRPGDialogueContext& Context) const override;
};

UCLASS(meta = (DisplayName = "Change Reputation"))
class RPGDIALOGUE_API URPGDialogueAction_ChangeReputation : public URPGDialogueAction
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Action", meta = (Categories = "Faction"))
	FGameplayTag Faction;

	UPROPERTY(EditAnywhere, Category = "Action")
	int32 Amount = 10;

	virtual void Execute(const FRPGDialogueContext& Context) const override;
};

/**
 * Broadcasts a tag on the player's dialogue component and on the NPC's speaker component.
 * The game layer reacts (open trade, start a quest, give XP). By default it fires after the conversation closed,
 * so a screen it opens is not closed by the dialogue screen.
 */
UCLASS(meta = (DisplayName = "Send Event"))
class RPGDIALOGUE_API URPGDialogueAction_SendEvent : public URPGDialogueAction
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Action", meta = (Categories = "Dialogue.Event"))
	FGameplayTag EventTag;

	UPROPERTY(EditAnywhere, Category = "Action")
	bool bWaitForDialogueEnd = true;

	virtual void Execute(const FRPGDialogueContext& Context) const override;
};

/** Subclass in Blueprint and implement "Receive Execute". */
UCLASS(Abstract, Blueprintable, meta = (DisplayName = "Blueprint Action"))
class RPGDIALOGUE_API URPGDialogueAction_Blueprint : public URPGDialogueAction
{
	GENERATED_BODY()

public:
	virtual void Execute(const FRPGDialogueContext& Context) const override { ReceiveExecute(Context); }

protected:
	UFUNCTION(BlueprintImplementableEvent, Category = "Action")
	void ReceiveExecute(const FRPGDialogueContext& Context) const;
};
