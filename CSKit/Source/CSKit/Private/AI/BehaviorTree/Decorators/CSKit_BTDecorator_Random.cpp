// Copyright 2020 megasuraman.
/**
 * @file CSKit_BTDecorator_Random.cpp
 * @brief BTDecorator Randomの結果判定用
 * @author megasuraman
 * @date 2025/05/19
 */
#include "AI/BehaviorTree/Decorators/CSKit_BTDecorator_Random.h"

#include "CSKit_Subsystem.h"
#include "BehaviorTree/BTCompositeNode.h"
#include "BehaviorTree/BlackboardComponent.h"


UCSKit_BTDecorator_Random::UCSKit_BTDecorator_Random(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	NodeName = "CSKit Random";
	mTextOperation = EArithmeticKeyOperation::Less;
}

bool	UCSKit_BTDecorator_Random::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	const int32 RandomValue = UCSKit_Subsystem::msRand.RandRange(0,100);
	switch (mTextOperation.GetValue())
	{
	case EArithmeticKeyOperation::Type::Equal:
		return (RandomValue == mBorderValue);
	case EArithmeticKeyOperation::Type::NotEqual:
		return (RandomValue != mBorderValue);
	case EArithmeticKeyOperation::Type::Less:
		return (RandomValue < mBorderValue);
	case EArithmeticKeyOperation::Type::LessOrEqual:
		return (RandomValue <= mBorderValue);
	case EArithmeticKeyOperation::Type::Greater:
		return (RandomValue > mBorderValue);
	case EArithmeticKeyOperation::Type::GreaterOrEqual:
		return (RandomValue >= mBorderValue);
	default:
		break;
	}
	return false;
}