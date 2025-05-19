// Copyright 2020 megasuraman.
/**
 * @file CSKit_BTDecorator_RepeatTestWorry.cpp
 * @brief 心配判定Decorator
 * @author megasuraman
 * @date 2025/05/19
 */
#pragma once

#include "CoreMinimal.h"
#include "AI/BehaviorTree/Decorators/CSKit_BTDecorator_RepeatTestBase.h"
#include "CSKit_BTDecorator_RepeatTestWorry.generated.h"

UCLASS(DisplayName = "CSKit Repeat Test Worry")
class CSKIT_API UCSKit_BTDecorator_RepeatTestWorry : public UCSKit_BTDecorator_RepeatTestBase
{
	GENERATED_BODY()
public:
	UCSKit_BTDecorator_RepeatTestWorry(const FObjectInitializer& ObjectInitializer);

	/** initialize any asset related data */
	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};
