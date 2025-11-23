// Copyright 2022 megasuraman
/**
 * @file CSKitEditor_EUW_ShortcutCommand.cpp
 * @brief ShortcutCommand用EUW
 * @author megasuraman
 * @date 2023/03/30
 */
#include "EditorUtilityWidget/CSKitEditor_EUW_ShortcutCommand.h"

#include "CSKitDebug_ShortcutCommand.h"
#include "CSKitDebug_Subsystem.h"
#include "Editor.h"
#include "Misc/FileHelper.h"

/**
 * @brief	
 */
void UCSKitEditor_EUW_ShortcutCommand::PostInitProperties()
{
	Super::PostInitProperties();

	Load();
}

/**
 * @brief	
 */
void UCSKitEditor_EUW_ShortcutCommand::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	Save();
}

/**
 * @brief	
 */
void UCSKitEditor_EUW_ShortcutCommand::OnRunGame(const UWorld& InWorld)
{
	Super::OnRunGame(InWorld);
	
	UCSKitDebug_ShortcutCommand* ShortcutCommand = GetShortcutCommand(InWorld);
	if (ShortcutCommand == nullptr)
	{
		return;
	}
	ShortcutCommand->AddPersonalShortcutCommand(mPersonalShortcutCommand);
}

/**
 * @brief	
 */
UCSKitDebug_ShortcutCommand* UCSKitEditor_EUW_ShortcutCommand::GetShortcutCommand(const UWorld& InWorld)
{
	const UGameInstance* GameInstance = InWorld.GetGameInstance();
	if (GameInstance == nullptr)
	{
		return nullptr;
	}
	const UCSKitDebug_Subsystem* CSKitEditorSubsystem = GameInstance->GetSubsystem<UCSKitDebug_Subsystem>();
	if (CSKitEditorSubsystem == nullptr)
	{
		return nullptr;
	}
	return CSKitEditorSubsystem->GetShortcutCommand();
}
/**
 * @brief	
 */
void UCSKitEditor_EUW_ShortcutCommand::Save()
{
	if (mPersonalShortcutCommand.Num() == 0)
	{
		return;
	}
	FCSKitEditor_PersonalShortcutCommand PersonalShortcutCommandJson;
	for (const auto& MapElement : mPersonalShortcutCommand)
	{
		FCSKitEditor_PersonalShortcutCommandSaveNode SaveNode;
		SaveNode.mConsoleCommand = MapElement.Key;
		SaveNode.mKeyboardString = MapElement.Value.mKeyboard.ToString();
		SaveNode.mPadString = MapElement.Value.mPad.ToString();
		PersonalShortcutCommandJson.mNodeList.Add(SaveNode);
	}
	FString FilePath = FPaths::ConvertRelativePathToFull(FPaths::ProjectSavedDir());
	FilePath += FString(TEXT("CSKit/PersonalShortcutCommand.txt"));
	FFileHelper::SaveStringToFile(PersonalShortcutCommandJson.ToJson(), *FilePath, FFileHelper::EEncodingOptions::ForceUTF8);
}
/**
 * @brief	
 */
void UCSKitEditor_EUW_ShortcutCommand::Load()
{
	FString FilePath = FPaths::ConvertRelativePathToFull(FPaths::ProjectSavedDir());
	FilePath += FString(TEXT("CSKit/PersonalShortcutCommand.txt"));
	FString JsonString;
	FFileHelper::LoadFileToString(JsonString, *FilePath);
	FCSKitEditor_PersonalShortcutCommand PersonalShortcutCommandJson;
	if (!PersonalShortcutCommandJson.FromJson(JsonString))
	{
		return;
	}

	for (const FCSKitEditor_PersonalShortcutCommandSaveNode& SaveNode : PersonalShortcutCommandJson.mNodeList)
	{
		FCSKitDebugKey DebugKey;
		DebugKey.mKeyboard = FKey(FName(*SaveNode.mKeyboardString));
		DebugKey.mPad = FKey(FName(*SaveNode.mPadString));
		mPersonalShortcutCommand.Add(SaveNode.mConsoleCommand, DebugKey);
	}
}