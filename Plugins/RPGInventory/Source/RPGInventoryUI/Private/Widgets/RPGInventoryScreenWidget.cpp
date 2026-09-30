// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/RPGInventoryScreenWidget.h"
#include "Widgets/RPGInventoryGridWidget.h"

void URPGInventoryScreenWidget::SetInventory(URPGInventoryComponent* NewInventory)
{
	InventoryGrid->SetInventory(NewInventory);
}
