// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/RPGEquipmentVisualsComponent.h"

#include "Components/RPGEquipmentComponent.h"
#include "Items/RPGEquipmentFragment.h"
#include "Items/RPGItemDefinition.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/SkeletalMesh.h"

URPGEquipmentVisualsComponent::URPGEquipmentVisualsComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

// Called when the game starts
void URPGEquipmentVisualsComponent::BeginPlay()
{
	Super::BeginPlay();

	Equipment = GetOwner()->FindComponentByClass<URPGEquipmentComponent>();
	if (!Equipment.Get())
	{
		UE_LOG(LogTemp,Warning, TEXT("No equipment component in the owner"));
		return;
	}
	
	TargetMesh = GetOwner()->FindComponentByTag<USkeletalMeshComponent>(TargetMeshTag);
	if (!TargetMesh.Get())
	{
		TargetMesh = GetOwner()->FindComponentByClass<USkeletalMeshComponent>();
		if (!TargetMesh.Get()) return;
	}
	
	Equipment->OnEquipmentChanged.AddDynamic(this, &URPGEquipmentVisualsComponent::HandleEquipmentChanged);
	
	for (const FGameplayTag Slot : Equipment->AvailableSlots)
	{
		const URPGItemDefinition* Item = Equipment->GetEquippedItem(Slot);
		if (Item)
		{
			CreateVisual(Slot, Item);
		}
	}
}

void URPGEquipmentVisualsComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (Equipment.IsValid())
	{
		Equipment->OnEquipmentChanged.RemoveDynamic(this, &URPGEquipmentVisualsComponent::HandleEquipmentChanged);
	}
	
	for (const TPair<FGameplayTag, TObjectPtr<UPrimitiveComponent>>& Pair : SpawnedVisuals)
	{
		if (Pair.Value)
		{
			Pair.Value->DestroyComponent();
		}
	}
	SpawnedVisuals.Empty();
	
	Super::EndPlay(EndPlayReason);
	
}


void URPGEquipmentVisualsComponent::HandleEquipmentChanged(FGameplayTag Slot, const URPGItemDefinition* NewItem,
                                                           const URPGItemDefinition* OldItem)
{
	RemoveVisual(Slot);
	if (NewItem)
	{
		CreateVisual(Slot, NewItem);
	}
}

void URPGEquipmentVisualsComponent::CreateVisual(FGameplayTag Slot, const URPGItemDefinition* Item)
{
	if (!Item) return;
	const URPGEquipmentFragment* Fragment = Item->FindFragmentByClass<URPGEquipmentFragment>();
	if (!Fragment) return;
	if (!TargetMesh.Get()) return;
	
	if (!Fragment->AttachedStaticMesh.IsNull())
	{
		UStaticMeshComponent* Comp = NewObject<UStaticMeshComponent>(GetOwner());
		Comp->SetStaticMesh(Fragment->AttachedStaticMesh.LoadSynchronous());
		Comp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Comp->SetupAttachment(TargetMesh.Get(), Fragment->AttachSocket);
		Comp->RegisterComponent();
		SpawnedVisuals.Add(Slot, Comp);
	}
	else if (!Fragment->AttachedSkeletalMesh.IsNull())
	{
		USkeletalMeshComponent* Comp = NewObject<USkeletalMeshComponent>(GetOwner());
		Comp->SetSkeletalMesh(Fragment->AttachedSkeletalMesh.LoadSynchronous());
		Comp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Comp->SetupAttachment(TargetMesh.Get(), Fragment->AttachSocket);
		Comp->RegisterComponent();
		Comp->SetLeaderPoseComponent(TargetMesh.Get());
		SpawnedVisuals.Add(Slot, Comp);
	}
}

void URPGEquipmentVisualsComponent::RemoveVisual(FGameplayTag Slot)
{
	TObjectPtr<UPrimitiveComponent> Visual;
	if (SpawnedVisuals.RemoveAndCopyValue(Slot, Visual) && Visual)
	{
		Visual->DestroyComponent();
	}
}
