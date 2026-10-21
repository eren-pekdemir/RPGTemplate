// RPGDialogueSpeakerComponent.h
// Put on an NPC. Holds its name, portrait and conversation, and receives dialogue events (open trade, start a fight...).

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "RPGDialogueSpeakerComponent.generated.h"

class UTexture2D;
class URPGDialogueAsset;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRPGSpeakerTalkSignature, AActor*, Player);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FRPGSpeakerEventSignature, FGameplayTag, EventTag, AActor*, Player);

UCLASS(ClassGroup = (RPG), meta = (BlueprintSpawnableComponent))
class RPGDIALOGUE_API URPGDialogueSpeakerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	URPGDialogueSpeakerComponent();

	/** Shown above the text. Empty = actor name. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RPG|Dialogue")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RPG|Dialogue")
	TObjectPtr<URPGDialogueAsset> Dialogue;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RPG|Dialogue")
	TSoftObjectPtr<UTexture2D> Portrait;

	/** A Send Event action ran while talking to this NPC (Dialogue.Event.Trade -> open trade). */
	UPROPERTY(BlueprintAssignable, Category = "RPG|Dialogue")
	FRPGSpeakerEventSignature OnDialogueEvent;

	/** For NPC reactions: face the player, play an idle talk animation. */
	UPROPERTY(BlueprintAssignable, Category = "RPG|Dialogue")
	FRPGSpeakerTalkSignature OnTalkStarted;

	UPROPERTY(BlueprintAssignable, Category = "RPG|Dialogue")
	FRPGSpeakerTalkSignature OnTalkEnded;

	/** Player = the pawn (or its controller). Call from your interaction event. */
	UFUNCTION(BlueprintCallable, Category = "RPG|Dialogue")
	bool StartDialogueWith(AActor* Player);

	UFUNCTION(BlueprintPure, Category = "RPG|Dialogue")
	FText GetDisplayName() const;

	// Called by URPGDialogueComponent.
	void NotifyTalkStarted(AActor* Player) { OnTalkStarted.Broadcast(Player); }
	void NotifyTalkEnded(AActor* Player) { OnTalkEnded.Broadcast(Player); }
	void NotifyDialogueEvent(FGameplayTag EventTag, AActor* Player) { OnDialogueEvent.Broadcast(EventTag, Player); }
};
