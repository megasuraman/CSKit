// Copyright 2020 megasuraman.
/**
 * @file CSKit_EnvQueryContext_NoticeTarget.cpp
 * @brief EnvQueryContext NoticeTarget取得
 * @author megasuraman
 * @date 2025/06/07
 */
#include "AI/EnvironmentQuery/Contexts/CSKit_EnvQueryContext_NoticeTarget.h"

#include "AI/CSKit_AIController.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Actor.h"
#include "GameFramework/Pawn.h"


void UCSKit_EnvQueryContext_NoticeTarget::ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const
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

	UEnvQueryItemType_Actor::SetContextHelper(ContextData, AIController->GetNoticeTarget());
}