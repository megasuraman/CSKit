// Copyright 2020 megasuraman.
/**
 * @file CSKit_BTDecorator_OwnAIFlow.h
 * @brief BTDecorator Communityの結果判定用
 * @author megasuraman
 * @date 2025/05/19
 */
#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "CSKit_BTDecorator_OwnCommunity.generated.h"

/**
 * 
 */
UCLASS(DisplayName = "CSKit OwnCommunity")
class CSKIT_API UCSKit_BTDecorator_OwnCommunity : public UBTDecorator
{
	GENERATED_BODY()
public:
	UCSKit_BTDecorator_OwnCommunity(const FObjectInitializer& ObjectInitializer);

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

protected:
};
