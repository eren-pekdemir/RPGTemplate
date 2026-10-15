// RPGCraftingStationComponent.h
// Put on a forge, workbench or alchemy table. The game layer calls BeginUse(Instigator) on interaction.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "RPGCraftingStationComponent.generated.h"

class URPGCraftingComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRPGOnStationUseChangedSignature, AActor*, User);

/**
 * A crafting station. While in use, the user's crafting component has this station as its active one.
 * Use ends when the user closes the crafting screen, moves to another station, or walks too far away.
 */
UCLASS(ClassGroup = (RPG), meta = (BlueprintSpawnableComponent))
class RPGCRAFTING_API URPGCraftingStationComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	URPGCraftingStationComponent();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RPG|Crafting", meta = (Categories = "Crafting.Station"))
	FGameplayTag StationTag;

	/** Title of the crafting screen ("Forge"). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RPG|Crafting")
	FText DisplayName;

	/** Use ends beyond this distance from the station. 0 = never. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RPG|Crafting", meta = (ClampMin = "0", Units = "cm"))
	float MaxUseDistance = 400.f;

	UPROPERTY(BlueprintAssignable, Category = "RPG|Crafting")
	FRPGOnStationUseChangedSignature OnUseStarted;

	UPROPERTY(BlueprintAssignable, Category = "RPG|Crafting")
	FRPGOnStationUseChangedSignature OnUseEnded;

	/** Makes this the user's active station. False if the user has no crafting component or no tag is set. */
	UFUNCTION(BlueprintCallable, Category = "RPG|Crafting")
	bool BeginUse(AActor* User);

	UFUNCTION(BlueprintCallable, Category = "RPG|Crafting")
	void EndUse();

	UFUNCTION(BlueprintPure, Category = "RPG|Crafting")
	AActor* GetUser() const { return CurrentUser.Get(); }

	UFUNCTION(BlueprintPure, Category = "RPG|Crafting")
	bool IsInUse() const { return CurrentUser.IsValid(); }

protected:
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	/** Polled while in use: distance, screen closed, user switched stations. */
	void CheckUser();

	TWeakObjectPtr<AActor> CurrentUser;
	TWeakObjectPtr<URPGCraftingComponent> UserCrafting;
	FTimerHandle CheckTimer;
};
