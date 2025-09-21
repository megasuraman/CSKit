// Copyright 2020 megasuraman
/**
 * @file CSKitEditor_CheckLevelActors_NullAssign.h
 * @brief アサインのない配置チェック
 * @author megasuraman
 * @date 2025/09/21
 */

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget/CheckLevelActors/CSKitEditor_CheckLevelActors_Base.h"
#include "CSKitEditor_CheckLevelActors_NullAssign.generated.h"


UCLASS(BlueprintType)
class CSKITEDITOR_API UCSKitEditor_CheckLevelActors_NullAssign : public UCSKitEditor_CheckLevelActors_Base
{
	GENERATED_BODY()

protected:
	virtual bool CheckErrorActor(FCSKitEditor_CheckLevelActors_ErrorData& OutError, const ULevel* InLevel, const AActor* InActor) override;

protected:
};