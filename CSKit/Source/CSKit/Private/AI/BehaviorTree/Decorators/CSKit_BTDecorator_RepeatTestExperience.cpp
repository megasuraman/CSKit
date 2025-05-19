// Copyright 2020 megasuraman.
/**
 * @file CSKit_BTDecorator_RepeatTestExperience.cpp
 * @brief BTDecorator Experience判定
 * @author megasuraman
 * @date 2025/05/19
 */
#include "AI/BehaviorTree/Decorators/CSKit_BTDecorator_RepeatTestExperience.h"

#include "AI/CSKit_AIController.h"
#include "BehaviorTree/BTCompositeNode.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Float.h"
#include "AI/Experience/CSKit_ExperienceComponent.h"

UCSKit_BTDecorator_RepeatTestExperience::UCSKit_BTDecorator_RepeatTestExperience(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	NodeName = "CSKitRepeatTest_Experience";

	mTextOperation = EArithmeticKeyOperation::Less;
}

/* ------------------------------------------------------------
   !FBlackboardKeySelectorの初期化のために
------------------------------------------------------------ */
void UCSKit_BTDecorator_RepeatTestExperience::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);
}

/* ------------------------------------------------------------
   !条件判定
------------------------------------------------------------ */
bool UCSKit_BTDecorator_RepeatTestExperience::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	const ACSKit_AIController* AIController = Cast<ACSKit_AIController>( OwnerComp.GetAIOwner() );
	if (AIController == nullptr)
	{
		return false;
	}
	const UCSKit_ExperienceComponent* ExperienceComponent = AIController->GetCSKitExperience();
	if (ExperienceComponent == nullptr)
	{
		return false;
	}
	const float Score = ExperienceComponent->GetScoreByName(mExperienceTypeName);
	switch (mTextOperation.GetValue())
	{
	case EArithmeticKeyOperation::Type::Equal:
		return (Score == mBorderScore);
	case EArithmeticKeyOperation::Type::NotEqual:
		return (Score != mBorderScore);
	case EArithmeticKeyOperation::Type::Less:
		return (Score < mBorderScore);
	case EArithmeticKeyOperation::Type::LessOrEqual:
		return (Score <= mBorderScore);
	case EArithmeticKeyOperation::Type::Greater:
		return (Score > mBorderScore);
	case EArithmeticKeyOperation::Type::GreaterOrEqual:
		return (Score >= mBorderScore);
	default:
		break;
	}

	return false;
}