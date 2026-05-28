#pragma once

#include "Modules/ModuleManager.h"

class FEasingFunctionModule : public IModuleInterface
{
public:

	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
