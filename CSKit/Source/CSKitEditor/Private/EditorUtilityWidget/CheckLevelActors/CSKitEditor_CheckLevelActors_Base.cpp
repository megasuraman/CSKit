// Copyright 2020 megasuraman
/**
* @file CSKitEditor_CheckLevelActors_Base.cpp
 * @brief 配置Actorチェック基底クラス
 * @author megasuraman
 * @date 2025/09/13
 */

#include "EditorUtilityWidget/CheckLevelActors/CSKitEditor_CheckLevelActors_Base.h"

#include "GameFramework/WorldSettings.h"

/**
 * @brief	エラーチェック
 */
bool FCSKitEditor_CheckLevelActors_Base::CheckError(FCSKitEditor_CheckLevelActors_ErrorDataList& OutError, const UWorld* InWorld) const
{
	bool bAddError = false;
	const TArray<ULevel*>& Levels = InWorld->GetLevels();
	for (const ULevel* Level : Levels)
	{
		if (IsIgnoreLevel(Level))
		{
			continue;
		}
		for (const AActor* Actor : Level->Actors)
		{
			if (IsIgnoreActor(Actor))
			{
				continue;
			}
			bAddError |= CheckErrorActor(OutError, Level, Actor);
		}
	}
	return bAddError;
}

/**
 * @brief	チェック対象から除外するActorかどうか
 */
bool FCSKitEditor_CheckLevelActors_Base::IsIgnoreActor(const AActor* InActor) const
{
	if (InActor == nullptr
		|| InActor->IsA(AWorldSettings::StaticClass())
		|| InActor->GetClass() == ABrush::StaticClass()
		)
	{
		return true;
	}
	return false;
}
