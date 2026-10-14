#pragma once

#include "CoreMinimal.h"
#include "Abilities/RPGGameplayAbility.h"
#include "Engine/EngineTypes.h"
#include "RPGMeleeAbility.generated.h"

class UAnimMontage;
class UGameplayEffect;

/**
 * Swing in front of the avatar and apply a damage effect to everything hit.
 * With a montage, the hit happens when the montage sends HitEventTag (via an AnimNotify).
 * Without a montage, the hit happens after HitDelay.
 */
UCLASS(Abstract)
class RPGABILITIES_API URPGMeleeAbility : public URPGGameplayAbility
{
    GENERATED_BODY()

public:
    URPGMeleeAbility();

protected:
    /** Optional. If empty, the hit happens after HitDelay. */
    UPROPERTY(EditDefaultsOnly, Category = "RPG|Melee")
    TObjectPtr<UAnimMontage> AttackMontage;

    /** Gameplay event the montage sends at the moment of impact. */
    UPROPERTY(EditDefaultsOnly, Category = "RPG|Melee", meta = (Categories = "Event"))
    FGameplayTag HitEventTag;

    /** Used only when there is no montage. */
    UPROPERTY(EditDefaultsOnly, Category = "RPG|Melee", meta = (ClampMin = "0.0"))
    float HitDelay = 0.3f;

    /** Usually GE_Damage. */
    UPROPERTY(EditDefaultsOnly, Category = "RPG|Melee")
    TSubclassOf<UGameplayEffect> DamageEffect;

    /** Added on top of the attacker's Damage attribute (SetByCaller.Damage). */
    UPROPERTY(EditDefaultsOnly, Category = "RPG|Melee")
    float BaseDamage = 0.f;

    UPROPERTY(EditDefaultsOnly, Category = "RPG|Melee|Trace", meta = (ClampMin = "0.0"))
    float TraceRange = 150.f;

    UPROPERTY(EditDefaultsOnly, Category = "RPG|Melee|Trace", meta = (ClampMin = "0.0"))
    float TraceRadius = 50.f;

    /** Object types the swing can hit. */
    UPROPERTY(EditDefaultsOnly, Category = "RPG|Melee|Trace")
    TArray<TEnumAsByte<EObjectTypeQuery>> HitObjectTypes;

    UPROPERTY(EditDefaultsOnly, Category = "RPG|Melee|Debug")
    bool bDrawDebug = false;

    virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo,
        const FGameplayEventData* TriggerEventData) override;

private:
    UFUNCTION()
    void HandleHitEvent(FGameplayEventData Payload);

    UFUNCTION()
    void HandleDelayFinished();

    UFUNCTION()
    void HandleMontageCompleted();

    UFUNCTION()
    void HandleMontageCancelled();

    /** Sweep in front of the avatar and damage every actor with an ASC (once per actor). */
    void PerformHit();

    void FinishAttack(bool bWasCancelled);

    /** One hit per swing, even if the montage sends the event twice. */
    bool bHitPerformed = false;
};