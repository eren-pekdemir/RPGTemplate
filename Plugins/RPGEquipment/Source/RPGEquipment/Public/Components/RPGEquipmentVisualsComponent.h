#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "RPGEquipmentVisualsComponent.generated.h"

class URPGEquipmentComponent;
class URPGItemDefinition;
class USkeletalMeshComponent;
class UPrimitiveComponent;

UCLASS(ClassGroup=(RPG), meta=(BlueprintSpawnableComponent))
class RPGEQUIPMENT_API URPGEquipmentVisualsComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	URPGEquipmentVisualsComponent();

	/** Component tag of the skeletal mesh to attach equipment to. Falls back to the first skeletal mesh. */
	UPROPERTY(EditAnywhere, Category = "Equipment|Visuals")
	FName TargetMeshTag = TEXT("EquipmentTarget");

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	/** Listens to the equipment component: removes the old visual, creates the new one. */
	UFUNCTION()
	void HandleEquipmentChanged(FGameplayTag Slot, const URPGItemDefinition* NewItem, const URPGItemDefinition* OldItem);

	/** Spawns the mesh described by the item's equipment fragment and attaches it. */
	void CreateVisual(FGameplayTag Slot, const URPGItemDefinition* Item);

	/** Destroys the mesh spawned for this slot, if any. */
	void RemoveVisual(FGameplayTag Slot);

	TWeakObjectPtr<URPGEquipmentComponent> Equipment;
	TWeakObjectPtr<USkeletalMeshComponent> TargetMesh;

	UPROPERTY()
	TMap<FGameplayTag, TObjectPtr<UPrimitiveComponent>> SpawnedVisuals;
};