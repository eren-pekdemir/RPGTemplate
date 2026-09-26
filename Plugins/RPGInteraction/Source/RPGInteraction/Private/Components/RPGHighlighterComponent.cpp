// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/RPGHighlighterComponent.h"
#include "Components/RPGInteractableComponent.h"
#include "Components/RPGInteractorComponent.h"

URPGHighlighterComponent::URPGHighlighterComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void URPGHighlighterComponent::BeginPlay()
{
	Super::BeginPlay();
	
	InteractorComponent = GetOwner()->FindComponentByClass<URPGInteractorComponent>();
	
	if (InteractorComponent.Get())
	{
		InteractorComponent->OnFocusChanged.AddDynamic(this, &URPGHighlighterComponent::HandleFocusChanged);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("%s Interactor Component couldn't find"),*GetOwner()->GetName());
	}
}

void URPGHighlighterComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	StopHighlight();
	
	if (InteractorComponent.Get())
	{
		InteractorComponent->OnFocusChanged.RemoveDynamic(this, &URPGHighlighterComponent::HandleFocusChanged);
	}
	
	Super::EndPlay(EndPlayReason);
}

void URPGHighlighterComponent::HandleFocusChanged(UObject* NewFocus, UObject* OldFocus)
{
	StopHighlight();
	
	if (NewFocus)
	{
		StartHighlight(NewFocus);
	}
}

void URPGHighlighterComponent::StartHighlight(UObject* Focus)
{
	AActor* FocusActor = GetActorFromFocus(Focus);
	if (!FocusActor) return;
	
	URPGInteractableComponent* FocusInteractableComponent = Cast<URPGInteractableComponent>(Focus);
	ERPGHighlightStyle HighlightStyle = DefaultStyle;
	if (FocusInteractableComponent)
	{
		HighlightStyle = FocusInteractableComponent->HighlightStyle;
	}
	
	if (HighlightStyle == ERPGHighlightStyle::Unhighlight) return;
	
	TArray<UPrimitiveComponent*> FocusComponents;
	FocusActor->GetComponents<UPrimitiveComponent>(FocusComponents);
	
	TArray<UPrimitiveComponent*> ComponentsWithTag;
	for (UPrimitiveComponent* Component : FocusComponents)
	{
		if (Component->ComponentHasTag(HighlightTag))
		{
			ComponentsWithTag.Add(Component);
		}
	}
	const TArray<UPrimitiveComponent*>& UsedComponents = (ComponentsWithTag.Num() > 0) ? ComponentsWithTag : FocusComponents;
	
	for (UPrimitiveComponent* Component : UsedComponents)
	{
		Component->SetRenderCustomDepth(true);
		Component->SetCustomDepthStencilValue(static_cast<int32>(HighlightStyle));
		HighlightedComponents.Add(Component);
	}
}

AActor* URPGHighlighterComponent::GetActorFromFocus(UObject* Focus) const
{
	if (!Focus) return nullptr;
	
	AActor* Actor = Cast<AActor>(Focus);
	if (Actor)
	{
		return Actor;
	}
	
	UActorComponent* FocusComponent = Cast<UActorComponent>(Focus);
	if (FocusComponent)
	{
		return FocusComponent->GetOwner();
	}
	
	return nullptr;
}

void URPGHighlighterComponent::StopHighlight()
{
	for (const auto& Component : HighlightedComponents)
	{
		if (Component.Get())
		{
			Component->SetRenderCustomDepth(false);
		}
	}
	
	HighlightedComponents.Empty();
}



