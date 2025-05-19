// Copyright 2020 megasuraman.
/**
 * @file CSKit_BTDecorator_RepeatTestExperience.h
 * @brief BTDecorator 見失い判定
 * @author megasuraman
 * @date 2025/05/19
 */
#pragma once

#include "CoreMinimal.h"
#include "AI/BehaviorTree/Decorators/CSKit_BTDecorator_RepeatTestBase.h"
#include "CSKit_BTDecorator_RepeatTestLostTargetHide.generated.h"

UCLASS(DisplayName = "CSKit Repeat Test LostTargetHide")
class CSKIT_API UCSKit_BTDecorator_RepeatTestLostTargetHide : public UCSKit_BTDecorator_RepeatTestBase
{
	GENERATED_BODY()
public:
	UCSKit_BTDecorator_RepeatTestLostTargetHide(const FObjectInitializer& ObjectInitializer);

	/** initialize any asset related data */
	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;

protected:
	UPROPERTY(EditAnywhere, Category = "Condition", DisplayName = "判定対象")
	FBlackboardKeySelector mBBKey_Target;

	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};
