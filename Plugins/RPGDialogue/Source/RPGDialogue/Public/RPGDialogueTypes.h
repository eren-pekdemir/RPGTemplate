// RPGDialogueTypes.h

#pragma once

#include "CoreMinimal.h"
#include "RPGDialogueTypes.generated.h"

class AActor;
class URPGDialogueAction;
class URPGDialogueComponent;
class URPGDialogueCondition;

/** Who is talking to whom; handed to every condition and action. */
USTRUCT(BlueprintType)
struct RPGDIALOGUE_API FRPGDialogueContext
{
	GENERATED_BODY()

	/** The player's pawn. */
	UPROPERTY(BlueprintReadOnly, Category = "Dialogue")
	TObjectPtr<AActor> Player;

	/** The NPC. */
	UPROPERTY(BlueprintReadOnly, Category = "Dialogue")
	TObjectPtr<AActor> Speaker;

	/** The player's dialogue component (flags, events). */
	UPROPERTY(BlueprintReadOnly, Category = "Dialogue")
	TObjectPtr<URPGDialogueComponent> Dialogue;
};

USTRUCT(BlueprintType)
struct RPGDIALOGUE_API FRPGDialogueChoice
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Choice", meta = (MultiLine = true))
	FText Text;

	/** Node to go to. None = end the conversation. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Choice")
	FName NextNode;

	/** All must pass for the choice to be available. */
	UPROPERTY(EditAnywhere, Instanced, BlueprintReadOnly, Category = "Choice")
	TArray<TObjectPtr<URPGDialogueCondition>> Conditions;

	/** Run in order when the choice is picked. */
	UPROPERTY(EditAnywhere, Instanced, BlueprintReadOnly, Category = "Choice")
	TArray<TObjectPtr<URPGDialogueAction>> Actions;

	/** Hide the choice instead of showing it greyed out when a condition fails. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Choice")
	bool bHideIfUnavailable = false;
};

USTRUCT(BlueprintType)
struct RPGDIALOGUE_API FRPGDialogueNode
{
	GENERATED_BODY()

	/** Unique inside the asset; choices point at it. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Node")
	FName Id;

	/** Empty = the NPC's name. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Node")
	FText SpeakerName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Node", meta = (MultiLine = true))
	FText Text;

	/** Run when the node is entered (give a reward, set a flag). */
	UPROPERTY(EditAnywhere, Instanced, BlueprintReadOnly, Category = "Node")
	TArray<TObjectPtr<URPGDialogueAction>> OnEnterActions;

	/** Empty = a single "Continue" that goes to NextNode. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Node", meta = (TitleProperty = "Text"))
	TArray<FRPGDialogueChoice> Choices;

	/** Used only when there are no choices. None = end the conversation. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Node")
	FName NextNode;
};

/** Alternative entry point, e.g. a different greeting once a flag is set. */
USTRUCT(BlueprintType)
struct RPGDIALOGUE_API FRPGDialogueStart
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Start")
	FName Node;

	UPROPERTY(EditAnywhere, Instanced, BlueprintReadOnly, Category = "Start")
	TArray<TObjectPtr<URPGDialogueCondition>> Conditions;
};

/** A choice as the UI sees it. */
USTRUCT(BlueprintType)
struct RPGDIALOGUE_API FRPGDialogueChoiceView
{
	GENERATED_BODY()

	/** Pass to SelectChoice. */
	UPROPERTY(BlueprintReadOnly, Category = "Dialogue")
	int32 Index = INDEX_NONE;

	UPROPERTY(BlueprintReadOnly, Category = "Dialogue")
	FText Text;

	UPROPERTY(BlueprintReadOnly, Category = "Dialogue")
	bool bAvailable = true;
};
