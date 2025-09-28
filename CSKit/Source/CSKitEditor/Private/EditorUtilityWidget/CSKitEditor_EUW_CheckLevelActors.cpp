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
#include "Framework/Notifications/NotificationManager.h"
#include "Misc/FileHelper.h"
#include "Misc/UObjectToken.h"
#include "Widgets/Notifications/SNotificationList.h"

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
 * @brief	開いてるレベルに対してエラーをチェック
 */
bool UCSKitEditor_EUW_CheckLevelActors::CheckError_CurrentLevel()
{
	FCSKitEditor_CheckLevelActors_ErrorData ErrorData;
	CollectError(ErrorData);
	OutputCollectError(ErrorData);
	
	return ErrorData.mList.Num() > 0;
}

/**
 * @brief	開いてるレベルに対してエラーを収集
 */
void UCSKitEditor_EUW_CheckLevelActors::CollectError(FCSKitEditor_CheckLevelActors_ErrorData& OutErrorData)
{
	for (UCSKitEditor_CheckLevelActors_Base* CheckLevelActors : mCheckLevelActorsClassList)
	{
		CheckLevelActors->CheckError(OutErrorData, GetWorld());
	}
}

/**
 * @brief	エラー情報を出力
 */
void UCSKitEditor_EUW_CheckLevelActors::OutputCollectError(const FCSKitEditor_CheckLevelActors_ErrorData& InErrorData)
{
	OutputFileResultError(InErrorData);
	RequestMessageLog(InErrorData);
	RequestNotification(InErrorData);
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

/**
 * @brief	エラー結果を通知ウィンドウでも出す
 */
void UCSKitEditor_EUW_CheckLevelActors::RequestNotification(
	const FCSKitEditor_CheckLevelActors_ErrorData& InErrorData) const
{
	const bool bOwnError = InErrorData.mList.Num() > 0;
	FString NotificationString;
	if (bOwnError)
	{
		NotificationString = FString::Printf(TEXT("CheckLevelActors Error(%d)"), InErrorData.mList.Num());
	}
	else
	{
		NotificationString = FString::Printf(TEXT("CheckLevelActors NoError"));
	}

	FNotificationInfo Info(FText::FromString(NotificationString));
	Info.bFireAndForget = true;
	Info.FadeOutDuration = 1.0f;
	Info.ExpireDuration = 2.0f;
	TSharedPtr<SNotificationItem> Notification = FSlateNotificationManager::Get().AddNotification(Info);
	if (Notification.IsValid())
	{
		if (bOwnError)
		{
			Notification->SetCompletionState(SNotificationItem::CS_Fail);
		}
		else
		{
			Notification->SetCompletionState(SNotificationItem::CS_Success);
		}
	}
}

/**
 * @brief	自動実行開始時処理
 */
void UCSKitEditor_EUW_CheckLevelActors::AutoRun_OnBegin()
{
	mAutoRun_CollectErrorMap.Clear();
	mAutoRunDataTableRowIndex = 0;
}

/**
 * @brief	自動実行時のSubLevelロード後の処理
 *			trueを返す間継続
 */
bool UCSKitEditor_EUW_CheckLevelActors::AutoRun_ExecPostLoadSubLevel()
{
	CollectError(mAutoRun_CollectErrorMap);
	return true;
}

/**
 * @brief	自動実行終了時処理
 */
void UCSKitEditor_EUW_CheckLevelActors::AutoRun_OnEnd()
{
	OutputCollectError(mAutoRun_CollectErrorMap);
}
