// Copyright 2020 megasuraman.
/**
 * @file CSKit_BTDecorator_OwnCommunity.cpp
 * @brief BTDecorator Communityの結果判定用
 * @author megasuraman
 * @date 2025/05/19
 */
#include "AI/BehaviorTree/Decorators/CSKit_BTDecorator_OwnCommunity.h"

#include "AI/CSKit_AIController.h"
#include "BehaviorTree/BTCompositeNode.h"
#include "AI/Community/CSKit_CommunityComponent.h"


UCSKit_BTDecorator_OwnCommunity::UCSKit_BTDecorator_OwnCommunity(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	NodeName = "CSKit Community";
}

bool	UCSKit_BTDecorator_OwnCommunity::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	const ACSKit_AIController* AIController = Cast<ACSKit_AIController>(OwnerComp.GetOwner());
	if(AIController == nullptr)
	{
		return false;
	}
	const UCSKit_CommunityComponent* CommunityComponent = AIController->GetCSKitCommunity();
	if(CommunityComponent == nullptr)
	{
		return false;
	}
	if(!CommunityComponent->IsOwnCommunity())
	{
		return false;
	}
	return true;
}