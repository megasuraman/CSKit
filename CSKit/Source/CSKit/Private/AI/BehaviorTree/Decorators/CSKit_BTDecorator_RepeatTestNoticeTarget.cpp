// Copyright 2020 megasuraman.
/**
 * @file CSKit_BTDecorator_RepeatTestNoticeTarget.cpp
 * @brief BTDecorator Notice判定
 * @author megasuraman
 * @date 2025/05/19
 */
#include "AI/BehaviorTree/Decorators/CSKit_BTDecorator_RepeatTestNoticeTarget.h"

#include "AI/CSKit_AIController.h"
#include "BehaviorTree/BTCompositeNode.h"
#include "AI/Community/CSKit_CommunityComponent.h"
#include "AI/Community/CSKit_CommunityNodeBase.h"

UCSKit_BTDecorator_RepeatTestNoticeTarget::UCSKit_BTDecorator_RepeatTestNoticeTarget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	NodeName = "CSKit_NoticeTarget";
}

/* ------------------------------------------------------------
   !条件判定
------------------------------------------------------------ */
bool UCSKit_BTDecorator_RepeatTestNoticeTarget::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	const ACSKit_AIController* AIController = Cast<ACSKit_AIController>( OwnerComp.GetAIOwner() );
	if (AIController == nullptr)
	{
		return false;
	}
	if(AIController->GetNoticeTarget() != nullptr)
	{
		return true;
	}
	if(mbCheckCommunity)
	{
		if(const UCSKit_CommunityComponent* CommunityComponent = AIController->GetCSKitCommunity())
		{
			if(const UCSKit_CommunityNodeBase* CommunityNode = CommunityComponent->GetEntryNode())
			{
				if(CommunityNode->IsOwnNoticeTarget())
				{
					return true;
				}
			}
		}
	}

	return false;
}