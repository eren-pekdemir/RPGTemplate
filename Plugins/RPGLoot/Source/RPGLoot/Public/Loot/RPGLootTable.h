#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "RPGLootTable.generated.h"

class URPGItemDefinition;
class URPGLootTable;

/** One rolled result: an item and how many. */
USTRUCT(BlueprintType)
struct RPGLOOT_API FRPGLootDrop
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loot")
    TObjectPtr<const URPGItemDefinition> Item;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loot", meta = (ClampMin = "1"))
    int32 Quantity = 1;
};

/** One line of a loot table: an item, or a nested table rolled in its place. */
USTRUCT(BlueprintType)
struct RPGLOOT_API FRPGLootEntry
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Loot", meta = (EditCondition = "NestedTable == nullptr"))
    TObjectPtr<const URPGItemDefinition> Item;

    /** Rolled instead of Item, e.g. a shared "rare gems" table. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Loot", meta = (EditCondition = "Item == nullptr"))
    TObjectPtr<const URPGLootTable> NestedTable;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Loot", meta = (ClampMin = "1", EditCondition = "NestedTable == nullptr"))
    int32 MinQuantity = 1;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Loot", meta = (ClampMin = "1", EditCondition = "NestedTable == nullptr"))
    int32 MaxQuantity = 1;

    /** Relative chance in the weighted pool. Ignored for guaranteed entries. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Loot", meta = (ClampMin = "0"))
    float Weight = 1.f;

#if WITH_EDITORONLY_DATA
    /** Chance of this entry per roll, calculated by the editor. */
    UPROPERTY(VisibleAnywhere, Transient, Category = "Loot", meta = (DisplayName = "Chance per Roll %"))
    float ChancePercent = 0.f;
#endif

    bool HasContent() const { return Item || NestedTable; }
};

/**
 * Weighted, nestable loot table. Only produces a list of drops;
 * what happens with it (spawn, give, fill a chest) is up to the caller.
 */
UCLASS(BlueprintType, Const)
class RPGLOOT_API URPGLootTable : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    /** Always dropped, every time the table is rolled. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loot", meta = (TitleProperty = "Item"))
    TArray<FRPGLootEntry> GuaranteedEntries;

    /** How many times the weighted pool is rolled (random between min and max). */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loot", meta = (ClampMin = "0"))
    int32 MinRolls = 1;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loot", meta = (ClampMin = "0"))
    int32 MaxRolls = 1;

    /** Weight of "no drop" in the pool. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loot", meta = (ClampMin = "0"))
    float NothingWeight = 0.f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loot", meta = (TitleProperty = "Item"))
    TArray<FRPGLootEntry> Entries;

#if WITH_EDITORONLY_DATA
    UPROPERTY(VisibleAnywhere, Transient, Category = "Loot", meta = (DisplayName = "Nothing Chance per Roll %"))
    float NothingChancePercent = 0.f;
#endif

    /** Rolls the table. Seed < 0 = random; the same seed always gives the same result. */
    UFUNCTION(BlueprintCallable, Category = "RPG|Loot")
    TArray<FRPGLootDrop> RollLoot(int32 Seed = -1) const;

    /** C++: rolls with a caller-owned stream; drops of the same item are merged into OutDrops. */
    void Roll(FRandomStream& Stream, TArray<FRPGLootDrop>& OutDrops) const;

    /** Safety net against cycles that slipped past validation. */
    static constexpr int32 MaxNestingDepth = 8;

    virtual void PostLoad() override;

#if WITH_EDITOR
    virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
    virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif

private:
    void RollInternal(FRandomStream& Stream, TArray<FRPGLootDrop>& OutDrops, int32 Depth) const;
    void RollEntry(const FRPGLootEntry& Entry, FRandomStream& Stream, TArray<FRPGLootDrop>& OutDrops, int32 Depth) const;
    const FRPGLootEntry* PickWeightedEntry(FRandomStream& Stream) const;
    float GetTotalWeight() const;

    static bool CountsInPool(const FRPGLootEntry& Entry) { return Entry.HasContent() && Entry.Weight > 0.f; }
    static void AddDrop(TArray<FRPGLootDrop>& OutDrops, const URPGItemDefinition* Item, int32 Quantity);

#if WITH_EDITOR
    void UpdateChances();
    bool HasCycle(TSet<const URPGLootTable*>& Path) const;
#endif
};