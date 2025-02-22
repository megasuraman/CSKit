// Copyright 2020 megasuraman
/**
 * @file CSKitDebug_SaveData.cpp
 * @brief Debug用の簡易セーブ情報(UE5.4からGConfig->Set系で.iniの書き込みがうまく行かなくなったので代わり)
 * @author megasuraman
 * @date 2025/01/27
 */
#include "CSKitDebug_SaveData.h"

#include "Misc/FileHelper.h"

void FCSKitDebug_SaveData::Save()
{
	mValueList.Empty();
	for (const auto& MapElement : mValueMap)
	{
		mValueList.Add(MapElement.Value);
	}

	FString FilePath = FPaths::ConvertRelativePathToFull(FPaths::ProjectSavedDir());
	FilePath += FString(TEXT("CSKitDebug/SaveData.txt"));
	FFileHelper::SaveStringToFile(ToJson(), *FilePath, FFileHelper::EEncodingOptions::ForceUTF8);
}

void FCSKitDebug_SaveData::Load()
{
	if(mbLoaded)
	{
		return;
	}
	FString FilePath = FPaths::ConvertRelativePathToFull(FPaths::ProjectSavedDir());
	FilePath += FString(TEXT("CSKitDebug/SaveData.txt"));
	
	FString JsonString;
	FFileHelper::LoadFileToString(JsonString, *FilePath);
	FromJson(JsonString);
	mbLoaded = true;

	mValueMap.Empty();
	for(const FCSKitDebug_SaveDataValue& SaveDataValue : mValueList)
	{
		mValueMap.Add(SaveDataValue.mTag, SaveDataValue);
	}
}

void FCSKitDebug_SaveData::SetBool(const FString& InTag, bool bInValue)
{
	if (FCSKitDebug_SaveDataValue* SaveDataValue = mValueMap.Find(InTag))
	{
		ensure(SaveDataValue->GetValueType() == ECSKitDebug_SaveDataValueType::Bool);
		if (bInValue)
		{
			SaveDataValue->mValueString = FString(TEXT("True"));
		}
		else
		{
			SaveDataValue->mValueString = FString(TEXT("False"));
		}
		Save();
		return;
	}

	//新規追加
	FCSKitDebug_SaveDataValue Value;
	Value.mTag = InTag;
	if(bInValue)
	{
		Value.mValueString = FString(TEXT("True"));
	}
	else
	{
		Value.mValueString = FString(TEXT("False"));
	}
	Value.mValueType = static_cast<int32>(ECSKitDebug_SaveDataValueType::Bool);
	mValueMap.Add(InTag, Value);
	Save();
}

bool FCSKitDebug_SaveData::GetBool(const FString& InTag) const
{
	if (const FCSKitDebug_SaveDataValue* SaveDataValue = mValueMap.Find(InTag))
	{
		ensure(SaveDataValue->GetValueType() == ECSKitDebug_SaveDataValueType::Bool);
		return SaveDataValue->mValueString.ToBool();
	}
	return false;
}
