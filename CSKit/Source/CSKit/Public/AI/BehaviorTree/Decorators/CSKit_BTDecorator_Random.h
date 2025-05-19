// Copyright 2020 megasuraman.
/**
 * @file CSKit_BTDecorator_Random.cpp
 * @brief BTDecorator Randomの結果判定用
 * @author megasuraman
 * @date 2025/05/19
 */
#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#if ENGINE_MAJOR_VERSION == 5
#include "BehaviorTree/Blackboard/BlackboardKeyEnums.h"
#else
#include "BehaviorTree/Blackboard/BlackboardKeyType.h"
#endif
#include "CSKit_BTDecorator_Random.generated.h"

UCLASS(DisplayName = "CSKit Random")
class CSKIT_API UCSKit_BTDecorator_Random : public UBTDecorator
{
	GENERATED_BODY()
public:
	UCSKit_BTDecorator_Random(const FObjectInitializer& ObjectInitializer);

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

protected:
	UPROPERTY(EditAnywhere, Category = "Condition", meta = (DisplayName = "判定値(0～100)", ClampMin = "0", ClampMax = "100", DisplayPriority = 1))
	int32 mBorderValue = 0;
	UPROPERTY(EditAnywhere, Category = "Condition", meta = (DisplayName = "判定", DisplayPriority = 2))
	TEnumAsByte<EArithmeticKeyOperation::Type> mTextOperation;
};
