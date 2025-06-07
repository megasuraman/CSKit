// Copyright 2020 megasuraman.
/**
 * @file CSKit_EnvQueryContext_NoticeTarget.h
 * @brief EnvQueryContext NoticeTarget取得
 * @author megasuraman
 * @date 2025/06/07
 */
#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryContext.h"
#include "CSKit_EnvQueryContext_NoticeTarget.generated.h"

UCLASS()
class CSKIT_API UCSKit_EnvQueryContext_NoticeTarget : public UEnvQueryContext
{
	GENERATED_BODY()

	virtual void ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const override;
};
