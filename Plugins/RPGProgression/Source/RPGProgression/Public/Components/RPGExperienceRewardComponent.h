#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interfaces/RPGSaveable.h"
#include "RPGExperienceRewardComponent.generated.h"

/**
 * Experience this actor is worth. The game layer calls GrantReward(Killer) when it dies;
 * the reward is given at most once.
 */
UCLASS(ClassGroup = (RPG), meta = (BlueprintSpawnableComponent))
class RPGPROGRESSION_API URPGExperienceRewardComponent : public UActorComponent, public IRPGSaveable
{
	GENERATED_BODY()

public:
	URPGExperienceRewardComponent();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RPG|Reward", meta = (ClampMin = "0"))
	int32 ExperienceReward = 50;

	/** Gives the reward to Receiver's progression (pawn, controller or instigator). Returns true if granted. */
	UFUNCTION(BlueprintCallable, Category = "RPG|Reward")
	bool GrantReward(AActor* Receiver);

	UFUNCTION(BlueprintPure, Category = "RPG|Reward")
	bool HasGrantedReward() const { return bRewardGranted; }

private:
	UPROPERTY(SaveGame)
	bool bRewardGranted = false;
	
};