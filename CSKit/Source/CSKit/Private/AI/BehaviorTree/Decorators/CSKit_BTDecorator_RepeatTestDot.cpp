// Copyright 2020 megasuraman.
/**
 * @file CSKit_BTDecorator_RepeatTestDot.cpp
 * @brief BTDecorator 内積判定
 * @author megasuraman
 * @date 2025/05/19
 */
#include "AI/BehaviorTree/Decorators/CSKit_BTDecorator_RepeatTestDot.h"

#include "AI/CSKit_AIController.h"
#include "BehaviorTree/BTCompositeNode.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Float.h"

UCSKit_BTDecorator_RepeatTestDot::UCSKit_BTDecorator_RepeatTestDot(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, mbJudgeXY(false)
	, mbOverrideBorderDot(false)
{
	NodeName = "CSKitRepeatTest_Dot";

	mTextOperation = EArithmeticKeyOperation::Less;
	
	mBBKey_OverrideBorderDot.AddFloatFilter(this, GET_MEMBER_NAME_CHECKED(UCSKit_BTDecorator_RepeatTestDot, mBBKey_OverrideBorderDot));
}

/* ------------------------------------------------------------
   !FBlackboardKeySelectorの初期化のために
------------------------------------------------------------ */
void UCSKit_BTDecorator_RepeatTestDot::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);

	if (const UBlackboardData* BBAsset = GetBlackboardAsset())
	{
		mBBKey_OverrideBorderDot.ResolveSelectedKey(*BBAsset);
	}
	else
	{
		UE_LOG(LogBehaviorTree, Warning, TEXT("Can't initialize %s due to missing blackboard data."), *GetName());
		mBBKey_OverrideBorderDot.InvalidateResolvedKey();
	}
}

/* ------------------------------------------------------------
   !条件判定
------------------------------------------------------------ */
bool UCSKit_BTDecorator_RepeatTestDot::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	const FVector VectorA = GetTargetNV(mTargetA, OwnerComp);
	const FVector VectorB = GetTargetNV(mTargetB, OwnerComp);
	float BorderDot = mBorderDot;
	if (mbOverrideBorderDot
		&& mBBKey_OverrideBorderDot.SelectedKeyType == UBlackboardKeyType_Float::StaticClass())
	{
		const UBlackboardComponent* MyBlackboard = OwnerComp.GetBlackboardComponent();
		BorderDot = MyBlackboard->GetValue<UBlackboardKeyType_Float>(mBBKey_OverrideBorderDot.GetSelectedKeyID());
	}
	const float Dot = FVector::DotProduct(VectorA, VectorB);
	switch (mTextOperation.GetValue())
	{
	case EArithmeticKeyOperation::Type::Equal:
		return (Dot == BorderDot);
	case EArithmeticKeyOperation::Type::NotEqual:
		return (Dot != BorderDot);
	case EArithmeticKeyOperation::Type::Less:
		return (Dot < BorderDot);
	case EArithmeticKeyOperation::Type::LessOrEqual:
		return (Dot <= BorderDot);
	case EArithmeticKeyOperation::Type::Greater:
		return (Dot > BorderDot);
	case EArithmeticKeyOperation::Type::GreaterOrEqual:
		return (Dot >= BorderDot);
	default:
		break;
	}

	return false;
}

FVector UCSKit_BTDecorator_RepeatTestDot::GetTargetNV(const ECSKit_BTDecoratorDotTargetKind InKind, const UBehaviorTreeComponent& OwnerComp) const
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

	FVector TargetV = FVector::ForwardVector;
	switch (InKind)
	{
	case ECSKit_BTDecoratorDotTargetKind::OwnerFront:
		TargetV = OwnerPawn->GetActorForwardVector();
		break;
	case ECSKit_BTDecoratorDotTargetKind::OwnerRight:
		TargetV = OwnerPawn->GetActorRightVector();
		break;
	case ECSKit_BTDecoratorDotTargetKind::Owner2HomePos:
		TargetV = AIController->GetHomePos() - OwnerPawn->GetActorLocation();
		break;
	case ECSKit_BTDecoratorDotTargetKind::Owner2NoticeTarget:
		if(const AActor* Target = AIController->GetNoticeTarget())
		{
			TargetV = Target->GetActorLocation() - OwnerPawn->GetActorLocation();
		}
		break;
	default:
		break;
	}
	
	if (mbJudgeXY)
	{
		TargetV.Z = 0.f;
	}

	return TargetV.GetSafeNormal();
}