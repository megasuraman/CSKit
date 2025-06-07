// Copyright 2020 megasuraman.
/**
 * @file CSKit_EnvQueryContext_CommunityLeader.h
 * @brief EnvQueryContext CommunityのLeader取得
 * @author megasuraman
 * @date 2025/06/07
 */
#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryContext.h"
#include "CSKit_EnvQueryContext_CommunityLeader.generated.h"

UCLASS()
class CSKIT_API UCSKit_EnvQueryContext_CommunityLeader : public UEnvQueryContext
{
	GENERATED_BODY()

	virtual void ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const override;
};
