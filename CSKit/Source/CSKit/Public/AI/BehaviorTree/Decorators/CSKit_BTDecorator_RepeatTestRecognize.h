// Copyright 2020 megasuraman.
/**
 * @file CSKit_BTDecorator_RepeatTestRecognize.h
 * @brief BTDecorator 認知判定
 * @author megasuraman
 * @date 2025/05/19
 */
#pragma once

#include "CoreMinimal.h"
#include "AI/BehaviorTree/Decorators/CSKit_BTDecorator_RepeatTestBase.h"
#include "CSKit_BTDecorator_RepeatTestRecognize.generated.h"

UCLASS(DisplayName = "CSKit Repeat Test Recognize")
class CSKIT_API UCSKit_BTDecorator_RepeatTestRecognize : public UCSKit_BTDecorator_RepeatTestBase
{
	GENERATED_BODY()
public:
	UCSKit_BTDecorator_RepeatTestRecognize(const FObjectInitializer& ObjectInitializer);

	/** initialize any asset related data */
	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;

protected:
	UPROPERTY(EditAnywhere, Category = "Condition", DisplayName = "判定対象")
	FBlackboardKeySelector mBBKey_Target;

	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};
