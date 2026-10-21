// RPGReputationEntryWidget.cpp

#include "Widgets/RPGReputationEntryWidget.h"

#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Engine/Texture2D.h"
#include "Factions/RPGFactionDefinition.h"

#define LOCTEXT_NAMESPACE "RPGReputationEntry"

void URPGReputationEntryWidget::Setup(const URPGFactionDefinition* InFaction, int32 Reputation)
{
	Faction = InFaction;
	if (!Faction)
	{
		return;
	}

	if (IconImage)
	{
		if (!Faction->Icon.IsNull())
		{
			IconImage->SetBrushFromSoftTexture(Faction->Icon);
			IconImage->SetVisibility(ESlateVisibility::HitTestInvisible);
		}
		else
		{
			IconImage->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	NameText->SetText(Faction->DisplayName);

	const int32 RankIndex = Faction->GetRankIndex(Reputation);
	const bool bHasRank = RankIndex != INDEX_NONE;
	const bool bHasNextRank = bHasRank && Faction->Ranks.IsValidIndex(RankIndex + 1);
	const FLinearColor RankColor = bHasRank ? Faction->Ranks[RankIndex].Color : FLinearColor::White;

	if (RankText)
	{
		RankText->SetText(bHasRank ? Faction->Ranks[RankIndex].DisplayName : FText::GetEmpty());
		RankText->SetColorAndOpacity(FSlateColor(RankColor));
	}

	if (ValueText)
	{
		ValueText->SetText(bHasNextRank
			? FText::Format(LOCTEXT("ToNext", "{0} / {1}"), FText::AsNumber(Reputation), FText::AsNumber(Faction->Ranks[RankIndex + 1].MinReputation))
			: FText::AsNumber(Reputation));
	}

	if (RankProgressBar)
	{
		float Percent = 1.f;
		if (bHasNextRank)
		{
			const int32 RankStart = Faction->Ranks[RankIndex].MinReputation;
			const int32 RankEnd = Faction->Ranks[RankIndex + 1].MinReputation;
			// float first: integer division would always give 0.
			Percent = static_cast<float>(Reputation - RankStart) / static_cast<float>(FMath::Max(RankEnd - RankStart, 1));
		}
		RankProgressBar->SetPercent(FMath::Clamp(Percent, 0.f, 1.f));
		RankProgressBar->SetFillColorAndOpacity(RankColor);
	}
}

#undef LOCTEXT_NAMESPACE
