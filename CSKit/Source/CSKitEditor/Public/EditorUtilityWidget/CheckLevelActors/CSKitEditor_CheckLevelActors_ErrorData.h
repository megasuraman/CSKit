// Copyright 2020 megasuraman
/**
 * @file CSKitEditor_CheckLevelActors_ErrorData.h
 * @brief 配置Actorチェックのエラー情報
 * @author megasuraman
 * @date 2025/09/13
 */

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

struct CSKITEDITOR_API FCSKitEditor_CheckLevelActors_ErrorDataNode
{
	TWeakObjectPtr<const AActor> mActor;
	FString mLevelName;
	FString mLabelName;
	FString mClassName;
	FString mErrorString;

	bool operator ==(const FCSKitEditor_CheckLevelActors_ErrorDataNode& InData) const
	{
		if(mLevelName == InData.mLevelName
			&& mLabelName == InData.mLabelName
			&& mErrorString == InData.mErrorString)
		{
			return true;
		}
		return false;
	}
	FCSKitEditor_CheckLevelActors_ErrorDataNode(){}
	FCSKitEditor_CheckLevelActors_ErrorDataNode(const AActor& InActor);
};

struct CSKITEDITOR_API FCSKitEditor_CheckLevelActors_ErrorData
{
	TArray<FCSKitEditor_CheckLevelActors_ErrorDataNode> mList;
};