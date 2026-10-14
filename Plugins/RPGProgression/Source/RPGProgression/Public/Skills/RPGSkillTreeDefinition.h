#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "RPGSkillTreeDefinition.generated.h"

class URPGSkillDefinition;

/** One skill placed on the tree screen. */
USTRUCT(BlueprintType)
struct FRPGSkillTreeNode
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill Tree")
	TObjectPtr<const URPGSkillDefinition> Skill;

	/** Position on the tree screen, in pixels from the top-left corner. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill Tree")
	FVector2D Position = FVector2D::ZeroVector;
};

/**
 * The skills a character can learn and where they appear on screen.
 * Connections come from each skill's Prerequisites; they are not stored here.
 */
UCLASS(BlueprintType)
class RPGPROGRESSION_API URPGSkillTreeDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill Tree", meta = (TitleProperty = "Skill"))
	TArray<FRPGSkillTreeNode> Nodes;
};