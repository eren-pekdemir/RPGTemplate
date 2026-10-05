// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"

RPGABILITIES_API DECLARE_LOG_CATEGORY_EXTERN(LogRPGAbilities, Log, All);

class FRPGAbilitiesModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
