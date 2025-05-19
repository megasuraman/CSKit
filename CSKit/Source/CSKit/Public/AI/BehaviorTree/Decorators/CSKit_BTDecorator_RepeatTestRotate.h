// Copyright 2020 megasuraman.
/**
 * @file CSKit_BTDecorator_RepeatTestRotate.cpp
 * @brief BTDecorator 向き判定
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
#include "CSKit_BTDecorator_RepeatTestRotate.generated.h"

UENUM(BlueprintType)
enum class ECSKit_BTDecoratorRotateTargetKind : uint8
{
	Invalid,
	Owner,
	HomeRot,
	NoticeTarget,
};

UCLASS(DisplayName = "CSKit Repeat Test Rotate")
class CSKIT_API UCSKit_BTDecorator_RepeatTestRotate : public UCSKit_BTDecorator_RepeatTestBase
{
	GENERATED_BODY()
public:
	UCSKit_BTDecorator_RepeatTestRotate(const FObjectInitializer& ObjectInitializer);

	/** initialize any asset related data */
	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
	FRotator GetTargetRotator(const ECSKit_BTDecoratorRotateTargetKind InKind, const UBehaviorTreeComponent& OwnerComp) const;

protected:
	UPROPERTY(VisibleAnywhere, Category = "Condition", meta = (DisplayName = "上書き判定角度差", DisplayPriority = 2, EditCondition = "mbOverrideBorderDiffAngle"))
	FBlackboardKeySelector mBBKey_OverrideBorderDiffAngle;
	UPROPERTY(EditAnywhere, Category = "Condition", meta = (DisplayName = "判定角度差", DisplayPriority = 2))
	float mBorderDiffAngle = 90.f;
	UPROPERTY(EditAnywhere, Category = "Condition", meta = (DisplayName = "判定角度差加算", DisplayPriority = 2))
	float mAddDiffAngle = 0.f;
	UPROPERTY(EditAnywhere, Category = "Condition", meta = (DisplayName = "判定", DisplayPriority = 3))
	TEnumAsByte<EArithmeticKeyOperation::Type> mTextOperation;
	UPROPERTY(EditAnywhere, Category = "Condition", meta = (DisplayName = "判定対象A", DisplayPriority = 1))
	ECSKit_BTDecoratorRotateTargetKind mTargetA;
	UPROPERTY(EditAnywhere, Category = "Condition", meta = (DisplayName = "判定対象B", DisplayPriority = 1))
	ECSKit_BTDecoratorRotateTargetKind mTargetB;
	UPROPERTY(EditAnywhere, Category = "Condition", meta = (DisplayName = "Yaw判定", DisplayPriority = 4))
	uint8	mbJudgeYaw : 1;
	UPROPERTY(EditAnywhere, Category = "Condition", meta = (InlineEditConditionToggle))
	uint8	mbOverrideBorderDiffAngle : 1;
};
