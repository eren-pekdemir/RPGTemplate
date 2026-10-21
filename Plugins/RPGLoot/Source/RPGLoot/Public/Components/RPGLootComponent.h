#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interfaces/RPGSaveable.h"
#include "Loot/RPGLootTable.h"
#include "RPGLootComponent.generated.h"

class URPGLootComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FRPGOnLootDroppedSignature,
    URPGLootComponent*, LootComponent, const TArray<FRPGLootDrop>&, Drops);

/**
 * Rolls a loot table and scatters the results around the owner as pickups.
 * The game layer decides when (on death, on chest open).
 */
UCLASS(ClassGroup = (RPG), meta = (BlueprintSpawnableComponent))
class RPGLOOT_API URPGLootComponent : public UActorComponent, public IRPGSaveable
{
    GENERATED_BODY()

public:
    URPGLootComponent();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RPG|Loot")
    TObjectPtr<const URPGLootTable> LootTable;

    /** Spawned once per drop; must implement IRPGWorldItem (e.g. BP_ItemPickup). */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RPG|Loot", meta = (MustImplement = "/Script/RPGCore.RPGWorldItem"))
    TSubclassOf<AActor> PickupClass;

    /** How far from the owner drops land. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RPG|Loot", meta = (ClampMin = "0", Units = "cm"))
    float ScatterRadius = 80.f;

    /** Loot is generated only once (enemies, chests). */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RPG|Loot")
    bool bLootOnce = true;

    /** After the pickups are spawned. For sounds, notifications, quest counters. */
    UPROPERTY(BlueprintAssignable, Category = "RPG|Loot")
    FRPGOnLootDroppedSignature OnLootDropped;

    /** Rolls the table and spawns one pickup per drop around the owner. Returns what was dropped. */
    UFUNCTION(BlueprintCallable, Category = "RPG|Loot")
    TArray<FRPGLootDrop> DropLoot();
    
    UFUNCTION(BlueprintCallable, Category = "RPG|Loot")
    TArray<FRPGLootDrop> GiveLootTo(AActor* Receiver);

    UFUNCTION(BlueprintPure, Category = "RPG|Loot")
    bool HasLooted() const { return bHasLooted; }

private:
    FVector GetDropLocation(int32 Index, int32 Count) const;
    AActor* SpawnPickup(const FRPGLootDrop& Drop, const FVector& Location) const;
    
    /** Single gate: checks bLootOnce, marks as looted, rolls. */
    bool TryRollLoot(TArray<FRPGLootDrop>& OutDrops);

    void SpawnPickups(const TArray<FRPGLootDrop>& Drops) const;
    
    UPROPERTY(SaveGame)
    bool bHasLooted = false;
};