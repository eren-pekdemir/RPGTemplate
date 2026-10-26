// RPGEconomyUIHelpers.h
// Small text helpers shared by the economy widgets (private to RPGEconomyUI).

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "RPGEconomyLibrary.h"

namespace RPGEconomyUI
{
	/** "120 Gold": the currency's DisplayName from Project Settings, or the last part of its tag. */
	inline FText FormatCurrency(int64 Amount, const FGameplayTag& Currency)
	{
		const FGameplayTag Resolved = URPGEconomyLibrary::ResolveCurrency(Currency);
		FText Name = URPGEconomyLibrary::GetCurrencyInfo(Resolved).DisplayName;

		if (Name.IsEmpty() && Resolved.IsValid())
		{
			FString TagName = Resolved.ToString();
			int32 DotIndex = INDEX_NONE;
			if (TagName.FindLastChar(TEXT('.'), DotIndex))
			{
				TagName.RightChopInline(DotIndex + 1);
			}
			Name = FText::FromString(TagName);
		}

		return FText::Format(NSLOCTEXT("RPGEconomyUI", "CurrencyFormat", "{0} {1}"), FText::AsNumber(Amount), Name);
	}

	/** Project default currency (an empty tag resolves to it). */
	inline FGameplayTag GetDefaultCurrency()
	{
		return URPGEconomyLibrary::ResolveCurrency(FGameplayTag());
	}
}
