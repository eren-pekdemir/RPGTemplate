// RPGDialogueComponent.h
// Lives on the PlayerController. Runs one conversation at a time and stores the player's dialogue flags (saved).

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "Interfaces/RPGSaveable.h"
#include "RPGDialogueTypes.h"
#include "RPGDialogueComponent.generated.h"

class APawn;
class APlayerController;
class UTexture2D;
class URPGDialogueAsset;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRPGDialogueSpeakerSignature, AActor*, Speaker);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRPGDialogueNodeChangedSignature, FName, NodeId);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FRPGDialogueEventSignature, FGameplayTag, EventTag, AActor*, Speaker);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FRPGDialogueFlagsChangedSignature);

UCLASS(ClassGroup = (RPG), meta = (BlueprintSpawnableComponent))
class RPGDIALOGUE_API URPGDialogueComponent : public UActorComponent, public IRPGSaveable
{
	GENERATED_BODY()

public:
	URPGDialogueComponent();

	/** The conversation ends when the player walks further than this from the speaker. 0 = never. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RPG|Dialogue", meta = (ClampMin = "0", Units = "cm"))
	float MaxTalkDistance = 500.f;

	/** Also give flags to the pawn as gameplay tags (Has Tags conditions, ability requirements). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RPG|Dialogue")
	bool bGrantFlagsAsTags = true;

	UPROPERTY(BlueprintAssignable, Category = "RPG|Dialogue")
	FRPGDialogueSpeakerSignature OnDialogueStarted;

	/** A new line is shown (or the choices changed). */
	UPROPERTY(BlueprintAssignable, Category = "RPG|Dialogue")
	FRPGDialogueNodeChangedSignature OnNodeChanged;

	UPROPERTY(BlueprintAssignable, Category = "RPG|Dialogue")
	FRPGDialogueSpeakerSignature OnDialogueEnded;

	/** "Send Event" actions. Also forwarded to the speaker's RPGDialogueSpeakerComponent. */
	UPROPERTY(BlueprintAssignable, Category = "RPG|Dialogue")
	FRPGDialogueEventSignature OnDialogueEvent;

	UPROPERTY(BlueprintAssignable, Category = "RPG|Dialogue")
	FRPGDialogueFlagsChangedSignature OnFlagsChanged;

	// ------------------------------------------------------------ Conversation

	/** False if a conversation is already running or the asset has no valid start. */
	UFUNCTION(BlueprintCallable, Category = "RPG|Dialogue")
	bool StartDialogue(URPGDialogueAsset* Dialogue, AActor* InSpeaker);

	UFUNCTION(BlueprintCallable, Category = "RPG|Dialogue")
	void EndDialogue();

	/** Index from GetChoices (FRPGDialogueChoiceView::Index). False if unavailable. */
	UFUNCTION(BlueprintCallable, Category = "RPG|Dialogue")
	bool SelectChoice(int32 ChoiceIndex);

	/** For nodes without (visible) choices: go to NextNode (or end). */
	UFUNCTION(BlueprintCallable, Category = "RPG|Dialogue")
	bool Continue();

	UFUNCTION(BlueprintPure, Category = "RPG|Dialogue")
	bool IsInDialogue() const { return ActiveDialogue != nullptr; }

	UFUNCTION(BlueprintPure, Category = "RPG|Dialogue")
	AActor* GetSpeaker() const { return CurrentSpeaker; }

	UFUNCTION(BlueprintPure, Category = "RPG|Dialogue")
	FName GetCurrentNodeId() const { return CurrentNodeId; }

	UFUNCTION(BlueprintPure, Category = "RPG|Dialogue")
	FText GetCurrentSpeakerName() const;

	UFUNCTION(BlueprintPure, Category = "RPG|Dialogue")
	FText GetCurrentText() const;

	/** The speaker component's portrait, or null. */
	UFUNCTION(BlueprintPure, Category = "RPG|Dialogue")
	TSoftObjectPtr<UTexture2D> GetCurrentPortrait() const;

	/** Visible choices of the current node. Empty = show "Continue". */
	UFUNCTION(BlueprintPure, Category = "RPG|Dialogue")
	TArray<FRPGDialogueChoiceView> GetChoices() const;

	UFUNCTION(BlueprintPure, Category = "RPG|Dialogue")
	bool HasChoices() const;

	UFUNCTION(BlueprintPure, Category = "RPG|Dialogue")
	FRPGDialogueContext GetContext() const;

	// ------------------------------------------------------------ Flags

	UFUNCTION(BlueprintPure, Category = "RPG|Dialogue")
	bool HasFlag(FGameplayTag Flag) const;

	UFUNCTION(BlueprintPure, Category = "RPG|Dialogue")
	bool HasFlags(const FGameplayTagContainer& InFlags, bool bRequireAll = true) const;

	UFUNCTION(BlueprintCallable, Category = "RPG|Dialogue")
	void AddFlags(const FGameplayTagContainer& InFlags);

	UFUNCTION(BlueprintCallable, Category = "RPG|Dialogue")
	void RemoveFlags(const FGameplayTagContainer& InFlags);

	UFUNCTION(BlueprintPure, Category = "RPG|Dialogue")
	FGameplayTagContainer GetFlags() const { return Flags; }

	// ------------------------------------------------------------ Events

	/** bWaitForDialogueEnd: hold the event until the conversation closes (so a trade screen does not open under it). */
	UFUNCTION(BlueprintCallable, Category = "RPG|Dialogue")
	void SendEvent(FGameplayTag EventTag, bool bWaitForDialogueEnd = true);

	/** Actor itself, a pawn's controller, or a controller. */
	UFUNCTION(BlueprintPure, Category = "RPG|Dialogue")
	static URPGDialogueComponent* FindDialogueComponent(AActor* Actor);

	// IRPGSaveable
	virtual void OnLoadedFromSave() override;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	APlayerController* GetPlayerController() const;
	APawn* GetPlayerPawn() const;
	const FRPGDialogueNode* GetCurrentNode() const;

	void EnterNode(FName NodeId);
	void SetFlagsInternal(const FGameplayTagContainer& NewFlags);
	void PushFlagTags();
	void BroadcastEvent(FGameplayTag EventTag, AActor* EventSpeaker);
	void CheckDistance();

	UFUNCTION()
	void HandlePossessedPawnChanged(APawn* OldPawn, APawn* NewPawn);

	UPROPERTY(SaveGame)
	FGameplayTagContainer Flags;

	UPROPERTY(Transient)
	TObjectPtr<URPGDialogueAsset> ActiveDialogue;

	UPROPERTY(Transient)
	TObjectPtr<AActor> CurrentSpeaker;

	FName CurrentNodeId;

	/** Events held by SendEvent(..., true) until EndDialogue. */
	TArray<FGameplayTag> PendingEvents;

	FTimerHandle DistanceTimer;
};
