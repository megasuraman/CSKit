// Copyright 2020 megasuraman
/**
 * @file CSKitEditor_EUW_AutoConvexCollision.cpp
 * @brief AutoConvexCollision操作用EUW
 * @author megasuraman
 * @date 2025/02/22
 */

#include "EditorUtilityWidget/CSKitEditor_EUW_AutoConvexCollision.h"

#include "Misc/FileHelper.h"


FString UCSKitEditor_EUW_AutoConvexCollision::FStaticMeshInfo::GetOutputString() const
{
	FString OutputString;
	const UStaticMesh* StaticMesh = mStaticMesh.Get();
	if(StaticMesh == nullptr)
	{
		return OutputString;
	}
	OutputString += StaticMesh->GetPathName();
	OutputString += FString(TEXT(","));
	OutputString += FString::Printf(TEXT("%lld"), mTimeStamp);
	return OutputString;
}

bool UCSKitEditor_EUW_AutoConvexCollision::Initialize()
{
	LoadIgnoreFile();
	return Super::Initialize();
}

void UCSKitEditor_EUW_AutoConvexCollision::LoadTargetFile()
{
	mTargetMap.Empty();

	FString FilePath = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir());
	FilePath += mTargetFilePath;
	
	FString FileString;
	FFileHelper::LoadFileToString(FileString, *FilePath);
	TArray<FString> FileLineStringList;
	FileString.ParseIntoArrayLines(FileLineStringList);
	for (const FString& FileLine : FileLineStringList)
	{
		TArray<FString> StringList;
		FileLine.ParseIntoArray(StringList, TEXT(","));
		if(StringList.Num() < 2)
		{
			continue;
		}
		const FString& AssetPath = StringList[0];
		UStaticMesh* StaticMesh = LoadObject<UStaticMesh>(nullptr, *AssetPath);
		if (StaticMesh == nullptr)
		{
			continue;
		}
		const FString& TimeStampString = StringList[1];
		const int64 TimeStamp =  FCString::Atoi64(*TimeStampString);
		
		FStaticMeshInfo StaticMeshInfo;
		StaticMeshInfo.mStaticMesh = StaticMesh;
		StaticMeshInfo.mTimeStamp = TimeStamp;
		mTargetMap.Add(AssetPath, StaticMeshInfo);
	}
}

void UCSKitEditor_EUW_AutoConvexCollision::AssignTargetListFromTargetFile(bool bInOnlyNewTimeStamp)
{
	LoadTargetFile();
	
	mTargetList.Empty();
	
	for (const auto& MapElement : mTargetMap)
	{
		const FString& AssetPath = MapElement.Key;
		const FStaticMeshInfo& StaticMeshInfo = MapElement.Value;
		
		UStaticMesh* StaticMesh = StaticMeshInfo.mStaticMesh.Get();
		if (StaticMesh == nullptr)
		{
			continue;
		}
		if(bInOnlyNewTimeStamp)
		{
			const int64 TimeStampSave =  StaticMeshInfo.mTimeStamp;
			const int64 TimeStampNow = GetFileTimeStampSecond(GetAssetFullPath(AssetPath));
			if(TimeStampSave == TimeStampNow)
			{
				continue;
			}
		}
		
		mTargetList.Add(StaticMesh);
	}
}

void UCSKitEditor_EUW_AutoConvexCollision::AddStaticMeshToTargetFile(UStaticMesh* InStaticMesh)
{
	if(InStaticMesh == nullptr)
	{
		return;
	}
	const FString StaticMeshPath = InStaticMesh->GetPathName();
	if(mTargetMap.Contains(StaticMeshPath))
	{
		return;
	}

	FStaticMeshInfo StaticMeshInfo;
	StaticMeshInfo.mStaticMesh = InStaticMesh;
	StaticMeshInfo.mTimeStamp = 0;
	mTargetMap.Add(StaticMeshPath, StaticMeshInfo);

	FString FilePath = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir());
	FilePath += mTargetFilePath;
	
	FString FileString;
	FFileHelper::LoadFileToString(FileString, *FilePath);
	TArray<FString> FileLineStringList;
	FileString.ParseIntoArrayLines(FileLineStringList);
	for (const FString& FileLine : FileLineStringList)
	{
		TArray<FString> StringList;
		FileLine.ParseIntoArray(StringList, TEXT(","));
		if(StringList.Num() < 2)
		{
			continue;
		}
		const FString& AssetPath = StringList[0];
		if (AssetPath == StaticMeshPath)
		{
			return;
		}
	}

	FileString += FString::Printf(TEXT("%s,0\n"), *StaticMeshPath);
	FFileHelper::SaveStringToFile(FileString, *FilePath, FFileHelper::EEncodingOptions::ForceUTF8);
}

void UCSKitEditor_EUW_AutoConvexCollision::AddIgnoreStaticMesh(TArray<UStaticMesh*> InList)
{
	for (UStaticMesh* StaticMesh : InList)
	{
		if (StaticMesh == nullptr)
		{
			continue;
		}
		mIgnoreList.AddUnique(StaticMesh);
	}
	SaveIgnoreFile();
}

bool UCSKitEditor_EUW_AutoConvexCollision::IsIgnoreStaticMesh(UStaticMesh* InStaticMesh) const
{
	return mIgnoreList.Contains(InStaticMesh);
}

void UCSKitEditor_EUW_AutoConvexCollision::SaveTargetFile()
{
	FString FilePath = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir());
	FilePath += mTargetFilePath;
	
	FString FileString;
	for (const auto& MapElement : mTargetMap)
	{
		const FStaticMeshInfo& StaticMeshInfo = MapElement.Value;
		FileString += FString::Printf(TEXT("%s\n"), *StaticMeshInfo.GetOutputString());
	}
	
	FFileHelper::SaveStringToFile(FileString, *FilePath, FFileHelper::EEncodingOptions::ForceUTF8);
}

void UCSKitEditor_EUW_AutoConvexCollision::UpdateTimeStamp(UStaticMesh* InStaticMesh)
{
	if(InStaticMesh == nullptr)
	{
		return;
	}
	const FString StaticMeshPath = InStaticMesh->GetPathName();
	if(FStaticMeshInfo* StaticMeshInfo = mTargetMap.Find(StaticMeshPath))
	{
		const int64 TimeStampNow = GetFileTimeStampSecond(GetAssetFullPath(StaticMeshPath));
		StaticMeshInfo->mTimeStamp = TimeStampNow;
	}
}

void UCSKitEditor_EUW_AutoConvexCollision::SaveIgnoreFile()
{
	FString FilePath = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir());
	FilePath += mIgnoreFilePath;
	
	FString FileString;
	for (const UStaticMesh* StaticMesh : mIgnoreList)
	{
		if (StaticMesh == nullptr)
		{
			continue;
		}
		FileString += FString::Printf(TEXT("%s\n"), *StaticMesh->GetPathName());
	}
	
	FFileHelper::SaveStringToFile(FileString, *FilePath, FFileHelper::EEncodingOptions::ForceUTF8);
}

void UCSKitEditor_EUW_AutoConvexCollision::LoadIgnoreFile()
{
	mIgnoreList.Empty();

	const FString SaveDirPath = FPaths::ProjectDir();
	FString FilePath = FPaths::ConvertRelativePathToFull(SaveDirPath);
	FilePath += mIgnoreFilePath;
	
	FString FileString;
	FFileHelper::LoadFileToString(FileString, *FilePath);
	TArray<FString> AssetPathStringList;
	FileString.ParseIntoArrayLines(AssetPathStringList);
	for (const FString& AssetPath : AssetPathStringList)
	{
		if (UStaticMesh* StaticMesh = LoadObject<UStaticMesh>(nullptr, *AssetPath))
		{
			mIgnoreList.Add(StaticMesh);
		}
	}
}
