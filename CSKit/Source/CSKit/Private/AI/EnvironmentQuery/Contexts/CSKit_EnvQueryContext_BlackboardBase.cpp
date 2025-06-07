// Copyright 2020 megasuraman.
/**
 * @file CSKit_EnvQueryContext_CommunityLeader.cpp
 * @brief EnvQueryContext Blackboardの座標orActorを参照(継承先でKeyを指定して使う)
 * @author megasuraman
 * @date 2025/06/07
 */
#include "AI/EnvironmentQuery/Contexts/CSKit_EnvQueryContext_BlackboardBase.h"

#include "AI/CSKit_AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Actor.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Point.h"
#include "GameFramework/Pawn.h"


void UCSKit_EnvQueryContext_BlackboardBase::ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const
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

	if(!mBBKeyName_Vector.IsNone())
	{
		if(const UBlackboardComponent* BlackboardComponent = AIController->GetBlackboardComponent())
		{
			UEnvQueryItemType_Point::SetContextHelper(ContextData, BlackboardComponent->GetValueAsVector(mBBKeyName_Vector));
		}
	}
	else if(!mBBKeyName_Actor.IsNone())
	{
		if(const UBlackboardComponent* BlackboardComponent = AIController->GetBlackboardComponent())
		{
			if(const AActor* Actor = Cast<AActor>(BlackboardComponent->GetValueAsObject(mBBKeyName_Actor)))
			{
				UEnvQueryItemType_Actor::SetContextHelper(ContextData, Actor);
			}
		}
	}
}