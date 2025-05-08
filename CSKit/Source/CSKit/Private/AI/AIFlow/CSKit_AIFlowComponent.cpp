// Copyright 2020 megasuraman.
/**
 * @file CSKit_AIFlowComponent.cpp
 * @brief 配置での行動指定管理Component
 * @author megasuraman
 * @date 2025/05/05
 */
#include "AI/AIFlow/CSKit_AIFlowComponent.h"

#include "AI/CSKit_AIController.h"
#include "CSKit_Subsystem.h"
#include "AI/AIFlow/CSKit_AIFlow.h"
#include "AI/AIFlow/CSKit_AIFlowDataTable.h"

#if USE_CSKIT_DEBUG
#include "CSKitDebug_Subsystem.h"
#endif

UCSKit_AIFlowComponent::UCSKit_AIFlowComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UCSKit_AIFlowComponent::BeginPlay()
{
	Super::BeginPlay();
}

/* ------------------------------------------------------------
   !
------------------------------------------------------------ */
void UCSKit_AIFlowComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

/* ------------------------------------------------------------
   !
------------------------------------------------------------ */
void UCSKit_AIFlowComponent::SetAIFlow(const ACSKit_AIFlow* InAIFlow)
{
	mAIFlow = InAIFlow;
}

/* ------------------------------------------------------------
   !
------------------------------------------------------------ */
void UCSKit_AIFlowComponent::SetupAIFlowNodeIndex()
{
	const ACSKit_AIController* AIController = Cast<ACSKit_AIController>(GetOwner());
	const ACSKit_AIFlow* AIFlow = mAIFlow.Get();
	if(AIController == nullptr
		|| AIFlow == nullptr)
	{
		mNextIndex = INDEX_NONE;
		return;
	}
	const APawn* OwnerPawn = AIController->GetPawn();
	if(OwnerPawn == nullptr)
	{
		mNextIndex = INDEX_NONE;
		return;
	}

	//とりあえず近いやつで
	const TArray<FCSKit_AIFlowNodeData>& NodeDataList = AIFlow->GetNodeDataList();
	const FVector BasePos = OwnerPawn->GetActorLocation();
	float MinDistanceSq = FLT_MAX;
	for(const FCSKit_AIFlowNodeData& Data : NodeDataList)
	{
		const float DistanceSq = FVector::DistSquared(BasePos, Data.mPos);
		if(DistanceSq < MinDistanceSq)
		{
			MinDistanceSq = DistanceSq;
			mNextIndex = Data.mIndex;
		}
	}
}

/* ------------------------------------------------------------
   !
------------------------------------------------------------ */
void UCSKit_AIFlowComponent::StepNextAIFlowNodeIndex()
{
	const FCSKit_AIFlowNodeData* TargetNodeData = GetNextNodeData();
	if(TargetNodeData == nullptr
		|| TargetNodeData->mLinkIndexList.Num() == 0)
	{
		mNextIndex = INDEX_NONE;
		return;
	}

	//とりあえず、複数あったらランダム
	const int32 SelectListIndex = UCSKit_Subsystem::msRand.RandRange(0,TargetNodeData->mLinkIndexList.Num()-1);
	mNextIndex = TargetNodeData->mLinkIndexList[SelectListIndex];
}

/* ------------------------------------------------------------
   !
------------------------------------------------------------ */
const ACSKit_AIFlow* UCSKit_AIFlowComponent::GetAIFlow() const
{
	return mAIFlow.Get();
}

/* ------------------------------------------------------------
   !
------------------------------------------------------------ */
const FCSKit_AIFlowNodeData* UCSKit_AIFlowComponent::GetNextNodeData() const
{
	if(mNextIndex < 0)
	{
		return nullptr;
	}
	const ACSKit_AIFlow* AIFlow = mAIFlow.Get();
	if(AIFlow == nullptr)
	{
		return nullptr;
	}
	
	const TArray<FCSKit_AIFlowNodeData>& NodeDataList = AIFlow->GetNodeDataList();
	if(mNextIndex < NodeDataList.Num())
	{
		return &NodeDataList[mNextIndex];
	}
	return nullptr;
}

/* ------------------------------------------------------------
  !
------------------------------------------------------------ */
UBehaviorTree* UCSKit_AIFlowComponent::GetNextNodeAction()
{
	const FCSKit_AIFlowNodeData* TargetNodeData = GetNextNodeData();
	if(TargetNodeData == nullptr)
	{
#if USE_CSKIT_DEBUG
		UCSKitDebug_Subsystem::sOneShotWarning(
			GetWorld(),
			false,
			TEXT("UCSKit_AIFlowComponent::GetNextNodeAction()"),
			FString::Printf(TEXT("NextNodeData null"))
		);
#endif
		return nullptr;
	}
	const FCSKit_AIFlowActionTableRow* AIFlowActionTableRow = GetAIFlowActionTableRow(TargetNodeData->mActionName);
	if(AIFlowActionTableRow == nullptr)
	{
#if USE_CSKIT_DEBUG
		UCSKitDebug_Subsystem::sOneShotWarning(
			GetWorld(),
			false,
			TEXT("UCSKit_AIFlowComponent::GetNextNodeAction()"),
			FString::Printf(TEXT("NoActionData : %s"), *TargetNodeData->mActionName.ToString())
		);
#endif
		return nullptr;
	}
	return AIFlowActionTableRow->mBehaviorTree.LoadSynchronous();
}

#if USE_CSKIT_DEBUG
/* ------------------------------------------------------------
  !ActorWatcher選択時
------------------------------------------------------------ */
FString UCSKit_AIFlowComponent::DebugDrawSelectedActorWatcher(UCanvas* InCanvas) const
{
	FString DebugInfo;

	DebugInfo += FString::Printf(TEXT("[AIFlow]\n"));
	DebugInfo += FString::Printf(TEXT("   mNextIndex : %d\n"), mNextIndex);

	if(const ACSKit_AIFlow* AIFlow = mAIFlow.Get())
	{
		AIFlow->DebugDrawCall(InCanvas);
	}

	return DebugInfo;
}
#endif