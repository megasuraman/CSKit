// Copyright 2020 megasuraman
/**
 * @file CSKitEditor_EUW_CheckLevelActors.h
 * @brief 配置Actorに対するチェック
 * @author megasuraman
 * @date 2024/11/30
 */

#pragma once

#include "CoreMinimal.h"
#include "CheckLevelActors/CSKitEditor_CheckLevelActors_InvalidClass.h"
#include "EditorUtilityWidget/CSKitEditor_EUW_Base.h"
#include "CSKitEditor_EUW_CheckLevelActors.generated.h"

UCLASS()
class CSKITEDITOR_API UCSKitEditor_EUW_CheckLevelActors : public UCSKitEditor_EUW_Base
{
	GENERATED_BODY()

protected:

public:
	UFUNCTION(BlueprintCallable)
	void CollectLevelActorsClass();
	UFUNCTION(BlueprintCallable)
	bool CheckError();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CheckLevelActors", meta = (DisplayName = "配置可能Actorチェック", DisplayPriority=1))
	FCSKitEditor_CheckLevelActors_InvalidClass mCheckInvalidClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LevelActorsClass", meta = (DisplayName = "配置ActorのClass収集", DisplayPriority=1))
	TMap<FString, FCSKitEditor_CheckLevelActors_ClassList> mLevelActorsClassList;
};
