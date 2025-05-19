// Copyright 2020 megasuraman.
/**
 * @file CSKit_BTDecorator_RepeatTestBase.cpp
 * @brief BTDecorator ノード変化時以外も繰り返し条件チェック
 * @author megasuraman
 * @date 2025/05/19
 */
#include "AI/BehaviorTree/Decorators/CSKit_BTDecorator_RepeatTestBase.h"

#include "BehaviorTree/BTCompositeNode.h"
#include "BehaviorTree/BlackboardComponent.h"

UCSKit_BTDecorator_RepeatTestBase::UCSKit_BTDecorator_RepeatTestBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, mbForceOneShot(false)
{
	NodeName = "CSKit_RepeatTestBase";
	bNotifyActivation = true;
	bNotifyTick = true;
	bTickIntervals = true;

	bAllowAbortLowerPri = false;
	bAllowAbortNone = false;
	FlowAbortMode = EBTFlowAbortMode::Self;
}

void UCSKit_BTDecorator_RepeatTestBase::OnNodeActivation(FBehaviorTreeSearchData& SearchData)
{
	if (uint8* RawMemory = SearchData.OwnerComp.GetNodeMemory(this, SearchData.OwnerComp.FindInstanceContainingNode(this)))
	{
		if (FBTAuxiliaryMemory* DecoratorMemory = GetSpecialNodeMemory<FBTAuxiliaryMemory>(RawMemory))
		{
			DecoratorMemory->NextTickRemainingTime = mTestIntervalSec;
			DecoratorMemory->AccumulatedDeltaTime = 0.0f;
		}
	}
}
void UCSKit_BTDecorator_RepeatTestBase::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	if (!mbForceOneShot
		&& (IsInversed() == CalculateRawConditionValue(OwnerComp, NodeMemory)) )
	{
		OwnerComp.RequestExecution(this);
	}
	SetNextTickTime(NodeMemory, mTestIntervalSec);
}