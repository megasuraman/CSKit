// Copyright 2020 megasuraman.
/**
 * @file CSKit_BTDecorator_RepeatTestDistance.cpp
 * @brief BTDecorator 距離判定
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
#include "CSKit_BTDecorator_RepeatTestDistance.generated.h"

UENUM(BlueprintType)
enum class ECSKit_BTDecoratorDistanceTargetKind : uint8
{
	Invalid,
	Owner,
	HomePos,
	NoticeTarget,
	Blackboard,
};

UCLASS(DisplayName = "CSKit Repeat Test Distance")
class CSKIT_API UCSKit_BTDecorator_RepeatTestDistance : public UCSKit_BTDecorator_RepeatTestBase
{
	GENERATED_BODY()
public:
	UCSKit_BTDecorator_RepeatTestDistance(const FObjectInitializer& ObjectInitializer);

	/** initialize any asset related data */
	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
	static FVector GetTargetPos(const ECSKit_BTDecoratorDistanceTargetKind InKind, const UBehaviorTreeComponent& OwnerComp, const FBlackboardKeySelector& InBBKey);

protected:
	UPROPERTY(VisibleAnywhere, Category = "Condition", meta = (DisplayName = "上書き判定距離", DisplayPriority = 2, EditCondition = "mbOverrideBorderDistance"))
	FBlackboardKeySelector mBBKey_OverrideBorderDistance;
	UPROPERTY(VisibleAnywhere, Category = "Condition", meta = (DisplayName = "判定対象A", DisplayPriority = 2, EditCondition = "mTargetA == ECSKit_BTDecoratorDistanceTargetKind::Blackboard"))
	FBlackboardKeySelector mBBKey_TargetA;
	UPROPERTY(VisibleAnywhere, Category = "Condition", meta = (DisplayName = "判定対象B", DisplayPriority = 2, EditCondition = "mTargetB == ECSKit_BTDecoratorDistanceTargetKind::Blackboard"))
	FBlackboardKeySelector mBBKey_TargetB;
	UPROPERTY(EditAnywhere, Category = "Condition", meta = (DisplayName = "判定距離", DisplayPriority = 2))
	float mBorderDistance = 100.f;
	UPROPERTY(EditAnywhere, Category = "Condition", meta = (DisplayName = "判定距離加算", DisplayPriority = 2))
	float mAddDistance = 0.f;
	UPROPERTY(EditAnywhere, Category = "Condition", meta = (DisplayName = "判定", DisplayPriority = 3))
	TEnumAsByte<EArithmeticKeyOperation::Type> mTextOperation;
	UPROPERTY(EditAnywhere, Category = "Condition", meta = (DisplayName = "判定対象A", DisplayPriority = 1))
	ECSKit_BTDecoratorDistanceTargetKind mTargetA;
	UPROPERTY(EditAnywhere, Category = "Condition", meta = (DisplayName = "判定対象B", DisplayPriority = 1))
	ECSKit_BTDecoratorDistanceTargetKind mTargetB;
	UPROPERTY(EditAnywhere, Category = "Condition", meta = (DisplayName = "XY平面で判定", DisplayPriority = 4))
	uint8	mbJudgeXY : 1;
	UPROPERTY(EditAnywhere, Category = "Condition", meta = (DisplayName = "高さのみ判定", DisplayPriority = 4))
	uint8	mbJudgeZ : 1;
	UPROPERTY(EditAnywhere, Category = "Condition", meta = (InlineEditConditionToggle))
	uint8	mbOverrideBorderDistance : 1;

};
