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
	FCSKitEditor_CheckLevelActors_ErrorDataList ErrorData;
	mCheckInvalidClass.CheckError(ErrorData, GetWorld());
	return ErrorData.mList.Num() > 0;
}