// Copyright Epic Games, Inc. All Rights Reserved.

#include "RPGCrafting.h"

#define LOCTEXT_NAMESPACE "FRPGCraftingModule"
DEFINE_LOG_CATEGORY(LogRPGCrafting);

void FRPGCraftingModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FRPGCraftingModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FRPGCraftingModule, RPGCrafting)