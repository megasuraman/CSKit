// Copyright 2020 megasuraman
/**
 * @file CSKitEditor_EUW_AutoConvexCollision.cpp
 * @brief AutoConvexCollision操作用EUW
 * @author megasuraman
 * @date 2025/02/22
 */

#include "EditorUtilityWidget/CSKitEditor_EUW_AutoConvexCollision.h"

#include "Misc/FileHelper.h"


bool UCSKitEditor_EUW_AutoConvexCollision::Initialize()
{
	LoadStaticMeshIgnoreList();
	return Super::Initialize();
}

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

void UCSKitEditor_EUW_AutoConvexCollision::AddIgnoreStaticMesh(TArray<UStaticMesh*> InList)
{
	for (UStaticMesh* StaticMesh : InList)
	{
		if (StaticMesh == nullptr)
		{
			continue;
		}
		mStaticMeshIgnoreList.AddUnique(StaticMesh);
	}
	SaveStaticMeshIgnoreList();
}

bool UCSKitEditor_EUW_AutoConvexCollision::IsIgnoreStaticMesh(UStaticMesh* InStaticMesh) const
{
	return mStaticMeshIgnoreList.Contains(InStaticMesh);
}

void UCSKitEditor_EUW_AutoConvexCollision::SaveStaticMeshIgnoreList()
{
	FString FilePath = FPaths::ConvertRelativePathToFull(FPaths::ProjectSavedDir());
	FilePath += mSavedIgnoreFilePath;
	
	FString FileString;
	for (const UStaticMesh* StaticMesh : mStaticMeshIgnoreList)
	{
		if (StaticMesh == nullptr)
		{
			continue;
		}
		FileString += FString::Printf(TEXT("%s\n"), *StaticMesh->GetPathName());
	}
	
	FFileHelper::SaveStringToFile(FileString, *FilePath, FFileHelper::EEncodingOptions::ForceUTF8);
}

void UCSKitEditor_EUW_AutoConvexCollision::LoadStaticMeshIgnoreList()
{
	mStaticMeshIgnoreList.Empty();
	
	FString FilePath = FPaths::ConvertRelativePathToFull(FPaths::ProjectSavedDir());
	FilePath += mSavedIgnoreFilePath;
	
	FString FileString;
	FFileHelper::LoadFileToString(FileString, *FilePath);
	TArray<FString> AssetPathStringList;
	FileString.ParseIntoArrayLines(AssetPathStringList);
	for (const FString& AssetPath : AssetPathStringList)
	{
		if (UStaticMesh* StaticMesh = LoadObject<UStaticMesh>(nullptr, *AssetPath))
		{
			mStaticMeshIgnoreList.Add(StaticMesh);
		}
	}
}
