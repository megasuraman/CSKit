// Copyright Epic Games, Inc. All Rights Reserved.

#include "CSKitDebug.h"
#include "CSKitDebug_Config.h"

#if WITH_EDITOR
#include "ISettingsModule.h"
#endif

#define LOCTEXT_NAMESPACE "FCSKitDebugModule"

void FCSKitDebugModule::StartupModule()
{
#if WITH_EDITOR
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings");
	if (SettingsModule != nullptr)
	{
		SettingsModule->RegisterSettings(
			"Project", //ProjectSettingに表示するため
			"Plugins",
			"CSKitDebug",
			LOCTEXT("CSKitDebugName", "CSKitDebug"),
			LOCTEXT("CSKitDebugDescription", "CSKitDebug Config"),
			GetMutableDefault<UCSKitDebug_Config>()
		);
	}
#endif
}

void FCSKitDebugModule::ShutdownModule()
{
#if WITH_EDITOR
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings");
	if (SettingsModule != nullptr)
	{
		SettingsModule->UnregisterSettings(
			"Project", //�v���W�F�N�g�ݒ�ɏo�����߂�
			"Plugins",
			"CSKitDebug"
		);
	}
#endif
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FCSKitDebugModule, CSKitDebug)