// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"

RPGINVENTORY_API DECLARE_LOG_CATEGORY_EXTERN(LogRPGInventory, Log, All);

class FRPGInventoryModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
