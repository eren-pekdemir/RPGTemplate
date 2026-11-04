// RPGCore.h

#pragma once

#include "Modules/ModuleManager.h"

class FRPGCoreModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
