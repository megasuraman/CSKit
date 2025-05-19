// Copyright 2020 megasuraman.
/**
 * @file CSKit_BTDecorator_RepeatTestDot.cpp
 * @brief BTDecorator 内積判定
 * @author megasuraman
 * @date 2025/05/19
 */
#pragma once

#include "CoreMinimal.h"
#if ENGINE_MAJOR_VERSION == 5
#include "BehaviorTree/Blackboard/BlackboardKeyEnums.h"
#else
#include "BehaviorTree/Blackboard/BlackboardKeyType.h"
#endif
#include "AI/BehaviorTree/Decorators/CSKit_BTDecorator_RepeatTestBase.h"
#include "CSKit_BTDecorator_RepeatTestDot.generated.h"

UENUM(BlueprintType)
enum class ECSKit_BTDecoratorDotTargetKind : uint8
{
	Invalid,
	OwnerFront,
	OwnerRight,
	Owner2HomePos,
	Owner2NoticeTarget,
};

UCLASS(DisplayName = "CSKit Repeat Test Dot")
class CSKIT_API UCSKit_BTDecorator_RepeatTestDot : public UCSKit_BTDecorator_RepeatTestBase
{
	GENERATED_BODY()
public:
	UCSKit_BTDecorator_RepeatTestDot(const FObjectInitializer& ObjectInitializer);

	/** initialize any asset related data */
	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
	FVector GetTargetNV(const ECSKit_BTDecoratorDotTargetKind InKind, const UBehaviorTreeComponent& OwnerComp) const;

protected:
	UPROPERTY(VisibleAnywhere, Category = "Condition", meta = (DisplayName = "上書き判定内積値", DisplayPriority = 2, EditCondition = "mbOverrideBorderDot"))
	FBlackboardKeySelector mBBKey_OverrideBorderDot;
	UPROPERTY(EditAnywhere, Category = "Condition", meta = (DisplayName = "判定内積値", DisplayPriority = 2))
	float mBorderDot = 0.f;
	UPROPERTY(EditAnywhere, Category = "Condition", meta = (DisplayName = "判定", DisplayPriority = 3))
	TEnumAsByte<EArithmeticKeyOperation::Type> mTextOperation;
	UPROPERTY(EditAnywhere, Category = "Condition", meta = (DisplayName = "判定対象A", DisplayPriority = 1))
	ECSKit_BTDecoratorDotTargetKind mTargetA;
	UPROPERTY(EditAnywhere, Category = "Condition", meta = (DisplayName = "判定対象B", DisplayPriority = 1))
	ECSKit_BTDecoratorDotTargetKind mTargetB;
	UPROPERTY(EditAnywhere, Category = "Condition", meta = (DisplayName = "XY平面で判定", DisplayPriority = 4))
	uint8	mbJudgeXY : 1;
	UPROPERTY(EditAnywhere, Category = "Condition", meta = (InlineEditConditionToggle))
	uint8	mbOverrideBorderDot : 1;

};
