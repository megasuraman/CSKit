// Copyright 2020 megasuraman.
/**
 * @file CSKit_EnvQueryContext_CommunityLeader.cpp
 * @brief EnvQueryContext CommunityのLeader取得
 * @author megasuraman
 * @date 2025/06/07
 */
#include "AI/EnvironmentQuery/Contexts/CSKit_EnvQueryContext_CommunityLeader.h"

#include "AI/CSKit_AIController.h"
#include "AI/Community/CSKit_CommunityComponent.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Actor.h"
#include "GameFramework/Pawn.h"


void UCSKit_EnvQueryContext_CommunityLeader::ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const
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

	const UCSKit_CommunityComponent* CommunityComponent = AIController->GetCSKitCommunity();
	if(CommunityComponent == nullptr)
	{
		return;
	}

	const AActor* LeaderActor = CommunityComponent->GetLeaderPawn();
	UEnvQueryItemType_Actor::SetContextHelper(ContextData, LeaderActor);
}