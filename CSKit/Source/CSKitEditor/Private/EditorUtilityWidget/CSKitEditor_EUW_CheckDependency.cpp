// Copyright 2020 megasuraman
/**
 * @file CSKitEditor_EUW_CheckDependency.cpp
 * @brief 依存アセットのリストアップ機能
 * @author megasuraman
 * @date 2025/06/29
 */
#include "EditorUtilityWidget/CSKitEditor_EUW_CheckDependency.h"

#include "CSKitDebug_Utility.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Misc/FileHelper.h"

UCSKitEditor_EUW_CheckDependency::UCSKitEditor_EUW_CheckDependency(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

/* ------------------------------------------------------------
   !
------------------------------------------------------------ */
void UCSKitEditor_EUW_CheckDependency::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	UpdateDependencyResultList();
	OutputResultFile();
	
	OnPostEditChangePropertyBP();
}

/* ------------------------------------------------------------
   !
------------------------------------------------------------ */
FString UCSKitEditor_EUW_CheckDependency::GetDependencyInfo() const
{
	FString OutString;
	for(const auto& MapElement : mDependencyResultMap)
	{
		const UObject* TargetObject = MapElement.Key.Get();
		if(TargetObject == nullptr)
		{
			continue;
		}
		OutString += FString::Printf(TEXT("[%s]\n"), *TargetObject->GetPathName());

		for(const FCSKitEditor_DependencyResult& Result : MapElement.Value.mList)
		{
			if(const UObject* Object = Result.mObject.Get())
			{
				OutString += FString::Printf(TEXT("   %s\n"), *Object->GetPathName());
				FString IndentString(TEXT("     "));
				for(const FString& Path : Result.mRootPath)
				{
					OutString += FString::Printf(TEXT("%s|-%s\n"),*IndentString, *Path);
					IndentString += FString(TEXT("  "));
				}
			}
		}
	}

	return OutString;
}

/* ------------------------------------------------------------
   !
------------------------------------------------------------ */
bool UCSKitEditor_EUW_CheckDependency::IsHitFilter(const FString& InString) const
{
	if(mFilterList.Num() == 0)
	{
		return true;
	}
	for(const FString& FilterString : mFilterList)
	{
		if(InString.Contains(FilterString))
		{
			return true;
		}
	}
	return false;
}

/* ------------------------------------------------------------
   !
------------------------------------------------------------ */
void UCSKitEditor_EUW_CheckDependency::UpdateDependencyResultList()
{
	mDependencyResultMap.Empty();

	if(mTarget != nullptr)
	{
		UpdateDependencyResultListByAsset(mTarget);
	}
	else if(!mTargetFolderPath.IsNone())
	{
		UpdateDependencyResultListByFolder(mTargetFolderPath);
	}
}

/* ------------------------------------------------------------
   !
------------------------------------------------------------ */
void UCSKitEditor_EUW_CheckDependency::UpdateDependencyResultListByFolder(const FName& InPath)
{
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	TArray<FAssetData> AssetDataList;
	AssetRegistryModule.Get().GetAssetsByPath(InPath, AssetDataList, true);
	for (const FAssetData& AssetData : AssetDataList)
	{
		if(UObject* TargetAsset = AssetData.GetAsset())
		{
			UpdateDependencyResultListByAsset(TargetAsset);
		}
	}
}

/* ------------------------------------------------------------
   !
------------------------------------------------------------ */
void UCSKitEditor_EUW_CheckDependency::UpdateDependencyResultListByAsset(const UObject* InTarget)
{
	if(InTarget == nullptr)
	{
		return;
	}

	FCSKitEditor_DependencyResultList ResultList;
	TArray<UCSKitDebug_Utility::FAssetDependency> DependencyDataList;
	UCSKitDebug_Utility::CollectAssetDependency(DependencyDataList, InTarget);
	for(const auto& DependencyData : DependencyDataList)
	{
		if(!IsHitFilter(DependencyData.mAssetPathName.ToString()))
		{
			continue;
		}
		
		FCSKitEditor_DependencyResult CSKitEditor_DependencyResult;
		CSKitEditor_DependencyResult.mObject = TSoftObjectPtr<UObject>( FSoftObjectPath(DependencyData.mAssetPathName.ToString())).LoadSynchronous();
		const int32 DependencyRootNum = DependencyData.mRoot.Num();
		for(int32 i=0; i<DependencyRootNum; ++i)
		{
			const FString& Path = DependencyData.mRoot[DependencyRootNum -1 -i].ToString();
			CSKitEditor_DependencyResult.mRootObject.Add(TSoftObjectPtr<UObject>( FSoftObjectPath(Path)).LoadSynchronous());
			CSKitEditor_DependencyResult.mRootPath.Add(Path);
		}
		ResultList.mList.Add(CSKitEditor_DependencyResult);
	}

	if(ResultList.mList.Num() > 0)
	{
		mDependencyResultMap.Add(InTarget, ResultList);
	}
}

/* ------------------------------------------------------------
   !mDependencyDataMapの結果をファイルに出力
------------------------------------------------------------ */
void UCSKitEditor_EUW_CheckDependency::OutputResultFile() const
{
	const FString ResultString = GetDependencyInfo();

	FString FilePath = FPaths::ConvertRelativePathToFull(FPaths::ProjectSavedDir());
	FilePath += FString::Printf(TEXT("/CSKit/DependencyResult.txt"));
	FFileHelper::SaveStringToFile(ResultString, *FilePath, FFileHelper::EEncodingOptions::ForceUTF8);
}
