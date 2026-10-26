#include "Animation/RPGAnimNotify_GameplayEvent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemGlobals.h"
#include "RPGAbilityTags.h"
#include "Components/SkeletalMeshComponent.h"

URPGAnimNotify_GameplayEvent::URPGAnimNotify_GameplayEvent()
{
    EventTag = RPGTags::Event_Melee_Hit;

#if WITH_EDITORONLY_DATA
    NotifyColor = FColor(230, 60, 40);
#endif
}

FString URPGAnimNotify_GameplayEvent::GetNotifyName_Implementation() const
{
    // The montage timeline shows the tag instead of the class name.
    return EventTag.IsValid() ? EventTag.ToString() : Super::GetNotifyName_Implementation();
}

void URPGAnimNotify_GameplayEvent::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
    const FAnimNotifyEventReference& EventReference)
{
    Super::Notify(MeshComp, Animation, EventReference);

    AActor* Owner = MeshComp ? MeshComp->GetOwner() : nullptr;
    if (!Owner || !EventTag.IsValid()) return;

    // Editor previews and props have no ASC; sending would only log an error.
    if (!UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Owner)) return;

    FGameplayEventData Payload;
    Payload.EventTag = EventTag;
    Payload.Instigator = Owner;
    UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Owner, EventTag, Payload);
}
