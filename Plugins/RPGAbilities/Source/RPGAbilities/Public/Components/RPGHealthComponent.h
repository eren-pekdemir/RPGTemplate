#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RPGHealthComponent.generated.h"

class UAbilitySystemComponent;
class URPGHealthComponent;
struct FOnAttributeChangeData;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FRPGOnHealthChangedSignature,
    URPGHealthComponent*, HealthComponent, float, OldValue, float, NewValue, AActor*, Instigator);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FRPGOnDeathSignature,
    AActor*, DeadActor, AActor*, Killer);

/**
 * Turns the owner's Health attribute into gameplay events.
 * Requires an Ability System Component with URPGHealthSet on the same actor.
 * It only reports; reacting to death (ragdoll, respawn, loot) is the game layer's job.
 */
UCLASS(ClassGroup = (RPG), meta = (BlueprintSpawnableComponent))
class RPGABILITIES_API URPGHealthComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    URPGHealthComponent();

    /** Every Health change (damage, heal, MaxHealth clamp). Instigator may be null. */
    UPROPERTY(BlueprintAssignable, Category = "RPG|Health")
    FRPGOnHealthChangedSignature OnHealthChanged;

    /** Fired once when Health reaches 0. */
    UPROPERTY(BlueprintAssignable, Category = "RPG|Health")
    FRPGOnDeathSignature OnDeath;

    UFUNCTION(BlueprintPure, Category = "RPG|Health")
    float GetHealth() const;

    UFUNCTION(BlueprintPure, Category = "RPG|Health")
    float GetMaxHealth() const;

    /** Health / MaxHealth in [0, 1]; 0 if MaxHealth is 0. For health bars. */
    UFUNCTION(BlueprintPure, Category = "RPG|Health")
    float GetHealthNormalized() const;

    UFUNCTION(BlueprintPure, Category = "RPG|Health")
    bool IsDead() const;

protected:
    /** Find the owner's ASC and bind to the Health attribute change delegate. */
    virtual void BeginPlay() override;

    /** Unbind from the ASC. */
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
    /** Native (non-dynamic) callback from the ASC: broadcast OnHealthChanged, detect death. */
    void HandleHealthChanged(const FOnAttributeChangeData& Data);

    /** State first (bIsDead, State.Dead tag), then OnDeath. */
    void HandleDeath(AActor* Killer);

    TWeakObjectPtr<UAbilitySystemComponent> AbilitySystem;

    /** Needed to remove a native delegate binding. */
    FDelegateHandle HealthChangedHandle;

    bool bIsDead = false;
};