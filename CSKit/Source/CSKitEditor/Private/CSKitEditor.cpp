// Copyright Epic Games, Inc. All Rights Reserved.

#include "CSKitEditor.h"

#include "MessageLogModule.h"
#include "PropertyTypeCustomization/CSKitEditor_DataTableRowSelectorCustomization.h"
#include "PropertyTypeCustomization/CSKitEditor_BrainQueryTestSelectorCustomization.h"

#define LOCTEXT_NAMESPACE "FCSKitEditorModule"

void FCSKitEditorModule::StartupModule()
{
	auto& moduleMgr = FModuleManager::Get();
	if (moduleMgr.IsModuleLoaded("PropertyEditor")) {

		auto& PropertyEditorModule = moduleMgr.LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");

		PropertyEditorModule.RegisterCustomPropertyTypeLayout(
			("CSKit_DataTableRowSelector"),
			FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FCSKitEditor_DataTableRowSelectorCustomization::MakeInstance)
			);
		PropertyEditorModule.RegisterCustomPropertyTypeLayout(
			("CSKit_BrainQueryTestSelector"),
			FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FCSKitEditor_BrainQueryTestSelectorCustomization::MakeInstance)
		);

		PropertyEditorModule.NotifyCustomizationModuleChanged();
	}
	
	// MessageLogのカテゴリ登録
	FMessageLogModule& MessageLogModule = FModuleManager::LoadModuleChecked<FMessageLogModule>("MessageLog");
	MessageLogModule.RegisterLogListing(
		"CSKit",
		NSLOCTEXT("CSKit", "CSKit", "CSKit")
	);
}

void FCSKitEditorModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FCSKitEditorModule, CSKitEditor)