// Copyright 2020 megasuraman.
/**
 * @file CSKit_EnvQueryContext_CommunityLeader.h
 * @brief EnvQueryContext Blackboardの座標orActorを参照(継承先でKeyを指定して使う)
 * @author megasuraman
 * @date 2025/06/07
 */
#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryContext.h"
#include "CSKit_EnvQueryContext_BlackboardBase.generated.h"

UCLASS(Abstract, Blueprintable)
class CSKIT_API UCSKit_EnvQueryContext_BlackboardBase : public UEnvQueryContext
{
	GENERATED_BODY()

	virtual void ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = BlackboardBase)
	FName mBBKeyName_Vector;
	UPROPERTY(EditDefaultsOnly, Category = BlackboardBase)
	FName mBBKeyName_Actor;
};
