// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"

RPGCRAFTING_API DECLARE_LOG_CATEGORY_EXTERN(LogRPGCrafting, Log, All);

class FRPGCraftingModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
