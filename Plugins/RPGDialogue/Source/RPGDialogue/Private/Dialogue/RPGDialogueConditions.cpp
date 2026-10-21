// RPGDialogueConditions.cpp

#include "Dialogue/RPGDialogueConditions.h"

#include "Components/RPGDialogueComponent.h"
#include "GameplayTagAssetInterface.h"
#include "Interfaces/RPGCurrencyHolder.h"
#include "Interfaces/RPGItemContainer.h"
#include "Interfaces/RPGReputationHolder.h"
#include "RPGCoreStatics.h"

bool URPGDialogueCondition_HasFlags::IsMet(const FRPGDialogueContext& Context) const
{
	if (!Context.Dialogue)
	{
		return false;
	}
	return Context.Dialogue->HasFlags(Flags, bRequireAll);
}

bool URPGDialogueCondition_HasTags::IsMet(const FRPGDialogueContext& Context) const
{
	FGameplayTagContainer OwnedTags;
	if (const IGameplayTagAssetInterface* TagSource =
			URPGCoreStatics::FindInterface<IGameplayTagAssetInterface, UGameplayTagAssetInterface>(Context.Player))
	{
		TagSource->GetOwnedGameplayTags(OwnedTags);
	}
	return bRequireAll ? OwnedTags.HasAll(Tags) : OwnedTags.HasAny(Tags);
}

bool URPGDialogueCondition_HasItem::IsMet(const FRPGDialogueContext& Context) const
{
	const IRPGItemContainer* Container = URPGCoreStatics::FindItemContainer(Context.Player);
	return Container && Item && Container->GetItemCount(Item) >= Quantity;
}

bool URPGDialogueCondition_HasCurrency::IsMet(const FRPGDialogueContext& Context) const
{
	const IRPGCurrencyHolder* Holder = URPGCoreStatics::FindCurrencyHolder(Context.Player);
	return Holder && Holder->CanAfford(Currency, Amount);
}

bool URPGDialogueCondition_Reputation::IsMet(const FRPGDialogueContext& Context) const
{
	const IRPGReputationHolder* Holder = IRPGReputationHolder::Find(Context.Player);
	return Holder && Holder->GetReputation(Faction) >= MinReputation;
}
