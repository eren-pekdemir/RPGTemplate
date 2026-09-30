// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/RPGInventoryFragment.h"

#include "Items/RPGItemDefinition.h"

float URPGInventoryFragment::GetUnitWeight(const URPGItemDefinition* Item)
{
	if (!Item) return 0.f;
	
	const URPGInventoryFragment* ItemFragment = Item->FindFragmentByClass<URPGInventoryFragment>();
	
	if (ItemFragment)
	{
		return ItemFragment->Weight;
	}
	else
	{
		return 0.f;
	}
}
