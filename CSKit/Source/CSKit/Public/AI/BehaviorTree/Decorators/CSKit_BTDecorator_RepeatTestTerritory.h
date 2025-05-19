// Copyright 2020 megasuraman.
/**
 * @file CSKit_BTDecorator_RepeatTestTerritory.cpp
 * @brief BTDecorator Territory判定
 * @author megasuraman
 * @date 2025/05/19
 */
#pragma once

#include "CoreMinimal.h"
#include "AI/BehaviorTree/Decorators/CSKit_BTDecorator_RepeatTestBase.h"
#include "CSKit_BTDecorator_RepeatTestTerritory.generated.h"

UENUM(BlueprintType)
enum class ECSKit_BTDecoratorTerritoryJudge : uint8
{
	Invalid,
	Owner,
	NoticeTarget,
	BBKey,
};
UCLASS(DisplayName = "CSKit Repeat Test Territory")
class CSKIT_API UCSKit_BTDecorator_RepeatTestTerritory : public UCSKit_BTDecorator_RepeatTestBase
{
	GENERATED_BODY()
public:
	UCSKit_BTDecorator_RepeatTestTerritory(const FObjectInitializer& ObjectInitializer);

	/** initialize any asset related data */
	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;

protected:
	UPROPERTY(EditAnywhere, Category = "Condition", meta = (DisplayName = "指定対象", EditCondition = "mJudge == ECSKit_BTDecoratorTerritoryJudge::BBKey", DisplayPriority = 3))
	FBlackboardKeySelector mBBKey_Target;
	UPROPERTY(EditAnywhere, Category = "Condition", meta = (DisplayName = "指定半径", DisplayPriority = 2))
	float mJudgeRadius = 0.f;
	UPROPERTY(EditAnywhere, Category = "Condition", meta = (DisplayName = "判定", DisplayPriority = 1))
	ECSKit_BTDecoratorTerritoryJudge mJudge = ECSKit_BTDecoratorTerritoryJudge::Invalid;

	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};
