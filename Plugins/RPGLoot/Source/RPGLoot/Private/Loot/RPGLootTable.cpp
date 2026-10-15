#include "Loot/RPGLootTable.h"

#include "Items/RPGItemDefinition.h"
#include "RPGLoot.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif

#define LOCTEXT_NAMESPACE "RPGLootTable"

// ---------------------------------------------------------------- Rolling

TArray<FRPGLootDrop> URPGLootTable::RollLoot(int32 Seed) const
{
    FRandomStream Stream(Seed >= 0 ? Seed : FMath::Rand());
    TArray<FRPGLootDrop> Drops;
    Roll(Stream, Drops);
    return Drops;
}

void URPGLootTable::Roll(FRandomStream& Stream, TArray<FRPGLootDrop>& OutDrops) const
{
    RollInternal(Stream, OutDrops, 0);
}

void URPGLootTable::RollInternal(FRandomStream& Stream, TArray<FRPGLootDrop>& OutDrops, int32 Depth) const
{
    if (Depth > MaxNestingDepth)
    {
        UE_LOG(LogRPGLoot, Error, TEXT("%s: nested deeper than %d tables, probably a cycle. Roll stopped."),
            *GetName(), MaxNestingDepth);
        return;
    }

    for (const FRPGLootEntry& Entry : GuaranteedEntries)
    {
        RollEntry(Entry, Stream, OutDrops, Depth);
    }

    const int32 Rolls = Stream.RandRange(MinRolls, FMath::Max(MinRolls, MaxRolls));
    for (int32 RollIndex = 0; RollIndex < Rolls; ++RollIndex)
    {
        if (const FRPGLootEntry* Picked = PickWeightedEntry(Stream))
        {
            RollEntry(*Picked, Stream, OutDrops, Depth);
        }
    }
}

void URPGLootTable::RollEntry(const FRPGLootEntry& Entry, FRandomStream& Stream, TArray<FRPGLootDrop>& OutDrops, int32 Depth) const
{
    if (Entry.NestedTable)
    {
        Entry.NestedTable->RollInternal(Stream, OutDrops, Depth + 1);
        return;
    }

    if (Entry.Item)
    {
        const int32 Quantity = Stream.RandRange(Entry.MinQuantity, FMath::Max(Entry.MinQuantity, Entry.MaxQuantity));
        AddDrop(OutDrops, Entry.Item, Quantity);
    }
}

float URPGLootTable::GetTotalWeight() const
{
    float TotalWeight = NothingWeight;
    for (const FRPGLootEntry& Entry : Entries)
    {
        if (CountsInPool(Entry))
        {
            TotalWeight += Entry.Weight;
        }
    }
    return TotalWeight;
}

const FRPGLootEntry* URPGLootTable::PickWeightedEntry(FRandomStream& Stream) const
{
    const float TotalWeight = GetTotalWeight();
    if (TotalWeight <= 0.f)
    {
        return nullptr;
    }

    // Lay the weights end to end on [0, Total), throw a dart, walk until we pass it.
    // "Nothing" is the last segment.
    float Pick = Stream.FRandRange(0.f, TotalWeight);
    const FRPGLootEntry* LastCounted = nullptr;

    for (const FRPGLootEntry& Entry : Entries)
    {
        if (!CountsInPool(Entry))
        {
            continue;
        }

        if (Pick < Entry.Weight)
        {
            return &Entry;
        }

        Pick -= Entry.Weight;
        LastCounted = &Entry;
    }

    // Landed in "nothing", or float rounding pushed past the last entry.
    return NothingWeight > 0.f ? nullptr : LastCounted;
}

void URPGLootTable::AddDrop(TArray<FRPGLootDrop>& OutDrops, const URPGItemDefinition* Item, int32 Quantity)
{
    if (!Item || Quantity <= 0)
    {
        return;
    }

    for (FRPGLootDrop& Drop : OutDrops)
    {
        if (Drop.Item == Item)
        {
            Drop.Quantity += Quantity;
            return;
        }
    }

    FRPGLootDrop& NewDrop = OutDrops.AddDefaulted_GetRef();
    NewDrop.Item = Item;
    NewDrop.Quantity = Quantity;
}

// ---------------------------------------------------------------- Editor

void URPGLootTable::PostLoad()
{
    Super::PostLoad();

#if WITH_EDITOR
    UpdateChances();
#endif
}

#if WITH_EDITOR

void URPGLootTable::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);
    UpdateChances();
}

void URPGLootTable::UpdateChances()
{
    const float TotalWeight = GetTotalWeight();

    for (FRPGLootEntry& Entry : Entries)
    {
        Entry.ChancePercent = (CountsInPool(Entry) && TotalWeight > 0.f) ? Entry.Weight / TotalWeight * 100.f : 0.f;
    }

    for (FRPGLootEntry& Entry : GuaranteedEntries)
    {
        Entry.ChancePercent = Entry.HasContent() ? 100.f : 0.f;
    }

    NothingChancePercent = TotalWeight > 0.f ? NothingWeight / TotalWeight * 100.f : 100.f;
}

bool URPGLootTable::HasCycle(TSet<const URPGLootTable*>& Path) const
{
    // Path = tables on the current branch only, so A→B, A→C, B→D, C→D (a diamond) is not a cycle.
    if (Path.Contains(this))
    {
        return true;
    }

    Path.Add(this);

    auto ListHasCycle = [&Path](const TArray<FRPGLootEntry>& List)
    {
        for (const FRPGLootEntry& Entry : List)
        {
            if (Entry.NestedTable && Entry.NestedTable->HasCycle(Path))
            {
                return true;
            }
        }
        return false;
    };

    const bool bCycle = ListHasCycle(GuaranteedEntries) || ListHasCycle(Entries);
    Path.Remove(this);
    return bCycle;
}

EDataValidationResult URPGLootTable::IsDataValid(FDataValidationContext& Context) const
{
    EDataValidationResult Result = CombineDataValidationResults(Super::IsDataValid(Context), EDataValidationResult::Valid);

    auto ValidateList = [&](const TArray<FRPGLootEntry>& List, const FText& ListName)
    {
        for (int32 Index = 0; Index < List.Num(); ++Index)
        {
            const FRPGLootEntry& Entry = List[Index];

            if (Entry.Item && Entry.NestedTable)
            {
                Context.AddError(FText::Format(LOCTEXT("BothSet", "{0} [{1}] has both an Item and a Nested Table."), ListName, Index));
                Result = EDataValidationResult::Invalid;
            }
            else if (!Entry.HasContent())
            {
                Context.AddWarning(FText::Format(LOCTEXT("Empty", "{0} [{1}] is empty and will never drop anything."), ListName, Index));
            }

            if (Entry.MaxQuantity < Entry.MinQuantity)
            {
                Context.AddWarning(FText::Format(LOCTEXT("Quantity", "{0} [{1}]: Max Quantity is lower than Min Quantity."), ListName, Index));
            }
        }
    };

    ValidateList(GuaranteedEntries, LOCTEXT("Guaranteed", "Guaranteed Entries"));
    ValidateList(Entries, LOCTEXT("Pool", "Entries"));

    if (MaxRolls < MinRolls)
    {
        Context.AddWarning(LOCTEXT("Rolls", "Max Rolls is lower than Min Rolls."));
    }

    TSet<const URPGLootTable*> Path;
    if (HasCycle(Path))
    {
        Context.AddError(LOCTEXT("Cycle", "This table contains itself through nested tables."));
        Result = EDataValidationResult::Invalid;
    }

    return Result;
}

#endif // WITH_EDITOR

#undef LOCTEXT_NAMESPACE