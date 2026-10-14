// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"

RPGPROGRESSION_API DECLARE_LOG_CATEGORY_EXTERN(LogRPGProgression, Log, All);

class FRPGProgressionModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
