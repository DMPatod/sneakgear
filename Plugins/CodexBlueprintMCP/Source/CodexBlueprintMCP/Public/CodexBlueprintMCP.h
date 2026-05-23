#pragma once

#include "Modules/ModuleManager.h"

class FCodexBlueprintMCPModule final : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
	void StartHttpServer();
	void StopHttpServer();
};
