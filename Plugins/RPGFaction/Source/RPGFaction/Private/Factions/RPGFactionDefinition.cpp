// RPGFactionDefinition.cpp

#include "Factions/RPGFactionDefinition.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif

#define LOCTEXT_NAMESPACE "RPGFactionDefinition"

int32 URPGFactionDefinition::GetRankIndex(int32 Reputation) const
{
	if (Ranks.IsEmpty())
	{
		return INDEX_NONE;
	}

	int32 Result = 0;
	for (int32 Index = 0; Index < Ranks.Num(); ++Index)
	{
		if (Ranks[Index].MinReputation <= Reputation)
		{
			Result = Index;
		}
	}
	return Result;
}

#if WITH_EDITOR

EDataValidationResult URPGFactionDefinition::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = CombineDataValidationResults(Super::IsDataValid(Context), EDataValidationResult::Valid);

	if (!FactionTag.IsValid())
	{
		Context.AddError(LOCTEXT("NoTag", "Faction Tag is empty."));
		Result = EDataValidationResult::Invalid;
	}

	if (Ranks.IsEmpty())
	{
		Context.AddWarning(LOCTEXT("NoRanks", "No ranks: the reputation panel shows only the number and nothing is ever hostile."));
	}

	for (int32 Index = 1; Index < Ranks.Num(); ++Index)
	{
		if (Ranks[Index].MinReputation <= Ranks[Index - 1].MinReputation)
		{
			Context.AddError(FText::Format(LOCTEXT("Unsorted", "Rank {0} must have a higher Min Reputation than rank {1}."),
				FText::AsNumber(Index), FText::AsNumber(Index - 1)));
			Result = EDataValidationResult::Invalid;
		}
	}

	for (const FRPGFactionRelation& Relation : Relations)
	{
		if (Relation.Faction == FactionTag)
		{
			Context.AddError(LOCTEXT("SelfRelation", "A faction cannot be related to itself."));
			Result = EDataValidationResult::Invalid;
		}
	}

	return Result;
}

#endif // WITH_EDITOR

#undef LOCTEXT_NAMESPACE
