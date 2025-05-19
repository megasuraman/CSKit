// Copyright 2020 megasuraman.
/**
 * @file CSKit_BTDecorator_RepeatTestWorry.cpp
 * @brief 心配判定Decorator
 * @author megasuraman
 * @date 2025/05/19
 */
#include "AI/BehaviorTree/Decorators/CSKit_BTDecorator_RepeatTestWorry.h"

#include "AI/CSKit_AIController.h"
#include "BehaviorTree/BTCompositeNode.h"
#include "AI/Worry/CSKit_WorryComponent.h"

UCSKit_BTDecorator_RepeatTestWorry::UCSKit_BTDecorator_RepeatTestWorry(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	NodeName = "CSKit_Worry";
}

/* ------------------------------------------------------------
   !FBlackboardKeySelectorの初期化のために
------------------------------------------------------------ */
void UCSKit_BTDecorator_RepeatTestWorry::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);
}

/* ------------------------------------------------------------
   !条件判定
------------------------------------------------------------ */
bool UCSKit_BTDecorator_RepeatTestWorry::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	const ACSKit_AIController* AIController = Cast<ACSKit_AIController>( OwnerComp.GetAIOwner() );
	if (AIController == nullptr)
	{
		return false;
	}
	const UCSKit_WorryComponent* WorryComponent = AIController->GetCSKitWorryComponent();
	if (WorryComponent == nullptr)
	{
		return false;
	}

	return WorryComponent->IsWishCheckWorryPoint();
}