// RPGDialogueConditions.h
// Instanced conditions for choices and alternative starts. Add your own in C++ or Blueprint (subclass "Blueprint Condition").

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "RPGDialogueTypes.h"
#include "UObject/Object.h"
#include "RPGDialogueConditions.generated.h"

class URPGItemDefinition;

UCLASS(Abstract, EditInlineNew, DefaultToInstanced, CollapseCategories)
class RPGDIALOGUE_API URPGDialogueCondition : public UObject
{
	GENERATED_BODY()

public:
	/** Pass when the check FAILS ("does not have the flag"). */
	UPROPERTY(EditAnywhere, Category = "Condition")
	bool bInvert = false;

	bool Evaluate(const FRPGDialogueContext& Context) const { return IsMet(Context) != bInvert; }

protected:
	virtual bool IsMet(const FRPGDialogueContext& Context) const { return true; }
};

/** The player has dialogue flags (set by "Set Flags" actions). */
UCLASS(meta = (DisplayName = "Has Flags"))
class RPGDIALOGUE_API URPGDialogueCondition_HasFlags : public URPGDialogueCondition
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Condition")
	FGameplayTagContainer Flags;

	/** All flags (true) or any of them (false). */
	UPROPERTY(EditAnywhere, Category = "Condition")
	bool bRequireAll = true;

protected:
	virtual bool IsMet(const FRPGDialogueContext& Context) const override;
};

/** The player's pawn owns gameplay tags: skills (Skill.Smithing), reputation ranks, flags, anything. */
UCLASS(meta = (DisplayName = "Has Tags"))
class RPGDIALOGUE_API URPGDialogueCondition_HasTags : public URPGDialogueCondition
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Condition")
	FGameplayTagContainer Tags;

	UPROPERTY(EditAnywhere, Category = "Condition")
	bool bRequireAll = true;

protected:
	virtual bool IsMet(const FRPGDialogueContext& Context) const override;
};

UCLASS(meta = (DisplayName = "Has Item"))
class RPGDIALOGUE_API URPGDialogueCondition_HasItem : public URPGDialogueCondition
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Condition")
	TObjectPtr<const URPGItemDefinition> Item;

	UPROPERTY(EditAnywhere, Category = "Condition", meta = (ClampMin = "1"))
	int32 Quantity = 1;

protected:
	virtual bool IsMet(const FRPGDialogueContext& Context) const override;
};

UCLASS(meta = (DisplayName = "Has Currency"))
class RPGDIALOGUE_API URPGDialogueCondition_HasCurrency : public URPGDialogueCondition
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Condition", meta = (Categories = "Currency"))
	FGameplayTag Currency;

	UPROPERTY(EditAnywhere, Category = "Condition", meta = (ClampMin = "0"))
	int64 Amount = 10;

protected:
	virtual bool IsMet(const FRPGDialogueContext& Context) const override;
};

UCLASS(meta = (DisplayName = "Reputation At Least"))
class RPGDIALOGUE_API URPGDialogueCondition_Reputation : public URPGDialogueCondition
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Condition", meta = (Categories = "Faction"))
	FGameplayTag Faction;

	UPROPERTY(EditAnywhere, Category = "Condition")
	int32 MinReputation = 0;

protected:
	virtual bool IsMet(const FRPGDialogueContext& Context) const override;
};

/** Subclass in Blueprint and implement "Receive Is Met" for game-specific checks. */
UCLASS(Abstract, Blueprintable, meta = (DisplayName = "Blueprint Condition"))
class RPGDIALOGUE_API URPGDialogueCondition_Blueprint : public URPGDialogueCondition
{
	GENERATED_BODY()

protected:
	UFUNCTION(BlueprintImplementableEvent, Category = "Condition")
	bool ReceiveIsMet(const FRPGDialogueContext& Context) const;

	virtual bool IsMet(const FRPGDialogueContext& Context) const override { return ReceiveIsMet(Context); }
};
