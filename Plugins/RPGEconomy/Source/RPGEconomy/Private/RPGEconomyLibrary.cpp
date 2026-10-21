// RPGEconomyLibrary.cpp

#include "RPGEconomyLibrary.h"

#include "Items/RPGItemDefinition.h"
#include "Items/RPGValueFragment.h"
#include "RPGCoreTags.h"
#include "RPGEconomySettings.h"

FGameplayTag URPGEconomyLibrary::ResolveCurrency(const FGameplayTag& Currency)
{
	return Currency.IsValid() ? Currency : GetDefault<URPGEconomySettings>()->DefaultCurrency;
}

int64 URPGEconomyLibrary::GetItemValue(const URPGItemDefinition* Item)
{
	const URPGValueFragment* Fragment = Item ? Item->FindFragmentByClass<URPGValueFragment>() : nullptr;
	if (!Fragment || Fragment->Value <= 0)
	{
		return 0;
	}

	const float* Multiplier = GetDefault<URPGEconomySettings>()->RarityValueMultipliers.Find(Item->Rarity);
	const double Scaled = static_cast<double>(Fragment->Value) * (Multiplier ? FMath::Max(*Multiplier, 0.f) : 1.f);
	return static_cast<int64>(FMath::RoundToDouble(Scaled));
}

bool URPGEconomyLibrary::CanItemBeSold(const URPGItemDefinition* Item)
{
	const URPGValueFragment* Fragment = Item ? Item->FindFragmentByClass<URPGValueFragment>() : nullptr;
	return Fragment && Fragment->bCanBeSold && !Item->ItemType.MatchesTag(RPGTags::Item_Type_Quest);
}

FRPGCurrencyInfo URPGEconomyLibrary::GetCurrencyInfo(FGameplayTag Currency)
{
	const FRPGCurrencyInfo* Found = GetDefault<URPGEconomySettings>()->Currencies.Find(ResolveCurrency(Currency));
	return Found ? *Found : FRPGCurrencyInfo();
}
