// RPGReputationPanelWidget.cpp

#include "Widgets/RPGReputationPanelWidget.h"

#include "Components/PanelWidget.h"
#include "Components/RPGReputationComponent.h"
#include "Factions/RPGFactionDefinition.h"
#include "GameFramework/PlayerState.h"
#include "RPGFactionSettings.h"
#include "Widgets/RPGReputationEntryWidget.h"

void URPGReputationPanelWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (!Reputation)
	{
		if (APlayerState* PlayerState = GetOwningPlayerState())
		{
			SetReputation(PlayerState->FindComponentByClass<URPGReputationComponent>());
		}
	}
	else
	{
		Rebuild();
	}
}

void URPGReputationPanelWidget::NativeDestruct()
{
	SetReputation(nullptr);
	Super::NativeDestruct();
}

void URPGReputationPanelWidget::SetReputation(URPGReputationComponent* NewReputation)
{
	// Unbind -> assign -> bind -> rebuild
	if (Reputation)
	{
		Reputation->OnReputationChanged.RemoveDynamic(this, &ThisClass::HandleReputationChanged);
	}

	Reputation = NewReputation;

	if (Reputation)
	{
		Reputation->OnReputationChanged.AddDynamic(this, &ThisClass::HandleReputationChanged);
	}

	Rebuild();
}

void URPGReputationPanelWidget::HandleReputationChanged(FGameplayTag Faction, int32 NewReputation, int32 Delta)
{
	Rebuild();
	BP_OnReputationChanged(Faction, NewReputation, Delta);
}

void URPGReputationPanelWidget::Rebuild()
{
	if (!FactionList)
	{
		return;
	}

	FactionList->ClearChildren();
	if (!Reputation || !EntryClass)
	{
		return;
	}

	for (const URPGFactionDefinition* Faction : URPGFactionSettings::GetAllFactions())
	{
		if (!Faction->bShowInUI)
		{
			continue;
		}

		URPGReputationEntryWidget* Entry = CreateWidget<URPGReputationEntryWidget>(this, EntryClass);
		Entry->Setup(Faction, Reputation->GetFactionReputation(Faction->FactionTag));
		FactionList->AddChild(Entry);
	}
}
