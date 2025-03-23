// Copyright 2020 megasuraman
/**
 * @file CSKitEditor_EUW_DTUID.cpp
 * @brief DataTable毎にユニークなEnumを生成する
 * @author megasuraman
 * @date 2022/12/27
 */


#include "EditorUtilityWidget/CSKitEditor_EUW_DTUID.h"
#include "Engine/DataTable.h"
#include "Misc/FileHelper.h"


 /**
  * @brief
  */
void	UCSKitEditor_EUW_DTUID::GenerateDataTableUID(const UDataTable* InDataTable)
{
	if (InDataTable == nullptr)
	{
		return;
	}
	const FString DataTableName = InDataTable->GetName();
	const FString TargetName = DataTableName + FString(TEXT("_UID"));
	FString FilePath = FPaths::ConvertRelativePathToFull(FPaths::GameSourceDir());
	FilePath += FString::Printf(TEXT("/%s/DataTableUID/%s.h"), FApp::GetProjectName(), *TargetName);
	FString HeaderFileString;

	TArray<FDTUIDInfo> DTUIDInfoList;
	GetExistDTUID(DTUIDInfoList, FilePath);
	if (DTUIDInfoList.Num() == 0)
	{
		return;
	}
	CheckDataTableRow(DTUIDInfoList, InDataTable);
	TMap<FString, FString> StringReplaceMap;
	CreateStringReplacepMap(StringReplaceMap, DTUIDInfoList, DataTableName);

	FString BaseFilePath = FPaths::ConvertRelativePathToFull(FPaths::ProjectPluginsDir());
	BaseFilePath += FString(TEXT("CSKitEditor/Content/EUW/DTUID_Template.txt"));
	FString NewHeaderFileString;
	GetNewFileString(NewHeaderFileString, BaseFilePath, StringReplaceMap);
	FFileHelper::SaveStringToFile(NewHeaderFileString, *FilePath, FFileHelper::EEncodingOptions::ForceUTF8);
}

/**
 * @brief
 */
bool UCSKitEditor_EUW_DTUID::GetExistDTUID(TArray<FDTUIDInfo>& OutDTUIDList, const FString& InPath) const
{
	FString FileString;
	if (!FFileHelper::LoadFileToString(FileString, *InPath))
	{
		return false;
	}

	TArray<FString> FileLineList;
	FileString.ParseIntoArray(FileLineList, TEXT("\n"));

	int32 InvalidStringIndex = INDEX_NONE;
	int32 NextUID = INDEX_NONE;
	int32 InsertStringListIndex = INDEX_NONE;
	for (int32 i = 0; i < FileLineList.Num(); ++i)
	{
		FString& TargetString = FileLineList[i];
		if (TargetString.Find(FString(TEXT("Invalid"))) != INDEX_NONE)
		{
			InvalidStringIndex = i;
		}
		else if (TargetString.Find(FString(TEXT("NextUID"))) != INDEX_NONE)
		{
			FString NextUIDWithNumberString;
			FString IgnoreString;
			TargetString.Split(TEXT(" "), &NextUIDWithNumberString, &IgnoreString);
			FString NextUIDString;
			NextUIDWithNumberString.Split(TEXT("="), &IgnoreString, &NextUIDString);
			NextUID = FCString::Atoi(*NextUIDString);
			InsertStringListIndex = i;
			break;
		}
		else if (InvalidStringIndex != INDEX_NONE)
		{
			FDTUIDInfo DTUIDInfo;

			FString LeftString;
			FString RightString;
			TargetString.Split(TEXT("="), &LeftString, &RightString);
			
			{//Enum名取得
				FString GarbageString;
				LeftString.Split(TEXT("\t"), &GarbageString, &DTUIDInfo.mName);
			}
			{//Id取得
				if (RightString.Find(TEXT("UMETA(Hidden)")) != INDEX_NONE)
				{
					FString IdString;
					FString GarbageString;
					RightString.Split(TEXT(" "), &IdString, &GarbageString);
					DTUIDInfo.mId = FCString::Atoi(*IdString);
					DTUIDInfo.mbDelete = true;
				}
				else
				{
					FString IdString;
					FString GarbageString;
					RightString.Split(TEXT(","), &IdString, &GarbageString);
					DTUIDInfo.mId = FCString::Atoi(*IdString);
				}
			}
			
			OutDTUIDList.Add(DTUIDInfo);
		}
	}

	return OutDTUIDList.Num()>0;
}

/**
 * @brief
 */
void UCSKitEditor_EUW_DTUID::CheckDataTableRow(TArray<FDTUIDInfo>& OutDTUIDList, const UDataTable* InDataTable) const
{
	const TArray<FName> RowNameList = InDataTable->GetRowNames();
	TArray<FName> AddRowNameList = RowNameList;

	// Delete情報更新
	for (FDTUIDInfo& Info : OutDTUIDList)
	{
		bool bOwnDataTable = false;
		for (const FName& RowName : RowNameList)
		{
			if (Info.mName == RowName.ToString())
			{
				bOwnDataTable = true;
				AddRowNameList.Remove(RowName);
				break;
			}
		}

		Info.mbDelete = !bOwnDataTable;
	}

	uint32 NextIndex = OutDTUIDList.Last().mId + 1;
	for (const FName AddName : AddRowNameList)
	{
		FDTUIDInfo AddDTUID;
		AddDTUID.mName = AddName.ToString();
		AddDTUID.mId = NextIndex;
		OutDTUIDList.Add(AddDTUID);
		++NextIndex;
	}
}

/**
 * @brief
 */
bool UCSKitEditor_EUW_DTUID::CreateStringReplacepMap(TMap<FString, FString>& OutStringReplaceMap, const TArray<FDTUIDInfo>& InDTUIDList, const FString& InDataTableName) const
{
	const FString DTUIDName = InDataTableName + FString(TEXT("_UID"));
	const FString EnumName = FString(TEXT("E")) + DTUIDName;
	{//***DTName***
		OutStringReplaceMap.Add(FString(TEXT("***DTName***")), DTUIDName);
	}
	{//***EnumName***
		OutStringReplaceMap.Add(FString(TEXT("***EnumName***")), EnumName);
	}
	{//***Enum***
		FString ReplaceString;
		for (const FDTUIDInfo& Info : InDTUIDList)
		{
			FString EnumString = FString::Printf(TEXT("\t%s=%d"), *Info.mName, Info.mId);
			if (Info.mbDelete)
			{
				EnumString += FString(TEXT(" UMETA(Hidden)"));
			}
			EnumString += FString(TEXT(",\n"));
			ReplaceString += EnumString;
		}
		OutStringReplaceMap.Add(FString(TEXT("***Enum***")), ReplaceString);
	}
	{//***EnumNextUID***
		FString ReplaceString = FString::Printf(TEXT("%d"), InDTUIDList.Last().mId + 1);
		OutStringReplaceMap.Add(FString(TEXT("***EnumNextUID***")), ReplaceString);
	}
	{//***GetNameMap***
		FString ReplaceString;
		for (const FDTUIDInfo& Info : InDTUIDList)
		{
			FString MapAddString = FString::Printf(TEXT("\t\t\tsmUIDMap.Add(%s::%s, FName(TEXT(\"%s\")));\n")
				, *EnumName
				, *Info.mName
				, *Info.mName);
			ReplaceString += MapAddString;
		}
		OutStringReplaceMap.Add(FString(TEXT("***GetNameMap***")), ReplaceString);
	}
	{//***GetUIDMap***
		FString ReplaceString;
		for (const FDTUIDInfo& Info : InDTUIDList)
		{
			FString MapAddString = FString::Printf(TEXT("\t\t\tsmUIDMap.Add(FName(TEXT(\"%s\")), %s::%s);\n")
				, *Info.mName
				, * EnumName
				, *Info.mName);
			ReplaceString += MapAddString;
		}
		OutStringReplaceMap.Add(FString(TEXT("***GetUIDMap***")), ReplaceString);
	}
	return true;
}

/**
 * @brief
 */
bool UCSKitEditor_EUW_DTUID::GetNewFileString(FString& OutFileString, const FString& InBaseFilePath, const TMap<FString, FString>& InStringReplaceMap)
{
	if (!FFileHelper::LoadFileToString(OutFileString, *InBaseFilePath))
	{
		return false;
	}

#if 0//なんかうまくReplaceできない
	for (const auto& Element : InStringReplaceMap)
	{
		OutFileString.Replace(*Element.Key, *Element.Value);
	}
#else
	TArray<FString> FileLineList;
	OutFileString.ParseIntoArray(FileLineList, TEXT("\n"));
	OutFileString.Empty();
	for (FString& LineString : FileLineList)
	{
		for (const auto& Element : InStringReplaceMap)
		{
			if (LineString.Find(*Element.Key) != INDEX_NONE)
			{
				FString LeftString;
				FString RightString;
				LineString.Split(*Element.Key, &LeftString, &RightString);
				LineString = LeftString + Element.Value + RightString;
			}
		}
		OutFileString += LineString + FString("\n");
	}
#endif
	return true;
}