// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/RPGItemPickup.h"
#include "RPGCoreStatics.h"
#include "Items/RPGInventoryFragment.h"
#include "Components/StaticMeshComponent.h"
#include "Items/RPGItemDefinition.h"
#include "Interfaces/RPGItemContainer.h"

#define LOCTEXT_NAMESPACE "RPGItemPickup"

// Sets default values
ARPGItemPickup::ARPGItemPickup()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);
}

void ARPGItemPickup::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	
	if (!Item) return;

	const URPGInventoryFragment* ItemFragment = Item->FindFragmentByClass<URPGInventoryFragment>();
	
	if (!ItemFragment) return;
	if (!ItemFragment->WorldMesh.IsNull())
	{
		Mesh->SetStaticMesh(ItemFragment->WorldMesh.LoadSynchronous());
	}
}

bool ARPGItemPickup::CanInteract_Implementation(AActor* InteractionInstigator) const
{
	return Item && Quantity > 0;
}

FText ARPGItemPickup::GetInteractionPrompt_Implementation(AActor* InteractionInstigator) const
{
	if (!Item) return FText::GetEmpty();

	if (const IRPGItemContainer* Container = URPGCoreStatics::FindItemContainer(InteractionInstigator))
	{
		if (!Container->CanAddItem(Item, 1))
		{
			return LOCTEXT("InventoryFull", "Inventory Full");
		}
	}

	if (Quantity > 1)
	{
		return FText::Format(
			LOCTEXT("PickupPromptQuantity", "Pick up {0} x{1}"),
			Item->DisplayName,
			FText::AsNumber(Quantity));
	}

	return FText::Format(LOCTEXT("PickupPrompt", "Pick up {0}"), Item->DisplayName);
}

float ARPGItemPickup::GetInteractionDuration_Implementation() const
{
	return 0.f;
}

void ARPGItemPickup::Interact_Implementation(AActor* InteractionInstigator)
{
	if (!Item || Quantity <= 0 ) return;
	
	IRPGItemContainer* Container = URPGCoreStatics::FindItemContainer(InteractionInstigator);
	if (!Container) return;
	
	const int32 Added = Container->AddItem(Item ,Quantity);
	Quantity -= Added;
	if (Quantity <= 0) Destroy();
}

#undef LOCTEXT_NAMESPACE