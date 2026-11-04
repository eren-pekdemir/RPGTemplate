// RPGInteraction.h

#pragma once

#include "Modules/ModuleManager.h"

class FRPGInteractionModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
