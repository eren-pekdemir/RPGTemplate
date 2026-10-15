#include "Widgets/RPGItemNotificationWidget.h"

#include "Components/RPGInventoryComponent.h"   // senin yoluna göre
#include "Components/VerticalBox.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "Widgets/RPGItemNotificationEntryWidget.h"

void URPGItemNotificationWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (APlayerController* PlayerController = GetOwningPlayer())
    {
        PlayerController->OnPossessedPawnChanged.AddUniqueDynamic(this, &ThisClass::HandlePossessedPawnChanged);
        HandlePossessedPawnChanged(nullptr, PlayerController->GetPawn());   // sync on subscribe
    }
}

void URPGItemNotificationWidget::NativeDestruct()
{
    if (APlayerController* PlayerController = GetOwningPlayer())
    {
        PlayerController->OnPossessedPawnChanged.RemoveDynamic(this, &ThisClass::HandlePossessedPawnChanged);
    }

    SetInventory(nullptr);
    Super::NativeDestruct();
}

void URPGItemNotificationWidget::HandlePossessedPawnChanged(APawn* OldPawn, APawn* NewPawn)
{
    SetInventory(NewPawn ? NewPawn->FindComponentByClass<URPGInventoryComponent>() : nullptr);
}

void URPGItemNotificationWidget::SetInventory(URPGInventoryComponent* NewInventory)
{
    // Unbind -> assign -> bind
    if (Inventory)
    {
        Inventory->OnItemsReceived.RemoveDynamic(this, &ThisClass::HandleItemsReceived);
    }

    Inventory = NewInventory;

    if (Inventory)
    {
        Inventory->OnItemsReceived.AddDynamic(this, &ThisClass::HandleItemsReceived);
    }
}

void URPGItemNotificationWidget::HandleItemsReceived(const URPGItemDefinition* Item, int32 Quantity)
{
    if (!Item || Quantity <= 0 || !EntryBox || !EntryClass)
    {
        return;
    }

    // Same item still on screen: merge instead of adding a line.
    for (UWidget* Child : EntryBox->GetAllChildren())
    {
        URPGItemNotificationEntryWidget* Entry = Cast<URPGItemNotificationEntryWidget>(Child);
        if (Entry && Entry->GetItem() == Item)
        {
            Entry->AddQuantity(Quantity);
            return;
        }
    }

    URPGItemNotificationEntryWidget* Entry = CreateWidget<URPGItemNotificationEntryWidget>(this, EntryClass);
    Entry->Setup(Item, Quantity);
    EntryBox->AddChildToVerticalBox(Entry);

    // Oldest lines go first.
    while (EntryBox->GetChildrenCount() > MaxEntries)
    {
        EntryBox->RemoveChildAt(0);
    }
}