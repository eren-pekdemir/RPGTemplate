// RPGDialogueComponent.cpp

#include "Components/RPGDialogueComponent.h"

#include "Components/RPGDialogueSpeakerComponent.h"
#include "Dialogue/RPGDialogueActions.h"
#include "Dialogue/RPGDialogueAsset.h"
#include "Engine/Texture2D.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "Interfaces/RPGTagReceiver.h"
#include "RPGCoreStatics.h"
#include "RPGDialogue.h"
#include "TimerManager.h"

URPGDialogueComponent::URPGDialogueComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void URPGDialogueComponent::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PlayerController = GetPlayerController())
	{
		PlayerController->OnPossessedPawnChanged.AddUniqueDynamic(this, &ThisClass::HandlePossessedPawnChanged);
	}
	else
	{
		UE_LOG(LogRPGDialogue, Warning, TEXT("RPGDialogueComponent should be on a PlayerController, not on %s."), *GetNameSafe(GetOwner()));
	}

	PushFlagTags();
}

void URPGDialogueComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (APlayerController* PlayerController = GetPlayerController())
	{
		PlayerController->OnPossessedPawnChanged.RemoveDynamic(this, &ThisClass::HandlePossessedPawnChanged);
	}

	if (const UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(DistanceTimer);
	}

	// Tear down silently: nobody should react to a conversation "ending" because the level is closing.
	ActiveDialogue = nullptr;
	CurrentSpeaker = nullptr;
	CurrentNodeId = NAME_None;
	PendingEvents.Reset();

	Super::EndPlay(EndPlayReason);
}

APlayerController* URPGDialogueComponent::GetPlayerController() const
{
	return Cast<APlayerController>(GetOwner());
}

APawn* URPGDialogueComponent::GetPlayerPawn() const
{
	const APlayerController* PlayerController = GetPlayerController();
	return PlayerController ? PlayerController->GetPawn() : nullptr;
}

URPGDialogueComponent* URPGDialogueComponent::FindDialogueComponent(AActor* Actor)
{
	if (!Actor)
	{
		return nullptr;
	}

	if (URPGDialogueComponent* Direct = Actor->FindComponentByClass<URPGDialogueComponent>())
	{
		return Direct;
	}

	if (const APawn* Pawn = Cast<APawn>(Actor))
	{
		if (const AController* Controller = Pawn->GetController())
		{
			return Controller->FindComponentByClass<URPGDialogueComponent>();
		}
	}
	return nullptr;
}

FRPGDialogueContext URPGDialogueComponent::GetContext() const
{
	FRPGDialogueContext Context;
	Context.Player = GetPlayerPawn();
	Context.Speaker = CurrentSpeaker;
	Context.Dialogue = const_cast<URPGDialogueComponent*>(this);
	return Context;
}

const FRPGDialogueNode* URPGDialogueComponent::GetCurrentNode() const
{
	return ActiveDialogue ? ActiveDialogue->FindNode(CurrentNodeId) : nullptr;
}

// ===================================================================== Conversation

bool URPGDialogueComponent::StartDialogue(URPGDialogueAsset* Dialogue, AActor* InSpeaker)
{
	if (!Dialogue)
	{
		UE_LOG(LogRPGDialogue, Warning, TEXT("StartDialogue: no dialogue asset."));
		return false;
	}

	if (IsInDialogue())
	{
		UE_LOG(LogRPGDialogue, Verbose, TEXT("StartDialogue: already talking to %s."), *GetNameSafe(CurrentSpeaker));
		return false;
	}

	APawn* Pawn = GetPlayerPawn();
	if (!Pawn)
	{
		UE_LOG(LogRPGDialogue, Warning, TEXT("StartDialogue: %s has no pawn (is the component on the PlayerController?)."), *GetNameSafe(GetOwner()));
		return false;
	}

	// State first.
	ActiveDialogue = Dialogue;
	CurrentSpeaker = InSpeaker;
	CurrentNodeId = NAME_None;
	PendingEvents.Reset();

	const FName StartNode = Dialogue->ChooseStartNode(GetContext());
	if (!Dialogue->FindNode(StartNode))
	{
		UE_LOG(LogRPGDialogue, Warning, TEXT("StartDialogue: %s has no node '%s'."), *GetNameSafe(Dialogue), *StartNode.ToString());
		ActiveDialogue = nullptr;
		CurrentSpeaker = nullptr;
		return false;
	}

	if (MaxTalkDistance > 0.f && InSpeaker)
	{
		GetWorld()->GetTimerManager().SetTimer(DistanceTimer, this, &ThisClass::CheckDistance, 0.25f, true);
	}

	OnDialogueStarted.Broadcast(InSpeaker);

	if (URPGDialogueSpeakerComponent* SpeakerComponent = InSpeaker ? InSpeaker->FindComponentByClass<URPGDialogueSpeakerComponent>() : nullptr)
	{
		SpeakerComponent->NotifyTalkStarted(Pawn);
	}

	if (ActiveDialogue == Dialogue)   // a listener may have ended it already
	{
		EnterNode(StartNode);
	}
	return true;
}

void URPGDialogueComponent::EnterNode(FName NodeId)
{
	const FRPGDialogueNode* Node = ActiveDialogue ? ActiveDialogue->FindNode(NodeId) : nullptr;
	if (!Node)
	{
		if (!NodeId.IsNone())
		{
			UE_LOG(LogRPGDialogue, Warning, TEXT("%s: node '%s' does not exist, ending the conversation."), *GetNameSafe(ActiveDialogue), *NodeId.ToString());
		}
		EndDialogue();   // None = normal end
		return;
	}

	CurrentNodeId = NodeId;

	const URPGDialogueAsset* Asset = ActiveDialogue;
	const FRPGDialogueContext Context = GetContext();
	for (const URPGDialogueAction* Action : Node->OnEnterActions)
	{
		if (Action)
		{
			Action->Execute(Context);
		}

		// An action (e.g. a Blueprint one) ended or redirected the conversation.
		if (ActiveDialogue != Asset || CurrentNodeId != NodeId)
		{
			return;
		}
	}

	OnNodeChanged.Broadcast(CurrentNodeId);
}

bool URPGDialogueComponent::SelectChoice(int32 ChoiceIndex)
{
	const FRPGDialogueNode* Node = GetCurrentNode();
	if (!Node || !Node->Choices.IsValidIndex(ChoiceIndex))
	{
		return false;
	}

	const FRPGDialogueChoice& Choice = Node->Choices[ChoiceIndex];
	const FRPGDialogueContext Context = GetContext();
	if (!URPGDialogueAsset::AreConditionsMet(Choice.Conditions, Context))
	{
		return false;
	}

	const URPGDialogueAsset* Asset = ActiveDialogue;
	const FName FromNode = CurrentNodeId;
	const FName NextNode = Choice.NextNode;

	for (const URPGDialogueAction* Action : Choice.Actions)
	{
		if (Action)
		{
			Action->Execute(Context);
		}

		if (ActiveDialogue != Asset || CurrentNodeId != FromNode)
		{
			return true;
		}
	}

	EnterNode(NextNode);
	return true;
}

bool URPGDialogueComponent::Continue()
{
	const FRPGDialogueNode* Node = GetCurrentNode();
	// Also allowed when every choice is hidden by its conditions: the screen then shows "Continue",
	// and refusing it would leave the player stuck in the conversation.
	if (!Node || !GetChoices().IsEmpty())
	{
		return false;
	}

	EnterNode(Node->NextNode);
	return true;
}

void URPGDialogueComponent::EndDialogue()
{
	if (!IsInDialogue())
	{
		return;
	}

	AActor* EndedSpeaker = CurrentSpeaker;
	TArray<FGameplayTag> EventsToSend = MoveTemp(PendingEvents);

	// State first: listeners (and the events below) must see a closed conversation.
	ActiveDialogue = nullptr;
	CurrentSpeaker = nullptr;
	CurrentNodeId = NAME_None;
	PendingEvents.Reset();
	GetWorld()->GetTimerManager().ClearTimer(DistanceTimer);

	OnDialogueEnded.Broadcast(EndedSpeaker);

	if (URPGDialogueSpeakerComponent* SpeakerComponent = IsValid(EndedSpeaker) ? EndedSpeaker->FindComponentByClass<URPGDialogueSpeakerComponent>() : nullptr)
	{
		SpeakerComponent->NotifyTalkEnded(GetPlayerPawn());
	}

	// The dialogue screen is closed now, so a trade screen can open safely.
	for (const FGameplayTag& EventTag : EventsToSend)
	{
		BroadcastEvent(EventTag, EndedSpeaker);
	}
}

void URPGDialogueComponent::CheckDistance()
{
	const APawn* Pawn = GetPlayerPawn();
	if (!Pawn || !IsValid(CurrentSpeaker))
	{
		EndDialogue();
		return;
	}

	if (FVector::DistSquared(Pawn->GetActorLocation(), CurrentSpeaker->GetActorLocation()) > FMath::Square(MaxTalkDistance))
	{
		EndDialogue();
	}
}

void URPGDialogueComponent::HandlePossessedPawnChanged(APawn* OldPawn, APawn* NewPawn)
{
	if (OldPawn && bGrantFlagsAsTags)
	{
		if (IRPGTagReceiver* OldReceiver = URPGCoreStatics::FindTagReceiver(OldPawn))
		{
			OldReceiver->SetGrantedTags(this, FGameplayTagContainer());
		}
	}

	// Dying or switching pawns mid-conversation ends it.
	if (IsInDialogue())
	{
		EndDialogue();
	}

	PushFlagTags();
}

// ===================================================================== Reading

FText URPGDialogueComponent::GetCurrentSpeakerName() const
{
	if (const FRPGDialogueNode* Node = GetCurrentNode(); Node && !Node->SpeakerName.IsEmpty())
	{
		return Node->SpeakerName;
	}

	if (!IsValid(CurrentSpeaker))
	{
		return FText::GetEmpty();
	}

	if (const URPGDialogueSpeakerComponent* SpeakerComponent = CurrentSpeaker->FindComponentByClass<URPGDialogueSpeakerComponent>())
	{
		return SpeakerComponent->GetDisplayName();
	}
	return FText::FromString(CurrentSpeaker->GetActorNameOrLabel());
}

FText URPGDialogueComponent::GetCurrentText() const
{
	const FRPGDialogueNode* Node = GetCurrentNode();
	return Node ? Node->Text : FText::GetEmpty();
}

TSoftObjectPtr<UTexture2D> URPGDialogueComponent::GetCurrentPortrait() const
{
	const URPGDialogueSpeakerComponent* SpeakerComponent =
		IsValid(CurrentSpeaker) ? CurrentSpeaker->FindComponentByClass<URPGDialogueSpeakerComponent>() : nullptr;
	return SpeakerComponent ? SpeakerComponent->Portrait : TSoftObjectPtr<UTexture2D>();
}

TArray<FRPGDialogueChoiceView> URPGDialogueComponent::GetChoices() const
{
	TArray<FRPGDialogueChoiceView> Views;

	const FRPGDialogueNode* Node = GetCurrentNode();
	if (!Node)
	{
		return Views;
	}

	const FRPGDialogueContext Context = GetContext();
	for (int32 Index = 0; Index < Node->Choices.Num(); ++Index)
	{
		const FRPGDialogueChoice& Choice = Node->Choices[Index];
		const bool bAvailable = URPGDialogueAsset::AreConditionsMet(Choice.Conditions, Context);
		if (!bAvailable && Choice.bHideIfUnavailable)
		{
			continue;
		}

		FRPGDialogueChoiceView& View = Views.AddDefaulted_GetRef();
		View.Index = Index;
		View.Text = Choice.Text;
		View.bAvailable = bAvailable;
	}
	return Views;
}

bool URPGDialogueComponent::HasChoices() const
{
	const FRPGDialogueNode* Node = GetCurrentNode();
	return Node && !Node->Choices.IsEmpty();
}

// ===================================================================== Flags

bool URPGDialogueComponent::HasFlag(FGameplayTag Flag) const
{
	return Flag.IsValid() && Flags.HasTag(Flag);
}

bool URPGDialogueComponent::HasFlags(const FGameplayTagContainer& InFlags, bool bRequireAll) const
{
	if (InFlags.IsEmpty())
	{
		return true;
	}
	return bRequireAll ? Flags.HasAll(InFlags) : Flags.HasAny(InFlags);
}

void URPGDialogueComponent::AddFlags(const FGameplayTagContainer& InFlags)
{
	FGameplayTagContainer NewFlags = Flags;
	NewFlags.AppendTags(InFlags);
	SetFlagsInternal(NewFlags);
}

void URPGDialogueComponent::RemoveFlags(const FGameplayTagContainer& InFlags)
{
	FGameplayTagContainer NewFlags = Flags;
	NewFlags.RemoveTags(InFlags);
	SetFlagsInternal(NewFlags);
}

void URPGDialogueComponent::SetFlagsInternal(const FGameplayTagContainer& NewFlags)
{
	// Single gate for every flag change.
	if (NewFlags == Flags)
	{
		return;
	}

	Flags = NewFlags;
	PushFlagTags();
	OnFlagsChanged.Broadcast();
}

void URPGDialogueComponent::PushFlagTags()
{
	if (!bGrantFlagsAsTags)
	{
		return;
	}

	if (IRPGTagReceiver* Receiver = URPGCoreStatics::FindTagReceiver(GetPlayerPawn()))
	{
		Receiver->SetGrantedTags(this, Flags);   // snapshot keyed by source
	}
}

// ===================================================================== Events

void URPGDialogueComponent::SendEvent(FGameplayTag EventTag, bool bWaitForDialogueEnd)
{
	if (!EventTag.IsValid())
	{
		return;
	}

	if (bWaitForDialogueEnd && IsInDialogue())
	{
		PendingEvents.Add(EventTag);
		return;
	}

	BroadcastEvent(EventTag, CurrentSpeaker);
}

void URPGDialogueComponent::BroadcastEvent(FGameplayTag EventTag, AActor* EventSpeaker)
{
	OnDialogueEvent.Broadcast(EventTag, EventSpeaker);

	if (URPGDialogueSpeakerComponent* SpeakerComponent = IsValid(EventSpeaker) ? EventSpeaker->FindComponentByClass<URPGDialogueSpeakerComponent>() : nullptr)
	{
		SpeakerComponent->NotifyDialogueEvent(EventTag, GetPlayerPawn());
	}
}

// ===================================================================== Save

void URPGDialogueComponent::OnLoadedFromSave()
{
	PushFlagTags();
	OnFlagsChanged.Broadcast();
}
