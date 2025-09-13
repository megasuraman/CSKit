// Copyright 2020 megasuraman
/**
 * @file CSKitEditor_CheckLevelActors_InvalidClass.cpp
 * @brief 許可してない配置Actorがないかチェック
 * @author megasuraman
 * @date 2025/09/13
 */

#include "EditorUtilityWidget/CheckLevelActors/CSKitEditor_CheckLevelActors_InvalidClass.h"
#include "CSKitDebug_Utility.h"

/**
 * @brief	エラーチェック
 */
bool FCSKitEditor_CheckLevelActors_InvalidClass::CheckErrorActor(
	FCSKitEditor_CheckLevelActors_ErrorDataList& OutError,
	const ULevel* InLevel, 
	const AActor* InActor
	) const
{
	const FString LevelName = UCSKitDebug_Utility::GetLevelName(InLevel);
	const FCSKitEditor_CheckLevelActors_ClassList* ActorClassList = FindPossibleToPlaceActorClassList(LevelName);
	if (ActorClassList == nullptr)
	{
		return false;
	}

	bool bPossibleToPlace = false;
	for (const TSoftClassPtr<AActor>& PossibleToPlaceActorClass : ActorClassList->mClassList)
	{
		if (InActor->IsA(PossibleToPlaceActorClass.Get()))
		{
			bPossibleToPlace = true;
			break;
		}
	}
	if (bPossibleToPlace)
	{
		return false;
	}

	FCSKitEditor_CheckLevelActors_ErrorData ErrorData(*InActor);
	ErrorData.mErrorString = FString(TEXT("想定外のActorが配置されてる"));
	OutError.mList.AddUnique(ErrorData);
	return true;
}

/**
 * @brief	エラーチェック
 */
const FCSKitEditor_CheckLevelActors_ClassList* FCSKitEditor_CheckLevelActors_InvalidClass::FindPossibleToPlaceActorClassList(const FString& InLevelName) const
{
	const FCSKitEditor_CheckLevelActors_ClassList* ActorClassList = nullptr;
	for (const auto& MapElement : mPossibleToPlaceActorClassMap)
	{
		const FString TargetLevelName = MapElement.Key;
		const FCSKitEditor_CheckLevelActors_ClassList& PossibleToPlaceClassList = MapElement.Value;
		if (TargetLevelName.Contains(TEXT("*")))
		{
			if (InLevelName.MatchesWildcard(TargetLevelName))
			{
				ActorClassList = &PossibleToPlaceClassList;
			}
		}
		else
		{
			if (InLevelName.Contains(TargetLevelName))
			{
				ActorClassList = &PossibleToPlaceClassList;
				break;
			}
		}
	}

	return ActorClassList;
}
