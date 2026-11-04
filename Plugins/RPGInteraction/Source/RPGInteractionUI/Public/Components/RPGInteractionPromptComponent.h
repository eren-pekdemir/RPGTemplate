// RPGInteractionPromptComponent.h

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RPGInteractionPromptComponent.generated.h"

class URPGInteractionPromptWidget;

UCLASS(ClassGroup=(RPG), meta=(BlueprintSpawnableComponent))
class RPGINTERACTIONUI_API URPGInteractionPromptComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	URPGInteractionPromptComponent();
	
	UPROPERTY(EditAnywhere, Category = "Prompt")
	TSubclassOf<URPGInteractionPromptWidget> PromptWidgetClass;
	
	UPROPERTY(EditAnywhere, Category = "Prompt")
	int32 ZOrder = 0;

	UFUNCTION(BlueprintCallable, Category = "Interaction|UI")
	void SetPromptSuppressed(bool bSuppressed);
	
protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	UFUNCTION()
	void HandlePossessedPawnChanged(APawn* OldPawn, APawn* NewPawn);
	
	UPROPERTY()
	TObjectPtr<URPGInteractionPromptWidget> PromptWidget;
};
