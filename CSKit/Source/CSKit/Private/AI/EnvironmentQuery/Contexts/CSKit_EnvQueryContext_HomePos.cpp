// Copyright 2020 megasuraman.
/**
 * @file CSKit_EnvQueryContext_HomePos.cpp
 * @brief EnvQueryContext HomePos取得
 * @author megasuraman
 * @date 2025/06/07
 */
#include "AI/EnvironmentQuery/Contexts/CSKit_EnvQueryContext_HomePos.h"

#include "AI/CSKit_AIController.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Point.h"
#include "GameFramework/Pawn.h"


void UCSKit_EnvQueryContext_HomePos::ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const
{
	const APawn* QueryOwner = Cast<APawn>(QueryInstance.Owner.Get());
	if (QueryOwner == nullptr)
	{
		return;
	}
	const ACSKit_AIController* AIController = Cast<ACSKit_AIController>(QueryOwner->GetController());
	if (AIController == nullptr)
	{
		return;
	}

	UEnvQueryItemType_Point::SetContextHelper(ContextData, AIController->GetHomePos());
}