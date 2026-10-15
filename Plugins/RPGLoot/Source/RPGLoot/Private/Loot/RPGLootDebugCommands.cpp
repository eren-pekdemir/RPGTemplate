#include "Loot/RPGLootTable.h"

#include "AssetRegistry/IAssetRegistry.h"
#include "HAL/IConsoleManager.h"
#include "Items/RPGItemDefinition.h"
#include "RPGCoreStatics.h"
#include "RPGLoot.h"

#if !UE_BUILD_SHIPPING

namespace RPGLootDebug
{
    /** Accepts an asset name (DA_LootTable_Goblin) or a full path (/Game/Loot/DA_LootTable_Goblin). */
    const URPGLootTable* FindLootTable(const FString& NameOrPath)
    {
        if (NameOrPath.Contains(TEXT("/")))
        {
            return LoadObject<URPGLootTable>(nullptr, *NameOrPath);
        }

        TArray<FAssetData> Assets;
        IAssetRegistry::GetChecked().GetAssetsByClass(URPGLootTable::StaticClass()->GetClassPathName(), Assets, true);

        for (const FAssetData& Asset : Assets)
        {
            if (Asset.AssetName.ToString().Equals(NameOrPath, ESearchCase::IgnoreCase))
            {
                return Cast<URPGLootTable>(Asset.GetAsset());
            }
        }
        return nullptr;
    }

    void Simulate(const TArray<FString>& Args)
    {
        if (Args.Num() < 1)
        {
            UE_LOG(LogRPGLoot, Display, TEXT("Usage: rpg.Loot.Simulate <TableNameOrPath> [Count=10000] [Seed=random]"));
            return;
        }

        const URPGLootTable* Table = FindLootTable(Args[0]);
        if (!Table)
        {
            UE_LOG(LogRPGLoot, Error, TEXT("Loot table '%s' not found."), *Args[0]);
            return;
        }

        const int32 Count = Args.Num() > 1 ? FMath::Clamp(FCString::Atoi(*Args[1]), 1, 1000000) : 10000;
        const int32 Seed = Args.Num() > 2 ? FCString::Atoi(*Args[2]) : FMath::Rand();

        struct FItemStats
        {
            int32 TimesDropped = 0;   // results that contained the item
            int64 TotalQuantity = 0;
        };

        TMap<const URPGItemDefinition*, FItemStats> Stats;
        int32 EmptyResults = 0;

        FRandomStream Stream(Seed);
        TArray<FRPGLootDrop> Drops;
        const double StartTime = FPlatformTime::Seconds();

        for (int32 Index = 0; Index < Count; ++Index)
        {
            Drops.Reset();   // keeps the allocation, no realloc per roll
            Table->Roll(Stream, Drops);

            if (Drops.IsEmpty())
            {
                ++EmptyResults;
                continue;
            }

            // Drops are merged per item, so each item appears at most once per result.
            for (const FRPGLootDrop& Drop : Drops)
            {
                FItemStats& ItemStats = Stats.FindOrAdd(Drop.Item);
                ++ItemStats.TimesDropped;
                ItemStats.TotalQuantity += Drop.Quantity;
            }
        }

        const double ElapsedMs = (FPlatformTime::Seconds() - StartTime) * 1000.0;

        Stats.ValueSort([](const FItemStats& A, const FItemStats& B) { return A.TimesDropped > B.TimesDropped; });

        UE_LOG(LogRPGLoot, Display, TEXT("Loot simulation: %s x%d (seed %d) in %.1f ms"),
            *Table->GetName(), Count, Seed, ElapsedMs);
        UE_LOG(LogRPGLoot, Display, TEXT("  %-28s %8s %10s  %s"), TEXT("Item"), TEXT("Drop %"), TEXT("Avg qty"), TEXT("Rarity"));

        for (const TPair<const URPGItemDefinition*, FItemStats>& Pair : Stats)
        {
            const URPGItemDefinition* Item = Pair.Key;
            const FString ItemName = Item->DisplayName.IsEmpty() ? Item->GetName() : Item->DisplayName.ToString();
            const FString RarityName = URPGCoreStatics::GetItemRarityInfo(Item).DisplayName.ToString();

            // 100.0 first: double arithmetic, no integer division.
            const double DropRate = 100.0 * Pair.Value.TimesDropped / Count;
            const double AverageQuantity = static_cast<double>(Pair.Value.TotalQuantity) / Pair.Value.TimesDropped;

            UE_LOG(LogRPGLoot, Display, TEXT("  %-28s %7.2f%% %10.1f  %s"), *ItemName, DropRate, AverageQuantity, *RarityName);
        }

        UE_LOG(LogRPGLoot, Display, TEXT("  %-28s %7.2f%%"), TEXT("(nothing)"), 100.0 * EmptyResults / Count);
    }

    static FAutoConsoleCommand SimulateCommand(
        TEXT("rpg.Loot.Simulate"),
        TEXT("Rolls a loot table many times and logs drop rates. Usage: rpg.Loot.Simulate <TableNameOrPath> [Count=10000] [Seed=random]"),
        FConsoleCommandWithArgsDelegate::CreateStatic(&Simulate));
}

#endif // !UE_BUILD_SHIPPING