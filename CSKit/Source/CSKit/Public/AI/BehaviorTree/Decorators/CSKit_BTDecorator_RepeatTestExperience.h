// Copyright 2020 megasuraman.
/**
 * @file CSKit_BTDecorator_RepeatTestExperience.h
 * @brief BTDecorator Experience判定
 * @author megasuraman
 * @date 2025/05/19
 */
#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#if ENGINE_MAJOR_VERSION == 5
#include "BehaviorTree/Blackboard/BlackboardKeyEnums.h"
#else
#include "BehaviorTree/Blackboard/BlackboardKeyType.h"
#endif
#include "AI/BehaviorTree/Decorators/CSKit_BTDecorator_RepeatTestBase.h"
#include "CSKit_BTDecorator_RepeatTestExperience.generated.h"

UCLASS(DisplayName = "CSKit Repeat Test Experience")
class CSKIT_API UCSKit_BTDecorator_RepeatTestExperience : public UCSKit_BTDecorator_RepeatTestBase
{
	GENERATED_BODY()
public:
	UCSKit_BTDecorator_RepeatTestExperience(const FObjectInitializer& ObjectInitializer);

	/** initialize any asset related data */
	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

protected:
	UPROPERTY(EditAnywhere, Category = "Condition", meta = (DisplayName = "判定Score", ClampMin=0.0, ClampMax=1.0, UIMin=0.0, UIMax=1.0, DisplayPriority = 2))
	float mBorderScore = 0.f;
	UPROPERTY(EditAnywhere, Category = "Condition", meta = (DisplayName = "ExperienceTypeId名", DisplayPriority = 2))
	FName mExperienceTypeName;
	UPROPERTY(EditAnywhere, Category = "Condition", meta = (DisplayName = "判定", DisplayPriority = 3))
	TEnumAsByte<EArithmeticKeyOperation::Type> mTextOperation;
};
