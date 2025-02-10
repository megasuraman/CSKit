// Copyright 2020 megasuraman
/**
 * @file CSKitEditor_EUW_DebugMenu.cpp
 * @brief DebugMenuŠÖ˜AEUW
 * @author megasuraman
 * @date 2025/01/27
 */

#include "EditorUtilityWidget/CSKitEditor_EUW_DebugMenu.h"

#include "CSKitDebug_Subsystem.h"

bool UCSKitEditor_EUW_DebugMenu::Initialize()
{
	const bool bResult = Super::Initialize();

	mbAutoLoad = UCSKitDebug_Subsystem::sGetSaveData().GetBool(FString(TEXT("DebugMenu_AutoLoad")));

	return bResult;
}

void UCSKitEditor_EUW_DebugMenu::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	UCSKitDebug_Subsystem::sGetSaveData().SetBool(FString(TEXT("DebugMenu_AutoLoad")), mbAutoLoad);
}

void UCSKitEditor_EUW_DebugMenu::OnRunGame(const UWorld& InWorld)
{

}
