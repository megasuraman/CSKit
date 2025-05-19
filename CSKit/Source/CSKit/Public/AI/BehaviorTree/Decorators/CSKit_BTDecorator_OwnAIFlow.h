// Copyright 2020 megasuraman.
/**
 * @file CSKit_BTDecorator_OwnAIFlow.cpp
 * @brief BTDecorator AIFlowの結果判定用
 * @author megasuraman
 * @date 2025/05/19
 */
#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "CSKit_BTDecorator_OwnAIFlow.generated.h"

UCLASS(DisplayName = "CSKit OwnAIFlow")
class CSKIT_API UCSKit_BTDecorator_OwnAIFlow : public UBTDecorator
{
	GENERATED_BODY()
public:
	UCSKit_BTDecorator_OwnAIFlow(const FObjectInitializer& ObjectInitializer);

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

protected:
};
