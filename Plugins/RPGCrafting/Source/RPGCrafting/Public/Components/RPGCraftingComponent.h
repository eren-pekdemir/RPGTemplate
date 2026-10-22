// RPGCraftingComponent.h
// Known recipes, active station and crafting (instant and timed) for its owner.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "Items/RPGItemQuantity.h"
#include "RPGCraftingTypes.h"
#include "Interfaces/RPGSaveable.h"
#include "RPGCraftingComponent.generated.h"

class IRPGItemContainer;
class URPGItemDefinition;
class URPGRecipeDefinition;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRPGOnRecipeLearnedSignature,
	const URPGRecipeDefinition*, Recipe);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FRPGOnItemCraftedSignature,
	const URPGRecipeDefinition*, Recipe, int32, CraftedCount);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FRPGOnCraftingStartedSignature,
	const URPGRecipeDefinition*, Recipe, int32, Count);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FRPGOnCraftingFinishedSignature,
	const URPGRecipeDefinition*, Recipe, int32, CraftedCount, ERPGCraftResult, Result);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRPGOnActiveStationChangedSignature,
	FGameplayTag, Station);

/**
 * Known recipes and crafting for its owner (usually the pawn).
 * Ingredients come from, and outputs go to, the owner's IRPGItemContainer.
 * Every single craft is all-or-nothing: items are never lost or duplicated.
 */
UCLASS(ClassGroup = (RPG), meta = (BlueprintSpawnableComponent))
class RPGCRAFTING_API URPGCraftingComponent : public UActorComponent, public IRPGSaveable
{
	GENERATED_BODY()

public:
	URPGCraftingComponent();

	/** Upper limit for one batch (also the cap for recipes without ingredients). */
	static constexpr int32 MaxCraftCount = 99;

	// ------------------------------------------------------------------ Settings & state

	UPROPERTY(EditAnywhere, Category = "RPG|Crafting")
	TArray<TObjectPtr<const URPGRecipeDefinition>> StartingRecipes;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, SaveGame, Category = "RPG|Crafting")
	TArray<TObjectPtr<const URPGRecipeDefinition>> KnownRecipes;

	// ------------------------------------------------------------------ Events

	UPROPERTY(BlueprintAssignable, Category = "RPG|Crafting")
	FRPGOnRecipeLearnedSignature OnRecipeLearned;

	/** Outputs were added: once per instant batch, once per item for timed crafts. */
	UPROPERTY(BlueprintAssignable, Category = "RPG|Crafting")
	FRPGOnItemCraftedSignature OnItemCrafted;

	/** A job started through StartCrafting (instant or timed). */
	UPROPERTY(BlueprintAssignable, Category = "RPG|Crafting")
	FRPGOnCraftingStartedSignature OnCraftingStarted;

	/** A job ended: completed, failed or cancelled. CraftedCount items were made before it ended. */
	UPROPERTY(BlueprintAssignable, Category = "RPG|Crafting")
	FRPGOnCraftingFinishedSignature OnCraftingFinished;

	UPROPERTY(BlueprintAssignable, Category = "RPG|Crafting")
	FRPGOnActiveStationChangedSignature OnActiveStationChanged;

	// ------------------------------------------------------------------ Recipes

	/** False if the recipe is null or already known. */
	UFUNCTION(BlueprintCallable, Category = "RPG|Crafting")
	bool LearnRecipe(const URPGRecipeDefinition* Recipe);

	UFUNCTION(BlueprintPure, Category = "RPG|Crafting")
	bool KnowsRecipe(const URPGRecipeDefinition* Recipe) const;

	/**
	 * Learns the recipe taught by an item with a Recipe fragment (a scroll).
	 * Consumes one item if requested and the recipe was new. False if the item teaches nothing new; the item is kept.
	 */
	UFUNCTION(BlueprintCallable, Category = "RPG|Crafting")
	bool LearnRecipeFromItem(const URPGItemDefinition* Item, bool bConsumeItem = true);

	// ------------------------------------------------------------------ Station

	/**
	 * Called by a station when the player starts using it. Empty tag = no station.
	 * Changing the station cancels a timed craft that needs the old one.
	 */
	UFUNCTION(BlueprintCallable, Category = "RPG|Crafting")
	void SetActiveStation(FGameplayTag Station, UObject* Source = nullptr);

	UFUNCTION(BlueprintCallable, Category = "RPG|Crafting")
	void ClearActiveStation();

	UFUNCTION(BlueprintPure, Category = "RPG|Crafting")
	FGameplayTag GetActiveStation() const { return ActiveStation; }

	/** The object that set the active station (usually a URPGCraftingStationComponent). */
	UFUNCTION(BlueprintPure, Category = "RPG|Crafting")
	UObject* GetActiveStationSource() const { return ActiveStationSource.Get(); }

	/** No station needed, or the active one is it or a child of it (Forge.Master counts as Forge). */
	UFUNCTION(BlueprintPure, Category = "RPG|Crafting")
	bool IsAtRequiredStation(const URPGRecipeDefinition* Recipe) const;

	// ------------------------------------------------------------------ Queries

	/**
	 * Can Count crafts start? Success means the requirements and ingredients are there;
	 * a craft can still fail with NoRoom, which is only known while crafting.
	 */
	UFUNCTION(BlueprintPure, Category = "RPG|Crafting")
	ERPGCraftResult CanCraft(const URPGRecipeDefinition* Recipe, int32 Count = 1) const;

	/** How many times the recipe can be made with the current ingredients (ignores requirements). */
	UFUNCTION(BlueprintPure, Category = "RPG|Crafting")
	int32 GetMaxCraftable(const URPGRecipeDefinition* Recipe) const;

	/** Required tags the crafter does not have, e.g. for "Requires: Smithing" in the UI. */
	UFUNCTION(BlueprintPure, Category = "RPG|Crafting")
	FGameplayTagContainer GetMissingRequirements(const URPGRecipeDefinition* Recipe) const;

	// ------------------------------------------------------------------ Instant crafting

	/**
	 * Crafts up to Count times, instantly, ignoring CraftTime. Stops at the first failure:
	 * OutCraftedCount says how many succeeded, the return value says why it stopped (Success if all were made).
	 */
	UFUNCTION(BlueprintCallable, Category = "RPG|Crafting")
	ERPGCraftResult Craft(const URPGRecipeDefinition* Recipe, int32 Count, int32& OutCraftedCount);

	// ------------------------------------------------------------------ Timed crafting

	/**
	 * Starts a job of Count crafts. Recipes with CraftTime 0 finish immediately.
	 * Returns why the job could not start, or Success; the outcome arrives in OnCraftingFinished.
	 */
	UFUNCTION(BlueprintCallable, Category = "RPG|Crafting")
	ERPGCraftResult StartCrafting(const URPGRecipeDefinition* Recipe, int32 Count = 1);

	/** Stops the job; the current item's ingredients are returned. Finished items are kept. */
	UFUNCTION(BlueprintCallable, Category = "RPG|Crafting")
	void CancelCrafting();

	UFUNCTION(BlueprintPure, Category = "RPG|Crafting")
	bool IsCrafting() const { return ActiveRecipe != nullptr; }

	UFUNCTION(BlueprintPure, Category = "RPG|Crafting")
	const URPGRecipeDefinition* GetActiveRecipe() const { return ActiveRecipe; }

	/** Progress of the current item, 0..1. */
	UFUNCTION(BlueprintPure, Category = "RPG|Crafting")
	float GetCraftProgress() const;

	/** Items finished in the current job. */
	UFUNCTION(BlueprintPure, Category = "RPG|Crafting")
	int32 GetCraftedCount() const { return JobCrafted; }

	/** Items still to make in the current job, including the one in progress. */
	UFUNCTION(BlueprintPure, Category = "RPG|Crafting")
	int32 GetRemainingCount() const { return JobRemaining; }

	// IRPGSaveable
	virtual void OnLoadedFromSave() override;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	IRPGItemContainer* GetContainer() const;

	/** Everything except ingredients: recipe valid, known, station, required tags. */
	ERPGCraftResult CheckRequirements(const URPGRecipeDefinition* Recipe) const;

	/** One all-or-nothing craft. */
	ERPGCraftResult ExecuteCraft(const URPGRecipeDefinition& Recipe, IRPGItemContainer& Container);

	/** Removes one craft's ingredients. On failure everything taken is returned and OutTaken is empty. */
	static bool TakeIngredients(const URPGRecipeDefinition& Recipe, IRPGItemContainer& Container,
		TArray<FRPGItemQuantity>& OutTaken);

	/** Adds one craft's outputs. On failure the outputs are removed and Taken is returned. */
	static ERPGCraftResult GiveOutputs(const URPGRecipeDefinition& Recipe, IRPGItemContainer& Container,
		const TArray<FRPGItemQuantity>& Taken);

	static void Rollback(IRPGItemContainer& Container,
		const TArray<FRPGItemQuantity>& Taken, const TArray<FRPGItemQuantity>& Given);

	void BeginNextTimedCraft();
	void HandleCraftTimerFinished();
	void FinishJob(ERPGCraftResult Result);

	FGameplayTag ActiveStation;
	TWeakObjectPtr<UObject> ActiveStationSource;

	UPROPERTY()
	TObjectPtr<const URPGRecipeDefinition> ActiveRecipe;

	/**
	 * Ingredients taken for the item in progress; returned on cancel.
	 * SaveGame: they are already out of the inventory, so a save made mid-craft keeps them and the load refunds them.
	 */
	UPROPERTY(SaveGame)
	TArray<FRPGItemQuantity> PendingIngredients;

	int32 JobCrafted = 0;
	int32 JobRemaining = 0;
	FTimerHandle CraftTimer;
};
