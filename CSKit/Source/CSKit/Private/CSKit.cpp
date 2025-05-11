// Copyright Epic Games, Inc. All Rights Reserved.

#include "CSKit.h"
#include "CSKit_Config.h"

#if WITH_EDITOR
#include "ISettingsModule.h"
#endif
#define LOCTEXT_NAMESPACE "FCSKitModule"

void FCSKitModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
#if WITH_EDITOR
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings");
	if (SettingsModule != nullptr)
	{
		SettingsModule->RegisterSettings(
			"Project", //ProjectSettingに表示するため
			"Plugins",
			"CSKit",
			LOCTEXT("CSKitName", "CSKit"),
			LOCTEXT("CSKitDescription", "CSKit Config"),
			GetMutableDefault<UCSKit_Config>()
		);
	}
#endif
}

void FCSKitModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FCSKitModule, CSKit)