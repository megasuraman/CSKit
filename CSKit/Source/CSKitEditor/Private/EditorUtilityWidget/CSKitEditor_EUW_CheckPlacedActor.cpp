// Copyright 2020 megasuraman
/**
 * @file CSKitEditor_EUW_CheckPlacedActor.cpp
 * @brief 配置Actorに対するチェック
 * @author megasuraman
 * @date 2024/11/30
 */


#include "EditorUtilityWidget/CSKitEditor_EUW_CheckPlacedActor.h"
#include "Engine/Classes/GameFramework/WorldSettings.h"
#include "CSKitDebug_Utility.h"

/**
 * @brief	配置Actorのクラス情報をレベル別に格納
 */
void UCSKitEditor_EUW_CheckPlacedActor::SetupPlacedActorClassMap()
{
	mPlacedActorClassMap.Empty();

	const TArray<ULevel*>& Levels = GetWorld()->GetLevels();
	for (ULevel* Level : Levels)
	{
		const FString LevelName = UCSKitDebug_Utility::GetLevelName(Level);
		FCSKitEditor_PlacedActorClassList& PlacedActorClassList = mPlacedActorClassMap.FindOrAdd(LevelName);
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
bool UCSKitEditor_EUW_CheckPlacedActor::CheckError()
{
	bool bError = false;
	bError |= CheckError_PossibleToPlaceActor();
	return bError;
}

/**
 * @brief	指定のレベルに配置想定じゃないActorが存在しないかチェック
 */
bool UCSKitEditor_EUW_CheckPlacedActor::CheckError_PossibleToPlaceActor()
{
	bool bError = false;
	const TArray<ULevel*>& Levels = GetWorld()->GetLevels();
	for (const auto& MapElement : mPossibleToPlaceActorClassMap)
	{
		const FString TargetLevelNameOrigin = MapElement.Key;
		const FCSKitEditor_PlacedActorClassList& PossibleToPlaceClassList = MapElement.Value;

		FString TempString;
		FString TargetLevelName;
		if (!TargetLevelNameOrigin.Split(FString(TEXT("*")), &TempString, &TargetLevelName))
		{
			TargetLevelName = TargetLevelNameOrigin;
		}

		for (ULevel* Level : Levels)
		{
			const FString LevelName = UCSKitDebug_Utility::GetLevelName(Level);
			if (!LevelName.Contains(TargetLevelName))
			{
				continue;
			}

			for (const AActor* Actor : Level->Actors)
			{
				if (Actor == nullptr
					|| Actor->IsA(AWorldSettings::StaticClass())
					|| Actor->GetClass() == ABrush::StaticClass()
					)
				{
					continue;
				}

				bool bPossibleToPlace = false;
				for (const TSoftClassPtr<AActor>& PossibleToPlaceActorClass : PossibleToPlaceClassList.mClassList)
				{
					if (Actor->IsA(PossibleToPlaceActorClass.Get()))
					{
						bPossibleToPlace = true;
						break;
					}
				}
				if (!bPossibleToPlace)
				{
					UE_LOG(LogTemp, Error, TEXT("Impossible to place Actor : [%s]<%s> %s"),
						*LevelName,
						*Actor->GetClass()->GetName(),
						*Actor->GetActorLabel()
					);
					bError = true;
				}
			}
		}
	}
	return bError;
}