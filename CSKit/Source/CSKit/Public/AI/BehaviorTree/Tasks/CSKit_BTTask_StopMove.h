// Copyright 2020 megasuraman.
/**
 * @file CSKit_BTTask_StopMove.h
 * @brief 移動停止Task
 * @author megasuraman
 * @date 2025/05/24
 */
#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "CSKit_BTTask_StopMove.generated.h"


UCLASS(DisplayName = "CSKit StopMove")
class CSKIT_API UCSKit_BTTask_StopMove : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UCSKit_BTTask_StopMove(const FObjectInitializer& ObjectInitializer);
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual FString GetStaticDescription() const override;

protected:
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	virtual EBTNodeResult::Type	RequestStopMove(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);

protected:
};
