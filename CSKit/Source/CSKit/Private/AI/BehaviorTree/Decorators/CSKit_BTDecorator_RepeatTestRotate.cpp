// Copyright 2020 megasuraman.
/**
 * @file CSKit_BTDecorator_RepeatTestRotate.cpp
 * @brief BTDecorator 向き判定
 * @author megasuraman
 * @date 2025/05/19
 */
#include "AI/BehaviorTree/Decorators/CSKit_BTDecorator_RepeatTestRotate.h"

#include "AI/CSKit_AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BTCompositeNode.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Float.h"

UCSKit_BTDecorator_RepeatTestRotate::UCSKit_BTDecorator_RepeatTestRotate(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, mbJudgeYaw(true)
	, mbOverrideBorderDiffAngle(false)
{
	NodeName = "CSKitRepeatTest_Rotate";
	
	mTextOperation = EArithmeticKeyOperation::Less;
	
	mBBKey_OverrideBorderDiffAngle.AddFloatFilter(this, GET_MEMBER_NAME_CHECKED(UCSKit_BTDecorator_RepeatTestRotate, mBBKey_OverrideBorderDiffAngle));
}

/**
 * @brief 
 */
void UCSKit_BTDecorator_RepeatTestRotate::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);
	
	if (const UBlackboardData* BBAsset = GetBlackboardAsset())
	{
		mBBKey_OverrideBorderDiffAngle.ResolveSelectedKey(*BBAsset);
	}
	else
	{
		UE_LOG(LogBehaviorTree, Warning, TEXT("Can't initialize %s due to missing blackboard data."), *GetName());
		mBBKey_OverrideBorderDiffAngle.InvalidateResolvedKey();
	}
}

/* ------------------------------------------------------------
   !条件判定
------------------------------------------------------------ */
bool UCSKit_BTDecorator_RepeatTestRotate::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	const FRotator RotA = GetTargetRotator(mTargetA, OwnerComp);
	const FRotator RotB = GetTargetRotator(mTargetB, OwnerComp);
	float BorderDiffAngle = mBorderDiffAngle + mAddDiffAngle;
	if (mbOverrideBorderDiffAngle
		&& mBBKey_OverrideBorderDiffAngle.SelectedKeyType == UBlackboardKeyType_Float::StaticClass())
	{
		const UBlackboardComponent* MyBlackboard = OwnerComp.GetBlackboardComponent();
		BorderDiffAngle = MyBlackboard->GetValue<UBlackboardKeyType_Float>(mBBKey_OverrideBorderDiffAngle.GetSelectedKeyID()) + mAddDiffAngle;
	}

	const FTransform BaseTransform(RotA.Quaternion());
	const FRotator DiffRot = BaseTransform.InverseTransformRotation(RotB.Quaternion()).Rotator();
	float DiffAngle = 0.f;
	if (mbJudgeYaw)
	{
		DiffAngle = FMath::Abs(DiffRot.Yaw);
	}

	switch (mTextOperation.GetValue())
	{
	case EArithmeticKeyOperation::Type::Equal:
		return (DiffAngle == BorderDiffAngle);
	case EArithmeticKeyOperation::Type::NotEqual:
		return (DiffAngle != BorderDiffAngle);
	case EArithmeticKeyOperation::Type::Less:
		return (DiffAngle < BorderDiffAngle);
	case EArithmeticKeyOperation::Type::LessOrEqual:
		return (DiffAngle <= BorderDiffAngle);
	case EArithmeticKeyOperation::Type::Greater:
		return (DiffAngle > BorderDiffAngle);
	case EArithmeticKeyOperation::Type::GreaterOrEqual:
		return (DiffAngle >= BorderDiffAngle);
	default:
		break;
	}

	return false;
}

FRotator UCSKit_BTDecorator_RepeatTestRotate::GetTargetRotator(const ECSKit_BTDecoratorRotateTargetKind InKind, const UBehaviorTreeComponent& OwnerComp) const
{
	const ACSKit_AIController* AIController = Cast<ACSKit_AIController>( OwnerComp.GetAIOwner() );
	if (AIController == nullptr)
	{
		return FRotator::ZeroRotator;
	}
	const APawn* OwnerPawn = AIController->GetPawn();
	if (OwnerPawn == nullptr)
	{
		return FRotator::ZeroRotator;
	}

	switch (InKind)
	{
	case ECSKit_BTDecoratorRotateTargetKind::Owner:
		return OwnerPawn->GetActorRotation();
	case ECSKit_BTDecoratorRotateTargetKind::HomeRot:
		return AIController->GetHomeRot();
	case ECSKit_BTDecoratorRotateTargetKind::NoticeTarget:
		if(const AActor* Target = AIController->GetNoticeTarget())
		{
			return FVector(Target->GetActorLocation()-OwnerPawn->GetActorLocation()).Rotation();
		}
		break;
	default:
		break;
	}

	return FRotator::ZeroRotator;
}
