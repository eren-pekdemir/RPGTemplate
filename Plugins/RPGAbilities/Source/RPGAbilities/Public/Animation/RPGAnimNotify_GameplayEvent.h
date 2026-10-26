#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "GameplayTagContainer.h"
#include "RPGAnimNotify_GameplayEvent.generated.h"

/**
 * Sends EventTag to the owning actor as a gameplay event when the animation reaches this notify.
 * Put Event.Melee.Hit on the frame where the blade passes the target: URPGMeleeAbility waits for it and sweeps then.
 * Owners without an ability system (e.g. the montage editor preview) are ignored.
 */
UCLASS(meta = (DisplayName = "Gameplay Event"))
class RPGABILITIES_API URPGAnimNotify_GameplayEvent : public UAnimNotify
{
    GENERATED_BODY()

public:
    URPGAnimNotify_GameplayEvent();

    /** Event sent to the owner. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RPG", meta = (Categories = "Event"))
    FGameplayTag EventTag;

    virtual FString GetNotifyName_Implementation() const override;

    virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
        const FAnimNotifyEventReference& EventReference) override;
};
