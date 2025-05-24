// Copyright 2020 megasuraman.
/**
 * @file CSKit_BTTask_StopMove.cpp
 * @brief 移動停止Task
 * @author megasuraman
 * @date 2025/05/24
 */
#include "AI/BehaviorTree/Tasks/CSKit_BTTask_StopMove.h"

#include "AI/CSKit_AIController.h"
#include "AI/CSKit_PathFollowingComponent.h"


UCSKit_BTTask_StopMove::UCSKit_BTTask_StopMove(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	NodeName = TEXT("CSKit_StopMove");
	bNotifyTick = true;
	bNotifyTaskFinished = true;
}

/* ------------------------------------------------------------
   !Task開始
------------------------------------------------------------ */
EBTNodeResult::Type UCSKit_BTTask_StopMove::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	return RequestStopMove(OwnerComp, NodeMemory);
}
/* ------------------------------------------------------------
   !静的なパラメータ表示？
------------------------------------------------------------ */
FString UCSKit_BTTask_StopMove::GetStaticDescription() const
{
	const FString KeyDesc("invalid");

	return FString::Printf(TEXT("%s: %s"), *Super::GetStaticDescription(), *KeyDesc);
}

/* ------------------------------------------------------------
   !タスク更新
------------------------------------------------------------ */
void UCSKit_BTTask_StopMove::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	const EBTNodeResult::Type ResultType = RequestStopMove(OwnerComp, NodeMemory);
	if (ResultType != EBTNodeResult::InProgress)
	{
		FinishLatentTask(OwnerComp, ResultType);
	}
}

/* ------------------------------------------------------------
   !停止リクエスト
------------------------------------------------------------ */
EBTNodeResult::Type UCSKit_BTTask_StopMove::RequestStopMove(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	const ACSKit_AIController* MyController = Cast<ACSKit_AIController>(OwnerComp.GetAIOwner());
	if (MyController == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	UCSKit_PathFollowingComponent* PathFollowing = Cast<UCSKit_PathFollowingComponent>(MyController->GetPathFollowingComponent());
	if (PathFollowing == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	PathFollowing->AbortMove(*this, FPathFollowingResultFlags::UserAbort);
	
	return EBTNodeResult::Succeeded;
}