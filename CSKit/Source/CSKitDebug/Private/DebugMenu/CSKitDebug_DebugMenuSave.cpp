// Copyright 2022 megasuraman
#include "DebugMenu/CSKitDebug_DebugMenuSave.h"

#include "Misc/FileHelper.h"

void FCSKitDebug_DebugMenuSaveData::Clear()
{
	mSaveNodeList.Empty();
	mValueMap.Empty();
}

void FCSKitDebug_DebugMenuSaveData::Save()
{
	mSaveNodeList.Empty();
	for (const auto& MapElement : mValueMap)
	{
		FCSKitDebug_DebugMenuSaveDataNode Node;
		Node.mPath = MapElement.Key;
		Node.mValueString = MapElement.Value;
		mSaveNodeList.Add(Node);
	}

	const FString FilePath = GetSaveFilePath();
	FFileHelper::SaveStringToFile(ToJson(), *FilePath, FFileHelper::EEncodingOptions::ForceUTF8);
}

void FCSKitDebug_DebugMenuSaveData::Load()
{
	if(mbLoaded)
	{
		return;
	}

	Clear();

	const FString FilePath = GetSaveFilePath();	
	FString JsonString;
	FFileHelper::LoadFileToString(JsonString, *FilePath);
	FromJson(JsonString);

	for (const FCSKitDebug_DebugMenuSaveDataNode& Node : mSaveNodeList)
	{
		WriteValue(Node.mPath, Node.mValueString);
	}
	mbLoaded = true;
}

void FCSKitDebug_DebugMenuSaveData::WriteValue(const FString& InPath, const FString& InValue)
{
	FString& ValueString = mValueMap.FindOrAdd(InPath);
	ValueString = InValue;
}

FString FCSKitDebug_DebugMenuSaveData::GetValueString(const FString& InPath) const
{
	if (const FString* ValueString = mValueMap.Find(InPath))
	{
		return *ValueString;
	}
	return FString();
}

FString FCSKitDebug_DebugMenuSaveData::GetSaveFilePath()
{
	FString FilePath = FPaths::ConvertRelativePathToFull(FPaths::ProjectSavedDir());
	FilePath += FString(TEXT("CSKit/DebugMenu/SaveData.json"));
	return FilePath;
}
