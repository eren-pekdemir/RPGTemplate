#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AttributeSet.h"
#include "RPGAttributeBarWidget.generated.h"

class APawn;
class UAbilitySystemComponent;
class UProgressBar;
class UTextBlock;
struct FOnAttributeChangeData;

/**
 * Generic bar for any "Value / Max" attribute pair (Health / MaxHealth, Stamina / MaxStamina, ...).
 * Choose the two attributes per instance in the designer. By default it follows the owning
 * player's pawn, including respawns and possession changes.
 */
UCLASS(Abstract)
class RPGABILITIESUI_API URPGAttributeBarWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    /** Bind to a specific ability system (e.g. an enemy's). nullptr clears the bar. */
    UFUNCTION(BlueprintCallable, Category = "RPG|Attribute Bar")
    void SetAbilitySystem(UAbilitySystemComponent* NewAbilitySystem);

    UFUNCTION(BlueprintPure, Category = "RPG|Attribute Bar")
    float GetValue() const;

    UFUNCTION(BlueprintPure, Category = "RPG|Attribute Bar")
    float GetMaxValue() const;

protected:
    UPROPERTY(EditAnywhere, Category = "RPG|Attribute Bar")
    FGameplayAttribute ValueAttribute;

    UPROPERTY(EditAnywhere, Category = "RPG|Attribute Bar")
    FGameplayAttribute MaxAttribute;

    /** Follow the owning player's pawn automatically. Turn off to call SetAbilitySystem manually. */
    UPROPERTY(EditAnywhere, Category = "RPG|Attribute Bar")
    bool bAutoBindToOwningPawn = true;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UProgressBar> Bar;

    UPROPERTY(meta = (BindWidgetOptional))
    TObjectPtr<UTextBlock> ValueText;

    /** For designer effects (flash on damage, colour when low). */
    UFUNCTION(BlueprintImplementableEvent, Category = "RPG|Attribute Bar")
    void BP_OnValueChanged(float NewValue, float MaxValue);

    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;

private:
    UFUNCTION()
    void HandlePossessedPawnChanged(APawn* OldPawn, APawn* NewPawn);

    void HandleAttributeChanged(const FOnAttributeChangeData& Data);

    void BindToPawn(const APawn* Pawn);
    void Refresh();

    TWeakObjectPtr<UAbilitySystemComponent> AbilitySystem;

    FDelegateHandle ValueChangedHandle;
    FDelegateHandle MaxChangedHandle;
};