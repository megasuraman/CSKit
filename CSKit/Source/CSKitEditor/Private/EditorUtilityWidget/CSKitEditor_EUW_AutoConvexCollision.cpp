// Copyright 2020 megasuraman
/**
 * @file CSKitEditor_EUW_AutoConvexCollision.cpp
 * @brief AutoConvexCollision操作用EUW
 * @author megasuraman
 * @date 2025/02/22
 */

#include "EditorUtilityWidget/CSKitEditor_EUW_AutoConvexCollision.h"

#include "Misc/FileHelper.h"


void UCSKitEditor_EUW_AutoConvexCollision::LoadStaticMeshList()
{
	mStaticMeshList.Empty();
	
	FString FilePath = FPaths::ConvertRelativePathToFull(FPaths::ProjectSavedDir());
	FilePath += mSavedFilePath;
	
	FString FileString;
	FFileHelper::LoadFileToString(FileString, *FilePath);
	TArray<FString> AssetPathStringList;
	FileString.ParseIntoArrayLines(AssetPathStringList);
	for (const FString& AssetPath : AssetPathStringList)
	{
		if (UStaticMesh* StaticMesh = LoadObject<UStaticMesh>(nullptr, *AssetPath))
		{
			mStaticMeshList.Add(StaticMesh);
		}
	}
}
