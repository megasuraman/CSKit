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
	bool CheckError_CurrentLevel();

protected:
	virtual void CollectError(FCSKitEditor_CheckLevelActors_ErrorData& OutErrorData);
	virtual void OutputCollectError(const FCSKitEditor_CheckLevelActors_ErrorData& InErrorData);
	virtual void OutputFileResultError(const FCSKitEditor_CheckLevelActors_ErrorData& InErrorData);
	virtual void RequestMessageLog(const FCSKitEditor_CheckLevelActors_ErrorData& InErrorData) const;
	virtual void RequestNotification(const FCSKitEditor_CheckLevelActors_ErrorData& InErrorData) const;
	virtual void AutoRun_OnBegin() override;
	virtual bool AutoRun_ExecPostLoadSubLevel() override;
	virtual void AutoRun_OnEnd() override;


protected:
	UPROPERTY(EditDefaultsOnly, Category = "CheckLevelActors", Instanced, meta = (DisplayName = "エラーチェッククラス", DisplayPriority=1))
	TArray<UCSKitEditor_CheckLevelActors_Base*> mCheckLevelActorsClassList;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LevelActorsClass", meta = (DisplayName = "配置ActorのClass収集", DisplayPriority=1))
	TMap<FString, FCSKitEditor_CheckLevelActors_ClassList> mLevelActorsClassList;
	
	FCSKitEditor_CheckLevelActors_ErrorData mAutoRun_CollectErrorMap;
};
