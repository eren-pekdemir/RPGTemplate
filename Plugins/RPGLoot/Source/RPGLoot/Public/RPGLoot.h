// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"

RPGLOOT_API DECLARE_LOG_CATEGORY_EXTERN(LogRPGLoot, Log, All);

class FRPGLootModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
