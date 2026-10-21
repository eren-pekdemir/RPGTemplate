// RPGDialogueAsset.cpp

#include "Dialogue/RPGDialogueAsset.h"

#include "Dialogue/RPGDialogueActions.h"
#include "Dialogue/RPGDialogueConditions.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif

#define LOCTEXT_NAMESPACE "RPGDialogueAsset"

const FRPGDialogueNode* URPGDialogueAsset::FindNode(FName NodeId) const
{
	if (NodeId.IsNone())
	{
		return nullptr;
	}
	return Nodes.FindByPredicate([NodeId](const FRPGDialogueNode& Node) { return Node.Id == NodeId; });
}

bool URPGDialogueAsset::AreConditionsMet(const TArray<TObjectPtr<URPGDialogueCondition>>& Conditions, const FRPGDialogueContext& Context)
{
	for (const URPGDialogueCondition* Condition : Conditions)
	{
		if (Condition && !Condition->Evaluate(Context))
		{
			return false;
		}
	}
	return true;
}

FName URPGDialogueAsset::ChooseStartNode(const FRPGDialogueContext& Context) const
{
	for (const FRPGDialogueStart& Start : ConditionalStarts)
	{
		if (!Start.Node.IsNone() && AreConditionsMet(Start.Conditions, Context))
		{
			return Start.Node;
		}
	}
	return StartNode;
}

#if WITH_EDITOR

EDataValidationResult URPGDialogueAsset::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = CombineDataValidationResults(Super::IsDataValid(Context), EDataValidationResult::Valid);

	auto Error = [&Context, &Result](const FText& Message)
	{
		Context.AddError(Message);
		Result = EDataValidationResult::Invalid;
	};

	// Ids: present and unique.
	TSet<FName> Ids;
	for (int32 Index = 0; Index < Nodes.Num(); ++Index)
	{
		const FName Id = Nodes[Index].Id;
		if (Id.IsNone())
		{
			Error(FText::Format(LOCTEXT("NoId", "Node [{0}] has no Id."), FText::AsNumber(Index)));
			continue;
		}

		bool bAlreadyInSet = false;
		Ids.Add(Id, &bAlreadyInSet);
		if (bAlreadyInSet)
		{
			Error(FText::Format(LOCTEXT("DuplicateId", "Node Id '{0}' is used twice."), FText::FromName(Id)));
		}
	}

	auto CheckReference = [&](FName Target, const FText& Where)
	{
		if (!Target.IsNone() && !Ids.Contains(Target))
		{
			Error(FText::Format(LOCTEXT("MissingNode", "{0} points to '{1}', which does not exist."), Where, FText::FromName(Target)));
		}
	};

	if (StartNode.IsNone())
	{
		Error(LOCTEXT("NoStart", "Start Node is empty."));
	}
	CheckReference(StartNode, LOCTEXT("Start", "Start Node"));

	for (const FRPGDialogueStart& Start : ConditionalStarts)
	{
		CheckReference(Start.Node, LOCTEXT("ConditionalStart", "A conditional start"));
	}

	// References and reachability.
	TSet<FName> Reachable;
	TArray<FName> Pending;
	Pending.Add(StartNode);
	for (const FRPGDialogueStart& Start : ConditionalStarts)
	{
		Pending.Add(Start.Node);
	}

	for (const FRPGDialogueNode& Node : Nodes)
	{
		const FText NodeName = FText::Format(LOCTEXT("NodeName", "Node '{0}'"), FText::FromName(Node.Id));
		CheckReference(Node.NextNode, NodeName);

		for (const FRPGDialogueChoice& Choice : Node.Choices)
		{
			CheckReference(Choice.NextNode, NodeName);
		}

		if (!Node.Choices.IsEmpty() && !Node.NextNode.IsNone())
		{
			Context.AddWarning(FText::Format(LOCTEXT("NextIgnored", "{0} has choices, so its Next Node is ignored."), NodeName));
		}
	}

	while (!Pending.IsEmpty())
	{
		const FName Current = Pending.Pop();
		if (Current.IsNone() || Reachable.Contains(Current))
		{
			continue;
		}
		Reachable.Add(Current);

		if (const FRPGDialogueNode* Node = FindNode(Current))
		{
			Pending.Add(Node->NextNode);
			for (const FRPGDialogueChoice& Choice : Node->Choices)
			{
				Pending.Add(Choice.NextNode);
			}
		}
	}

	for (const FRPGDialogueNode& Node : Nodes)
	{
		if (!Node.Id.IsNone() && !Reachable.Contains(Node.Id))
		{
			Context.AddWarning(FText::Format(LOCTEXT("Unreachable", "Node '{0}' can never be reached."), FText::FromName(Node.Id)));
		}
	}

	return Result;
}

#endif // WITH_EDITOR

#undef LOCTEXT_NAMESPACE
