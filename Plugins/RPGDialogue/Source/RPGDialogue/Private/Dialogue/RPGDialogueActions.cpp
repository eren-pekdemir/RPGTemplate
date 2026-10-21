// RPGDialogueActions.cpp

#include "Dialogue/RPGDialogueActions.h"

#include "Components/RPGDialogueComponent.h"
#include "Interfaces/RPGCurrencyHolder.h"
#include "Interfaces/RPGItemContainer.h"
#include "Items/RPGItemDefinition.h"
#include "Interfaces/RPGReputationHolder.h"
#include "RPGCoreStatics.h"
#include "RPGDialogue.h"

void URPGDialogueAction_SetFlags::Execute(const FRPGDialogueContext& Context) const
{
	if (Context.Dialogue)
	{
		Context.Dialogue->AddFlags(AddFlags);
		Context.Dialogue->RemoveFlags(RemoveFlags);
	}
}

void URPGDialogueAction_ChangeItem::Execute(const FRPGDialogueContext& Context) const
{
	IRPGItemContainer* Container = URPGCoreStatics::FindItemContainer(Context.Player);
	if (!Container || !Item || Quantity == 0)
	{
		return;
	}

	if (Quantity > 0)
	{
		const int32 Added = Container->AddItem(Item, Quantity);
		if (Added < Quantity)
		{
			UE_LOG(LogRPGDialogue, Warning, TEXT("Change Item: only %d of %d %s fitted."), Added, Quantity, *GetNameSafe(Item));
		}
	}
	else
	{
		Container->RemoveItem(Item, -Quantity);
	}
}

void URPGDialogueAction_ChangeCurrency::Execute(const FRPGDialogueContext& Context) const
{
	IRPGCurrencyHolder* Holder = URPGCoreStatics::FindCurrencyHolder(Context.Player);
	if (!Holder || Amount == 0)
	{
		return;
	}

	if (Amount > 0)
	{
		Holder->AddCurrency(Currency, Amount);
	}
	else if (!Holder->SpendCurrency(Currency, -Amount))
	{
		UE_LOG(LogRPGDialogue, Warning, TEXT("Change Currency: the player could not pay %lld."), -Amount);
	}
}

void URPGDialogueAction_ChangeReputation::Execute(const FRPGDialogueContext& Context) const
{
	if (IRPGReputationHolder* Holder = IRPGReputationHolder::Find(Context.Player))
	{
		Holder->AddReputation(Faction, Amount);
	}
}

void URPGDialogueAction_SendEvent::Execute(const FRPGDialogueContext& Context) const
{
	if (Context.Dialogue && EventTag.IsValid())
	{
		Context.Dialogue->SendEvent(EventTag, bWaitForDialogueEnd);
	}
}
