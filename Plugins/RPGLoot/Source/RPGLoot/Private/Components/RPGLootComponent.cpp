#include "Components/RPGLootComponent.h"

#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Interfaces/RPGItemContainer.h"
#include "Items/RPGItemDefinition.h"
#include "Items/RPGWorldItem.h"
#include "RPGCoreStatics.h"
#include "RPGLoot.h"

URPGLootComponent::URPGLootComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

// ---------------------------------------------------------------- Public

TArray<FRPGLootDrop> URPGLootComponent::DropLoot()
{
    TArray<FRPGLootDrop> Drops;

    if (!PickupClass)
    {
        UE_LOG(LogRPGLoot, Warning, TEXT("%s: PickupClass is not set, nothing dropped."), *GetNameSafe(GetOwner()));
        return Drops;
    }

    if (!TryRollLoot(Drops))
    {
        return Drops;
    }

    SpawnPickups(Drops);
    OnLootDropped.Broadcast(this, Drops);
    return Drops;
}

TArray<FRPGLootDrop> URPGLootComponent::GiveLootTo(AActor* Receiver)
{
    TArray<FRPGLootDrop> Drops;

    // Check before rolling: a wrong receiver must not use up the loot.
    IRPGItemContainer* Container = URPGCoreStatics::FindItemContainer(Receiver);
    if (!Container)
    {
        UE_LOG(LogRPGLoot, Warning, TEXT("%s: %s has no item container, loot kept."),
            *GetNameSafe(GetOwner()), *GetNameSafe(Receiver));
        return Drops;
    }

    if (!TryRollLoot(Drops))
    {
        return Drops;
    }

    TArray<FRPGLootDrop> Leftovers;
    for (const FRPGLootDrop& Drop : Drops)
    {
        const int32 Added = Container->AddItem(Drop.Item, Drop.Quantity);
        if (Added < Drop.Quantity)
        {
            FRPGLootDrop& Leftover = Leftovers.AddDefaulted_GetRef();
            Leftover.Item = Drop.Item;
            Leftover.Quantity = Drop.Quantity - Added;
        }
    }

    if (!Leftovers.IsEmpty())
    {
        if (PickupClass)
        {
            SpawnPickups(Leftovers);
        }
        else
        {
            UE_LOG(LogRPGLoot, Error, TEXT("%s: %d stacks did not fit and PickupClass is not set; they are lost."),
                *GetNameSafe(GetOwner()), Leftovers.Num());
        }
    }

    OnLootDropped.Broadcast(this, Drops);
    return Drops;
}

// ---------------------------------------------------------------- Private

bool URPGLootComponent::TryRollLoot(TArray<FRPGLootDrop>& OutDrops)
{
    if (bLootOnce && bHasLooted)
    {
        return false;
    }

    if (!LootTable)
    {
        UE_LOG(LogRPGLoot, Warning, TEXT("%s: LootTable is not set."), *GetNameSafe(GetOwner()));
        return false;
    }

    // State first: even an empty roll counts as "looted".
    bHasLooted = true;

    // GenerateNewSeed, not FMath::Rand(): Rand() only returns 0..32767 on Windows (32768 possible loot sequences).
    FRandomStream Stream;
    Stream.GenerateNewSeed();
    LootTable->Roll(Stream, OutDrops);
    return true;
}

void URPGLootComponent::SpawnPickups(const TArray<FRPGLootDrop>& Drops) const
{
    for (int32 Index = 0; Index < Drops.Num(); ++Index)
    {
        SpawnPickup(Drops[Index], GetDropLocation(Index, Drops.Num()));
    }
}

FVector URPGLootComponent::GetDropLocation(int32 Index, int32 Count) const
{
    const AActor* Owner = GetOwner();
    const FVector Origin = Owner->GetActorLocation();

    // Evenly around a circle, with jitter so it does not look like a clock face.
    const float Angle = 2.f * PI * static_cast<float>(Index) / FMath::Max(Count, 1) + FMath::FRandRange(-0.3f, 0.3f);
    const float Radius = ScatterRadius * FMath::FRandRange(0.6f, 1.f);
    const FVector Candidate = Origin + FVector(FMath::Cos(Angle), FMath::Sin(Angle), 0.f) * Radius;

    // Snap down to the ground (the owner's location may be well above the floor).
    FHitResult Hit;
    FCollisionQueryParams Params(SCENE_QUERY_STAT(RPGLootDrop), false, Owner);
    const FVector TraceStart = Candidate + FVector(0.f, 0.f, 100.f);
    const FVector TraceEnd = Candidate - FVector(0.f, 0.f, 500.f);

    if (GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Visibility, Params))
    {
        return Hit.ImpactPoint + FVector(0.f, 0.f, 10.f);
    }

    return Candidate;
}

AActor* URPGLootComponent::SpawnPickup(const FRPGLootDrop& Drop, const FVector& Location) const
{
    UWorld* World = GetWorld();
    if (!World || !Drop.Item)
    {
        return nullptr;
    }

    const FTransform SpawnTransform(FRotator(0.f, FMath::FRandRange(0.f, 360.f), 0.f), Location);

    // Deferred: the item is set before OnConstruction picks the world mesh.
    AActor* Pickup = World->SpawnActorDeferred<AActor>(PickupClass, SpawnTransform, nullptr, nullptr,
        ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

    if (!Pickup)
    {
        UE_LOG(LogRPGLoot, Error, TEXT("%s: failed to spawn a pickup for %s."), *GetNameSafe(GetOwner()), *GetNameSafe(Drop.Item));
        return nullptr;
    }

    if (IRPGWorldItem* WorldItem = Cast<IRPGWorldItem>(Pickup))
    {
        WorldItem->InitializeWorldItem(Drop.Item, Drop.Quantity);
    }

    Pickup->FinishSpawning(SpawnTransform);
    return Pickup;
}