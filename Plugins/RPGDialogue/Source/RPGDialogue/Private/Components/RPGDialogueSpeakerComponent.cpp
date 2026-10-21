// RPGDialogueSpeakerComponent.cpp

#include "Components/RPGDialogueSpeakerComponent.h"

#include "Components/RPGDialogueComponent.h"
#include "RPGDialogue.h"

URPGDialogueSpeakerComponent::URPGDialogueSpeakerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

FText URPGDialogueSpeakerComponent::GetDisplayName() const
{
	if (!DisplayName.IsEmpty())
	{
		return DisplayName;
	}
	return GetOwner() ? FText::FromString(GetOwner()->GetActorNameOrLabel()) : FText::GetEmpty();
}

bool URPGDialogueSpeakerComponent::StartDialogueWith(AActor* Player)
{
	URPGDialogueComponent* DialogueComponent = URPGDialogueComponent::FindDialogueComponent(Player);
	if (!DialogueComponent)
	{
		UE_LOG(LogRPGDialogue, Warning, TEXT("StartDialogueWith: %s has no RPGDialogueComponent on its PlayerController."), *GetNameSafe(Player));
		return false;
	}

	if (!Dialogue)
	{
		UE_LOG(LogRPGDialogue, Warning, TEXT("StartDialogueWith: %s has no Dialogue asset."), *GetNameSafe(GetOwner()));
		return false;
	}

	return DialogueComponent->StartDialogue(Dialogue, GetOwner());
}
