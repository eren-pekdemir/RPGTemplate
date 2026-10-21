// RPGDialogueAsset.h
// A conversation as data: nodes with text, choices, conditions and actions.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "RPGDialogueTypes.h"
#include "RPGDialogueAsset.generated.h"

UCLASS(BlueprintType, Const)
class RPGDIALOGUE_API URPGDialogueAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	/** Used when no conditional start matches. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dialogue")
	FName StartNode;

	/** Checked in order; the first whose conditions all pass is used instead of StartNode. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dialogue", meta = (TitleProperty = "Node"))
	TArray<FRPGDialogueStart> ConditionalStarts;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dialogue", meta = (TitleProperty = "Id"))
	TArray<FRPGDialogueNode> Nodes;

	const FRPGDialogueNode* FindNode(FName NodeId) const;

	FName ChooseStartNode(const FRPGDialogueContext& Context) const;

	/** All conditions pass (null entries are ignored). */
	static bool AreConditionsMet(const TArray<TObjectPtr<URPGDialogueCondition>>& Conditions, const FRPGDialogueContext& Context);

#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif
};
