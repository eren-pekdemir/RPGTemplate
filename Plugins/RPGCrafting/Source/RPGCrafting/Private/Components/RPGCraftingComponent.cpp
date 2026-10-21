// RPGCraftingComponent.cpp

#include "Components/RPGCraftingComponent.h"

#include "Engine/World.h"
#include "GameplayTagAssetInterface.h"
#include "Interfaces/RPGItemContainer.h"
#include "Items/RPGItemDefinition.h"
#include "Recipes/RPGRecipeDefinition.h"
#include "Recipes/RPGRecipeFragment.h"
#include "RPGCoreStatics.h"
#include "RPGCrafting.h"
#include "TimerManager.h"

URPGCraftingComponent::URPGCraftingComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void URPGCraftingComponent::BeginPlay()
{
	Super::BeginPlay();

	for (const URPGRecipeDefinition* Recipe : StartingRecipes)
	{
		LearnRecipe(Recipe);
	}
}

void URPGCraftingComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// Return the ingredients of an unfinished item and stop the timer.
	CancelCrafting();
	Super::EndPlay(EndPlayReason);
}

void URPGCraftingComponent::OnLoadedFromSave()
{
	// A deleted recipe asset loads as null.
	KnownRecipes.RemoveAll([](const TObjectPtr<const URPGRecipeDefinition>& Recipe) { return Recipe == nullptr; });

	// Starting recipes added to the design after the save was made are still known.
	for (const URPGRecipeDefinition* Recipe : StartingRecipes)
	{
		LearnRecipe(Recipe);
	}
}

// ===================================================================== Recipes

bool URPGCraftingComponent::LearnRecipe(const URPGRecipeDefinition* Recipe)
{
	if (!Recipe || KnowsRecipe(Recipe))
	{
		return false;
	}

	KnownRecipes.Add(Recipe);
	OnRecipeLearned.Broadcast(Recipe);
	return true;
}

bool URPGCraftingComponent::KnowsRecipe(const URPGRecipeDefinition* Recipe) const
{
	return Recipe && KnownRecipes.Contains(Recipe);
}

bool URPGCraftingComponent::LearnRecipeFromItem(const URPGItemDefinition* Item, bool bConsumeItem)
{
	// Learn first, consume second: if removing fails, nothing is lost.
	if (!LearnRecipe(URPGRecipeFragment::GetRecipe(Item)))
	{
		return false;
	}

	if (bConsumeItem)
	{
		if (IRPGItemContainer* Container = GetContainer())
		{
			Container->RemoveItem(Item, 1);
		}
	}
	return true;
}

// ===================================================================== Station

void URPGCraftingComponent::SetActiveStation(FGameplayTag Station, UObject* Source)
{
	if (ActiveStation == Station && ActiveStationSource.Get() == Source)
	{
		return;
	}

	ActiveStation = Station;
	ActiveStationSource = Source;

	// Walking away from the forge stops forging; station-less crafts carry on.
	if (IsCrafting() && !IsAtRequiredStation(ActiveRecipe))
	{
		CancelCrafting();
	}

	OnActiveStationChanged.Broadcast(ActiveStation);
}

void URPGCraftingComponent::ClearActiveStation()
{
	SetActiveStation(FGameplayTag(), nullptr);
}

bool URPGCraftingComponent::IsAtRequiredStation(const URPGRecipeDefinition* Recipe) const
{
	// Direction matters: the ACTIVE station must be the required one or a child of it.
	return Recipe && (!Recipe->RequiredStation.IsValid() || ActiveStation.MatchesTag(Recipe->RequiredStation));
}

// ===================================================================== Queries

IRPGItemContainer* URPGCraftingComponent::GetContainer() const
{
	return URPGCoreStatics::FindItemContainer(GetOwner());
}

ERPGCraftResult URPGCraftingComponent::CheckRequirements(const URPGRecipeDefinition* Recipe) const
{
	if (!Recipe || Recipe->Outputs.IsEmpty())
	{
		return ERPGCraftResult::InvalidRecipe;
	}

	if (!KnowsRecipe(Recipe))
	{
		return ERPGCraftResult::UnknownRecipe;
	}

	if (!IsAtRequiredStation(Recipe))
	{
		return ERPGCraftResult::WrongStation;
	}

	if (!GetMissingRequirements(Recipe).IsEmpty())
	{
		return ERPGCraftResult::MissingRequirements;
	}

	return ERPGCraftResult::Success;
}

FGameplayTagContainer URPGCraftingComponent::GetMissingRequirements(const URPGRecipeDefinition* Recipe) const
{
	FGameplayTagContainer Missing;
	if (!Recipe || Recipe->RequiredTags.IsEmpty())
	{
		return Missing;
	}

	// Engine interface: implemented by the ability system component (loose tags included), or by anything else.
	FGameplayTagContainer OwnedTags;
	if (const IGameplayTagAssetInterface* TagSource =
			URPGCoreStatics::FindInterface<IGameplayTagAssetInterface, UGameplayTagAssetInterface>(GetOwner()))
	{
		TagSource->GetOwnedGameplayTags(OwnedTags);
	}

	for (const FGameplayTag& Required : Recipe->RequiredTags)
	{
		if (!OwnedTags.HasTag(Required))
		{
			Missing.AddTag(Required);
		}
	}
	return Missing;
}

int32 URPGCraftingComponent::GetMaxCraftable(const URPGRecipeDefinition* Recipe) const
{
	const IRPGItemContainer* Container = GetContainer();
	if (!Recipe || !Container)
	{
		return 0;
	}

	int32 MaxCount = MaxCraftCount;
	for (const FRPGItemQuantity& Ingredient : Recipe->Ingredients)
	{
		if (Ingredient.IsValid())
		{
			// Integer division on purpose: 7 ore / 3 per craft = 2 crafts.
			MaxCount = FMath::Min(MaxCount, Container->GetItemCount(Ingredient.Item) / Ingredient.Quantity);
		}
	}
	return MaxCount;
}

ERPGCraftResult URPGCraftingComponent::CanCraft(const URPGRecipeDefinition* Recipe, int32 Count) const
{
	const ERPGCraftResult Requirements = CheckRequirements(Recipe);
	if (Requirements != ERPGCraftResult::Success)
	{
		return Requirements;
	}

	if (!GetContainer())
	{
		return ERPGCraftResult::NoContainer;
	}

	return GetMaxCraftable(Recipe) >= FMath::Max(Count, 1)
		? ERPGCraftResult::Success
		: ERPGCraftResult::MissingIngredients;
}

// ===================================================================== Instant crafting

ERPGCraftResult URPGCraftingComponent::Craft(const URPGRecipeDefinition* Recipe, int32 Count, int32& OutCraftedCount)
{
	OutCraftedCount = 0;

	if (IsCrafting())
	{
		return ERPGCraftResult::Busy;
	}

	Count = FMath::Clamp(Count, 1, MaxCraftCount);

	const ERPGCraftResult Requirements = CheckRequirements(Recipe);
	if (Requirements != ERPGCraftResult::Success)
	{
		return Requirements;
	}

	IRPGItemContainer* Container = GetContainer();
	if (!Container)
	{
		return ERPGCraftResult::NoContainer;
	}

	ERPGCraftResult Result = ERPGCraftResult::Success;
	while (OutCraftedCount < Count)
	{
		Result = ExecuteCraft(*Recipe, *Container);
		if (Result != ERPGCraftResult::Success)
		{
			break;
		}
		++OutCraftedCount;
	}

	if (OutCraftedCount > 0)
	{
		OnItemCrafted.Broadcast(Recipe, OutCraftedCount);
	}

	return Result;
}

ERPGCraftResult URPGCraftingComponent::ExecuteCraft(const URPGRecipeDefinition& Recipe, IRPGItemContainer& Container)
{
	TArray<FRPGItemQuantity> Taken;
	if (!TakeIngredients(Recipe, Container, Taken))
	{
		return ERPGCraftResult::MissingIngredients;
	}
	return GiveOutputs(Recipe, Container, Taken);
}

bool URPGCraftingComponent::TakeIngredients(const URPGRecipeDefinition& Recipe, IRPGItemContainer& Container,
	TArray<FRPGItemQuantity>& OutTaken)
{
	OutTaken.Reset();

	for (const FRPGItemQuantity& Ingredient : Recipe.Ingredients)
	{
		if (!Ingredient.IsValid())
		{
			continue;
		}

		const int32 Removed = Container.RemoveItem(Ingredient.Item, Ingredient.Quantity);
		if (Removed > 0)
		{
			OutTaken.Emplace(Ingredient.Item, Removed);
		}

		if (Removed < Ingredient.Quantity)
		{
			Rollback(Container, OutTaken, TArray<FRPGItemQuantity>());
			OutTaken.Reset();
			return false;
		}
	}
	return true;
}

ERPGCraftResult URPGCraftingComponent::GiveOutputs(const URPGRecipeDefinition& Recipe, IRPGItemContainer& Container,
	const TArray<FRPGItemQuantity>& Taken)
{
	TArray<FRPGItemQuantity> Given;

	for (const FRPGItemQuantity& Output : Recipe.Outputs)
	{
		if (!Output.IsValid())
		{
			continue;
		}

		const int32 Added = Container.AddItem(Output.Item, Output.Quantity);
		if (Added > 0)
		{
			Given.Emplace(Output.Item, Added);
		}

		if (Added < Output.Quantity)
		{
			Rollback(Container, Taken, Given);
			return ERPGCraftResult::NoRoom;
		}
	}
	return ERPGCraftResult::Success;
}

void URPGCraftingComponent::Rollback(IRPGItemContainer& Container,
	const TArray<FRPGItemQuantity>& Taken, const TArray<FRPGItemQuantity>& Given)
{
	// Outputs first: removing them frees the space the ingredients need to come back.
	for (const FRPGItemQuantity& Output : Given)
	{
		Container.RemoveItem(Output.Item, Output.Quantity);
	}

	for (const FRPGItemQuantity& Ingredient : Taken)
	{
		const int32 Returned = Container.AddItem(Ingredient.Item, Ingredient.Quantity);
		if (Returned < Ingredient.Quantity)
		{
			// Should be impossible: these items fitted a moment ago.
			UE_LOG(LogRPGCrafting, Error, TEXT("Rollback could not return %d x %s."),
				Ingredient.Quantity - Returned, *GetNameSafe(Ingredient.Item));
		}
	}
}

// ===================================================================== Timed crafting

ERPGCraftResult URPGCraftingComponent::StartCrafting(const URPGRecipeDefinition* Recipe, int32 Count)
{
	if (IsCrafting())
	{
		return ERPGCraftResult::Busy;
	}

	const ERPGCraftResult CanStart = CanCraft(Recipe, 1);
	if (CanStart != ERPGCraftResult::Success)
	{
		return CanStart;
	}

	Count = FMath::Clamp(Count, 1, MaxCraftCount);

	// Instant recipes: one call, same events as a timed job.
	if (Recipe->CraftTime <= 0.f)
	{
		OnCraftingStarted.Broadcast(Recipe, Count);

		int32 Crafted = 0;
		const ERPGCraftResult Result = Craft(Recipe, Count, Crafted);
		OnCraftingFinished.Broadcast(Recipe, Crafted, Result);
		return ERPGCraftResult::Success;
	}

	// State first, then the event.
	ActiveRecipe = Recipe;
	JobCrafted = 0;
	JobRemaining = Count;

	OnCraftingStarted.Broadcast(Recipe, Count);
	BeginNextTimedCraft();
	return ERPGCraftResult::Success;
}

void URPGCraftingComponent::BeginNextTimedCraft()
{
	// Checked per item: a skill could have been lost since the job started.
	const ERPGCraftResult Requirements = CheckRequirements(ActiveRecipe);
	if (Requirements != ERPGCraftResult::Success)
	{
		FinishJob(Requirements);
		return;
	}

	IRPGItemContainer* Container = GetContainer();
	if (!Container)
	{
		FinishJob(ERPGCraftResult::NoContainer);
		return;
	}

	// Ingredients are taken when an item STARTS, so they cannot be used twice; cancelling returns them.
	if (!TakeIngredients(*ActiveRecipe, *Container, PendingIngredients))
	{
		FinishJob(ERPGCraftResult::MissingIngredients);
		return;
	}

	GetWorld()->GetTimerManager().SetTimer(CraftTimer, this, &ThisClass::HandleCraftTimerFinished,
		ActiveRecipe->CraftTime, false);
}

void URPGCraftingComponent::HandleCraftTimerFinished()
{
	IRPGItemContainer* Container = GetContainer();
	if (!Container)
	{
		PendingIngredients.Reset();
		FinishJob(ERPGCraftResult::NoContainer);
		return;
	}

	// On failure GiveOutputs has already returned the pending ingredients.
	const ERPGCraftResult Result = GiveOutputs(*ActiveRecipe, *Container, PendingIngredients);
	PendingIngredients.Reset();

	if (Result != ERPGCraftResult::Success)
	{
		FinishJob(Result);
		return;
	}

	++JobCrafted;
	--JobRemaining;
	OnItemCrafted.Broadcast(ActiveRecipe.Get(), 1);

	// A listener may have cancelled the job.
	if (!IsCrafting())
	{
		return;
	}

	if (JobRemaining > 0)
	{
		BeginNextTimedCraft();
	}
	else
	{
		FinishJob(ERPGCraftResult::Success);
	}
}

void URPGCraftingComponent::CancelCrafting()
{
	if (!IsCrafting())
	{
		return;
	}

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(CraftTimer);
	}

	if (IRPGItemContainer* Container = GetContainer())
	{
		Rollback(*Container, PendingIngredients, TArray<FRPGItemQuantity>());
	}
	PendingIngredients.Reset();

	FinishJob(ERPGCraftResult::Cancelled);
}

void URPGCraftingComponent::FinishJob(ERPGCraftResult Result)
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(CraftTimer);
	}

	const URPGRecipeDefinition* Recipe = ActiveRecipe;
	const int32 Crafted = JobCrafted;

	// State first: a listener may start the next job right away.
	ActiveRecipe = nullptr;
	JobCrafted = 0;
	JobRemaining = 0;
	PendingIngredients.Reset();

	OnCraftingFinished.Broadcast(Recipe, Crafted, Result);
}

float URPGCraftingComponent::GetCraftProgress() const
{
	const UWorld* World = GetWorld();
	if (!ActiveRecipe || ActiveRecipe->CraftTime <= 0.f || !World)
	{
		return 0.f;
	}

	const float Elapsed = World->GetTimerManager().GetTimerElapsed(CraftTimer);
	return Elapsed > 0.f ? FMath::Clamp(Elapsed / ActiveRecipe->CraftTime, 0.f, 1.f) : 0.f;
}
