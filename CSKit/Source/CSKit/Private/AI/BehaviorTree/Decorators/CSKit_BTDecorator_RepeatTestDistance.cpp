// Copyright 2020 megasuraman.
/**
 * @file CSKit_BTDecorator_RepeatTestDistance.cpp
 * @brief BTDecorator 距離判定
 * @author megasuraman
 * @date 2025/05/19
 */
#include "AI/BehaviorTree/Decorators/CSKit_BTDecorator_RepeatTestDistance.h"

#include "AI/CSKit_AIController.h"
#include "BehaviorTree/BTCompositeNode.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Float.h"

UCSKit_BTDecorator_RepeatTestDistance::UCSKit_BTDecorator_RepeatTestDistance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, mbJudgeXY(false)
	, mbJudgeZ(false)
	, mbOverrideBorderDistance(false)
{
	NodeName = "CSKitRepeatTest_Distance";

	mTextOperation = EArithmeticKeyOperation::Less;
	
	mBBKey_OverrideBorderDistance.AddFloatFilter(this, GET_MEMBER_NAME_CHECKED(UCSKit_BTDecorator_RepeatTestDistance, mBBKey_OverrideBorderDistance));
}

/* ------------------------------------------------------------
   !FBlackboardKeySelectorの初期化のために
------------------------------------------------------------ */
void UCSKit_BTDecorator_RepeatTestDistance::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);

	if (const UBlackboardData* BBAsset = GetBlackboardAsset())
	{
		mBBKey_OverrideBorderDistance.ResolveSelectedKey(*BBAsset);
	}
	else
	{
		UE_LOG(LogBehaviorTree, Warning, TEXT("Can't initialize %s due to missing blackboard data."), *GetName());
		mBBKey_OverrideBorderDistance.InvalidateResolvedKey();
	}
}

/* ------------------------------------------------------------
   !条件判定
------------------------------------------------------------ */
bool UCSKit_BTDecorator_RepeatTestDistance::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	const FVector PosA = GetTargetPos(mTargetA, OwnerComp);
	const FVector PosB = GetTargetPos(mTargetB, OwnerComp);
	float BorderDistanceSq = FMath::Square(mBorderDistance + mAddDistance);
	if (mbOverrideBorderDistance
		&& mBBKey_OverrideBorderDistance.SelectedKeyType == UBlackboardKeyType_Float::StaticClass())
	{
		const UBlackboardComponent* MyBlackboard = OwnerComp.GetBlackboardComponent();
		BorderDistanceSq = FMath::Square(MyBlackboard->GetValue<UBlackboardKeyType_Float>(mBBKey_OverrideBorderDistance.GetSelectedKeyID()) + mAddDistance);
	}
	float DistanceSq;
	if (mbJudgeXY)
	{
		DistanceSq = FVector::DistSquaredXY(PosB, PosA);
	}
	else if (mbJudgeZ)
	{
		DistanceSq = FMath::Square(PosB.Z - PosA.Z);
	}
	else
	{
		DistanceSq = FVector::DistSquared(PosB, PosA);
	}
	switch (mTextOperation.GetValue())
	{
	case EArithmeticKeyOperation::Type::Equal:
		return (DistanceSq == BorderDistanceSq);
	case EArithmeticKeyOperation::Type::NotEqual:
		return (DistanceSq != BorderDistanceSq);
	case EArithmeticKeyOperation::Type::Less:
		return (DistanceSq < BorderDistanceSq);
	case EArithmeticKeyOperation::Type::LessOrEqual:
		return (DistanceSq <= BorderDistanceSq);
	case EArithmeticKeyOperation::Type::Greater:
		return (DistanceSq > BorderDistanceSq);
	case EArithmeticKeyOperation::Type::GreaterOrEqual:
		return (DistanceSq >= BorderDistanceSq);
	default:
		break;
	}

	return false;
}

FVector UCSKit_BTDecorator_RepeatTestDistance::GetTargetPos(const ECSKit_BTDecoratorDistanceTargetKind InKind, const UBehaviorTreeComponent& OwnerComp)
{
	const ACSKit_AIController* AIController = Cast<ACSKit_AIController>( OwnerComp.GetAIOwner() );
	if (AIController == nullptr)
	{
		return FVector::ZeroVector;
	}
	const APawn* OwnerPawn = AIController->GetPawn();
	if (OwnerPawn == nullptr)
	{
		return FVector::ZeroVector;
	}

	switch (InKind)
	{
	case ECSKit_BTDecoratorDistanceTargetKind::Owner:
		return OwnerPawn->GetActorLocation();
	case ECSKit_BTDecoratorDistanceTargetKind::HomePos:
		return AIController->GetHomePos();
	case ECSKit_BTDecoratorDistanceTargetKind::NoticeTarget:
		if(const AActor* Target = AIController->GetNoticeTarget())
		{
			return Target->GetActorLocation();
		}
		break;
	default:
		break;
	}

	return FVector::ZeroVector;
}