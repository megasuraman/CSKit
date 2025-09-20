// Copyright 2020 megasuraman
/**
 * @file CSKitEditor_CheckLevelActors_Base.h
 * @brief 配置Actorチェック基底クラス
 * @author megasuraman
 * @date 2025/09/13
 */

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget/CheckLevelActors/CSKitEditor_CheckLevelActors_ErrorData.h"
#include "CSKitEditor_CheckLevelActors_Base.generated.h"

UCLASS(BlueprintType, EditInlineNew, Abstract)
class CSKITEDITOR_API UCSKitEditor_CheckLevelActors_Base : public UObject
{
	GENERATED_BODY()

public:
	UCSKitEditor_CheckLevelActors_Base(){}
	
	virtual bool CheckError(FCSKitEditor_CheckLevelActors_ErrorData& OutError, const UWorld* InWorld) const;

protected:
	//チェック対象から除外するLevelかどうか
	virtual bool IsIgnoreLevel(const ULevel* InLevel) const {return false;}
	//チェック対象から除外するActorかどうか
	virtual bool IsIgnoreActor(const AActor* InActor) const;
	//指定のActorのエラーをチェック
	virtual bool CheckErrorActor(FCSKitEditor_CheckLevelActors_ErrorData& OutError, const ULevel* InLevel, const AActor* InActor) const{return false;}
};
