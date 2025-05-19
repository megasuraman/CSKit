// Copyright 2020 megasuraman.
/**
 * @file CSKit_BTDecorator_RepeatTestRecognize.cpp
 * @brief BTDecorator 認知判定
 * @author megasuraman
 * @date 2025/05/19
 */
#include "AI/BehaviorTree/Decorators/CSKit_BTDecorator_RepeatTestRecognize.h"

#include "AI/CSKit_AIController.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "BehaviorTree/BTCompositeNode.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AI/Recognition/CSKit_RecognitionComponent.h"

UCSKit_BTDecorator_RepeatTestRecognize::UCSKit_BTDecorator_RepeatTestRecognize(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	NodeName = "CSKit_TargetRecognize";

	mBBKey_Target.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UCSKit_BTDecorator_RepeatTestRecognize, mBBKey_Target), AActor::StaticClass());
}

/* ------------------------------------------------------------
   !FBlackboardKeySelectorの初期化のために
------------------------------------------------------------ */
void UCSKit_BTDecorator_RepeatTestRecognize::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);

	if (const UBlackboardData* BBAsset = GetBlackboardAsset())
	{
		mBBKey_Target.ResolveSelectedKey(*BBAsset);
	}
	else
	{
		UE_LOG(LogBehaviorTree, Warning, TEXT("Can't initialize %s due to missing blackboard data."), *GetName());
		mBBKey_Target.InvalidateResolvedKey();
	}
}

/* ------------------------------------------------------------
   !条件判定
------------------------------------------------------------ */
bool UCSKit_BTDecorator_RepeatTestRecognize::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	const ACSKit_AIController* AIController = Cast<ACSKit_AIController>( OwnerComp.GetAIOwner() );
	if (AIController == nullptr)
	{
		return false;
	}
	const UCSKit_RecognitionComponent* RecognitionComponent = AIController->GetCSKitRecognition();
	if (RecognitionComponent == nullptr)
	{
		return false;
	}

	const UBlackboardComponent* MyBlackboard = OwnerComp.GetBlackboardComponent();
	if (const AActor* TargetActor = Cast<AActor>(MyBlackboard->GetValue<UBlackboardKeyType_Object>(mBBKey_Target.GetSelectedKeyID())))
	{
		if(const FCSKitRecognitionTarget* RecognitionTarget = RecognitionComponent->FindRecognitionTarget(TargetActor))
		{
			return RecognitionTarget->mbRecognize;
		}
	}

	return false;
}