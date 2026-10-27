// Fill out your copyright notice in the Description page of Project Settings.

#include "Abilities/RPGMeleeAbility.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "RPGAbilities.h"
#include "RPGAbilityTags.h"
#include "RPGCoreTags.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Kismet/KismetSystemLibrary.h"

URPGMeleeAbility::URPGMeleeAbility()
{
    HitEventTag = RPGTags::Event_Melee_Hit;
    HitObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));
    HitObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldDynamic));
}

void URPGMeleeAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
    const FGameplayEventData* TriggerEventData)
{
    // No Super call: the base implementation commits the ability itself when there is no BP graph,
    // which would apply cost and cooldown twice.
    if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    // InstancedPerActor reuses this object for every swing, so reset per-swing state.
    bHitPerformed = false;

    if (AttackMontage)
    {
        // Listen for the hit event BEFORE starting the montage, so an early notify is not missed.
        UAbilityTask_WaitGameplayEvent* WaitHit = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, HitEventTag);
        WaitHit->EventReceived.AddDynamic(this, &ThisClass::HandleHitEvent);
        WaitHit->ReadyForActivation();

        UAbilityTask_PlayMontageAndWait* PlayTask =
            UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, AttackMontage);
        PlayTask->OnCompleted.AddDynamic(this, &ThisClass::HandleMontageCompleted);
        PlayTask->OnBlendOut.AddDynamic(this, &ThisClass::HandleMontageCompleted);
        PlayTask->OnInterrupted.AddDynamic(this, &ThisClass::HandleMontageCancelled);
        PlayTask->OnCancelled.AddDynamic(this, &ThisClass::HandleMontageCancelled);
        PlayTask->ReadyForActivation();
    }
    else
    {
        // No animation: hit after a short delay, then end.
        UAbilityTask_WaitDelay* WaitDelay = UAbilityTask_WaitDelay::WaitDelay(this, HitDelay);
        WaitDelay->OnFinish.AddDynamic(this, &ThisClass::HandleDelayFinished);
        WaitDelay->ReadyForActivation();
    }
}

void URPGMeleeAbility::HandleHitEvent(FGameplayEventData Payload)
{
    PerformHit();
}

void URPGMeleeAbility::HandleDelayFinished()
{
    PerformHit();
    FinishAttack(false);
}

void URPGMeleeAbility::HandleMontageCompleted()
{
    FinishAttack(false);
}

void URPGMeleeAbility::HandleMontageCancelled()
{
    FinishAttack(true);
}

void URPGMeleeAbility::PerformHit()
{
    // One hit per swing, even if the montage sends the event more than once.
    if (bHitPerformed) return;
    bHitPerformed = true;

    AActor* Avatar = GetAvatarActorFromActorInfo();
    if (!Avatar) return;

    if (!DamageEffect)
    {
        UE_LOG(LogRPGAbilities, Warning, TEXT("%s: DamageEffect is not set, melee hit ignored"), *GetNameSafe(this));
        return;
    }

    UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo();
    if (!SourceASC) return;

    const FVector Start = Avatar->GetActorLocation();
    const FVector End = Start + Avatar->GetActorForwardVector() * TraceRange;

    TArray<FHitResult> Hits;
    UKismetSystemLibrary::SphereTraceMultiForObjects(
        Avatar, Start, End, TraceRadius, HitObjectTypes, false, { Avatar },
        bDrawDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None, Hits, true);

    // An actor can be hit through several components (capsule + mesh); damage it only once.
    TSet<AActor*> AlreadyHit;

    for (const FHitResult& Hit : Hits)
    {
        AActor* HitActor = Hit.GetActor();
        if (!HitActor || AlreadyHit.Contains(HitActor)) continue;
        AlreadyHit.Add(HitActor);

        UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(HitActor);
        if (!TargetASC) continue;   // walls, props: no ASC, no damage

        if (TargetASC->HasMatchingGameplayTag(RPGTags::State_Dead)) continue;   // skip this one, keep checking others
        if (TargetASC->HasMatchingGameplayTag(RPGTags::State_Invulnerable)) continue;   // rolling: i-frames

        FGameplayEffectSpecHandle Spec = MakeOutgoingGameplayEffectSpec(DamageEffect, GetAbilityLevel());
        if (!Spec.IsValid()) continue;

        Spec.Data->SetSetByCallerMagnitude(RPGTags::SetByCaller_Damage, BaseDamage);
        SourceASC->ApplyGameplayEffectSpecToTarget(*Spec.Data.Get(), TargetASC);
    }
}

void URPGMeleeAbility::FinishAttack(bool bWasCancelled)
{
    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, bWasCancelled);
}