// Copyright 2020 megasuraman.
/**
 * @file CSKit_BTDecorator_RepeatTestAIAbility.cpp
 * @brief BTDecorator AIAbility判定
 * @author megasuraman
 * @date 2025/05/19
 */
#include "AI/BehaviorTree/Decorators/CSKit_BTDecorator_RepeatTestAIAbility.h"

#include "AI/CSKit_AIController.h"
#include "BehaviorTree/BTCompositeNode.h"
#include "BehaviorTree/BlackboardComponent.h"

UCSKit_BTDecorator_RepeatTestAIAbility::UCSKit_BTDecorator_RepeatTestAIAbility(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	NodeName = "CSKit_TargetAIAbility";
}

/**
 * @brief 
 */
void UCSKit_BTDecorator_RepeatTestAIAbility::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);
}

/* ------------------------------------------------------------
   !条件判定
------------------------------------------------------------ */
bool UCSKit_BTDecorator_RepeatTestAIAbility::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	const ACSKit_AIController* AIController = Cast<ACSKit_AIController>( OwnerComp.GetAIOwner() );
	if (AIController == nullptr)
	{
		return false;
	}
	return AIController->IsOwnAbility(mAIAbility);
}