// Copyright 2020 megasuraman
/**
 * @file CSKitEditor_EUW_CheckLevelActors.cpp
 * @brief 配置Actorに対するチェック
 * @author megasuraman
 * @date 2024/11/30
 */


#include "EditorUtilityWidget/CSKitEditor_EUW_CheckLevelActors.h"
#include "EditorUtilityWidget/CheckLevelActors/CSKitEditor_CheckLevelActors_ErrorData.h"
#include "EditorUtilityWidget/CheckLevelActors/CSKitEditor_CheckLevelActors_InvalidClass.h"
#include "CSKitDebug_Utility.h"
#include "Misc/FileHelper.h"
#include "Misc/UObjectToken.h"

/**
 * @brief	配置Actorのクラス情報をレベル別に格納
 */
void UCSKitEditor_EUW_CheckLevelActors::CollectLevelActorsClass()
{
	mLevelActorsClassList.Empty();

	const TArray<ULevel*>& Levels = GetWorld()->GetLevels();
	for (ULevel* Level : Levels)
	{
		const FString LevelName = UCSKitDebug_Utility::GetLevelName(Level);
		FCSKitEditor_CheckLevelActors_ClassList& PlacedActorClassList = mLevelActorsClassList.FindOrAdd(LevelName);
		for (const AActor* Actor : Level->Actors)
		{
			if (Actor == nullptr)
			{
				continue;
			}
			if (PlacedActorClassList.mClassList.Find(Actor->GetClass()) == INDEX_NONE)
			{
				PlacedActorClassList.mClassList.Add(Actor->GetClass());
			}
		}
	}
}

/**
 * @brief	エラーチェック
 */
bool UCSKitEditor_EUW_CheckLevelActors::CheckError()
{
	FCSKitEditor_CheckLevelActors_ErrorData ErrorData;
	mCheckInvalidClass.CheckError(ErrorData, GetWorld());

	PostCheckError(ErrorData);
	
	return ErrorData.mList.Num() > 0;
}

/**
 * @brief	エラーチェック後処理
 */
void UCSKitEditor_EUW_CheckLevelActors::PostCheckError(const FCSKitEditor_CheckLevelActors_ErrorData& InErrorData)
{
	OutputFileResultError(InErrorData);
	RequestMessageLog(InErrorData);
}

/**
 * @brief	エラーチェック結果をFileに出力
 */
void UCSKitEditor_EUW_CheckLevelActors::OutputFileResultError(
	const FCSKitEditor_CheckLevelActors_ErrorData& InErrorData)
{
	TMap<FString, TArray<FString>> ErrorLevelMap;
	for (const FCSKitEditor_CheckLevelActors_ErrorDataNode& Node : InErrorData.mList)
	{
		TArray<FString>& ErrorStringList = ErrorLevelMap.FindOrAdd(Node.mLevelName);
		const FString OutputErrorString = FString::Printf(TEXT("%s [Label:%s] [Class:%s]"),
			*Node.mErrorString,
			*Node.mLabelName,
			*Node.mClassName);
		ErrorStringList.Add(OutputErrorString);
	}
	
	FString OutputString;
	for (const auto& MapElement : ErrorLevelMap)
	{
		OutputString += FString::Printf(TEXT("%s\n"), *MapElement.Key);
		for (const FString& ErrorString : MapElement.Value)
		{
			OutputString += FString::Printf(TEXT("   %s\n"), *ErrorString);
		}
	}
	
	FString FilePath = FPaths::ConvertRelativePathToFull(FPaths::ProjectSavedDir());
	FilePath += FString::Printf(TEXT("/CSKit/CheckLevelActorsResult.txt"));
	FFileHelper::SaveStringToFile(OutputString, *FilePath, FFileHelper::EEncodingOptions::ForceUTF8);
}

/**
 * @brief	エラーをMessageLogで出力
 */
void UCSKitEditor_EUW_CheckLevelActors::RequestMessageLog(
	const FCSKitEditor_CheckLevelActors_ErrorData& InErrorData) const
{
	bool bWishOpenMessageLog = false;
	for (const FCSKitEditor_CheckLevelActors_ErrorDataNode& Node : InErrorData.mList)
	{
		const AActor* TargetActor = Node.mActor.Get();
		if (TargetActor == nullptr)
		{
			continue;
		}

		const FString ErrorString = FString::Printf(TEXT("%s [Class:%s]"), *Node.mErrorString, *Node.mClassName);
		FMessageLog ErrorMessage("CSKit");
		ErrorMessage.Error()
			->AddToken(FUObjectToken::Create(TargetActor))
			->AddToken(FTextToken::Create(FText::FromString(ErrorString)));
		bWishOpenMessageLog = true;
	}

	if (bWishOpenMessageLog)
	{
		FMessageLog("CSKit").Open();
	}
}
