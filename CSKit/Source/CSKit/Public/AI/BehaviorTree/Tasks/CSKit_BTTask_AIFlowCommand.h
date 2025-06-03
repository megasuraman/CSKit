// Copyright 2020 megasuraman.
/**
 * @file CSKit_BTTask_AIFlowCommand.h
 * @brief AIFlow操作
 * @author megasuraman
 * @date 2025/06/03
 */ 
#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "CSKit_BTTask_AIFlowCommand.generated.h"


UENUM(BlueprintType)
enum class ECSKit_BTTaskAIFlowCommandType : uint8
{
	Invalid,
	//AIFlowNode準備
	Setup,
	//AIFlowNodeを次へ進める
	StepNext,
	//AIFlowの値をBlackboardにセット
	SetBlackboard,
};

UCLASS(DisplayName = "CSKit AIFlowCommand")
class CSKIT_API UCSKit_BTTask_AIFlowCommand : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
public:
	UCSKit_BTTask_AIFlowCommand(const FObjectInitializer& ObjectInitializer);
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual FString GetStaticDescription() const override;

protected:
	static EBTNodeResult::Type Setup(UBehaviorTreeComponent& OwnerComp);
	static EBTNodeResult::Type StepNext(UBehaviorTreeComponent& OwnerComp);
	EBTNodeResult::Type SetBlackboard(UBehaviorTreeComponent& OwnerComp) const;

protected:
	UPROPERTY(EditAnywhere, Category = "CSKit_BTTask_AIFlowCommand", meta = (DisplayName = "Type"))
	ECSKit_BTTaskAIFlowCommandType mCommandType = ECSKit_BTTaskAIFlowCommandType::Invalid;
	
#if WITH_EDITOR
public:
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};
