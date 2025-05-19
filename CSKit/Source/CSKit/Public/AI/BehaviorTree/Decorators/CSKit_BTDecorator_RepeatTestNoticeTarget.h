// Copyright 2020 megasuraman.
/**
 * @file CSKit_BTDecorator_RepeatTestNoticeTarget.cpp
 * @brief BTDecorator Notice判定
 * @author megasuraman
 * @date 2025/05/19
 */
#pragma once

#include "CoreMinimal.h"
#include "AI/BehaviorTree/Decorators/CSKit_BTDecorator_RepeatTestBase.h"
#include "CSKit_BTDecorator_RepeatTestNoticeTarget.generated.h"

UCLASS(DisplayName = "CSKit Repeat Test NoticeTarget")
class CSKIT_API UCSKit_BTDecorator_RepeatTestNoticeTarget : public UCSKit_BTDecorator_RepeatTestBase
{
	GENERATED_BODY()
public:
	UCSKit_BTDecorator_RepeatTestNoticeTarget(const FObjectInitializer& ObjectInitializer);

protected:
	UPROPERTY(EditAnywhere, Category = "Condition", meta = (DisplayName = "Community含む", DisplayPriority = 1))
	bool mbCheckCommunity = false;

	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};
