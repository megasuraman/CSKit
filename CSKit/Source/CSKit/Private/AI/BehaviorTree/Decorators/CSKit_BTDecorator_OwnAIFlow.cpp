// Copyright 2020 megasuraman.
/**
 * @file CSKit_BTDecorator_OwnAIFlow.cpp
 * @brief BTDecorator AIFlowの結果判定用
 * @author megasuraman
 * @date 2025/05/19
 */
#include "AI/BehaviorTree/Decorators/CSKit_BTDecorator_OwnAIFlow.h"

#include "AI/CSKit_AIController.h"
#include "BehaviorTree/BTCompositeNode.h"
#include "AI/AIFlow/CSKit_AIFlowComponent.h"


UCSKit_BTDecorator_OwnAIFlow::UCSKit_BTDecorator_OwnAIFlow(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	NodeName = "CSKit AIFlow";
}

bool	UCSKit_BTDecorator_OwnAIFlow::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	const ACSKit_AIController* AIController = Cast<ACSKit_AIController>(OwnerComp.GetOwner());
	if(AIController == nullptr)
	{
		return false;
	}
	const UCSKit_AIFlowComponent* AIFlowComponent = AIController->GetCSKitAIFlowComponent();
	if(AIFlowComponent == nullptr)
	{
		return false;
	}
	if(AIFlowComponent->GetAIFlow() == nullptr)
	{
		return false;
	}
	return true;
}