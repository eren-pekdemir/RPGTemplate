#include "Items/RPGRaritySettings.h"

const FRPGRarityInfo& URPGRaritySettings::GetRarityInfo(const FGameplayTag& Rarity) const
{
	if (const FRPGRarityInfo* Found = Rarities.Find(Rarity))
	{
		return *Found;
	}
	return FallbackRarity;
}