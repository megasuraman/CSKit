// Copyright 2020 megasuraman.
/**
 * @file CSKit_BTDecorator_RepeatTestAIAbility.cpp
 * @brief BTDecorator AIAbility判定
 * @author megasuraman
 * @date 2025/05/19
 */
#pragma once

#include "CoreMinimal.h"
#include "AI/BehaviorTree/Decorators/CSKit_BTDecorator_RepeatTestBase.h"
#include "CSKit_BTDecorator_RepeatTestAIAbility.generated.h"

UCLASS(DisplayName = "CSKit Repeat Test AIAbility")
class CSKIT_API UCSKit_BTDecorator_RepeatTestAIAbility : public UCSKit_BTDecorator_RepeatTestBase
{
	GENERATED_BODY()
public:
	UCSKit_BTDecorator_RepeatTestAIAbility(const FObjectInitializer& ObjectInitializer);

	/** initialize any asset related data */
	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;

protected:
	UPROPERTY(EditAnywhere, Category = "Condition")
	FName mAIAbility;

	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};
