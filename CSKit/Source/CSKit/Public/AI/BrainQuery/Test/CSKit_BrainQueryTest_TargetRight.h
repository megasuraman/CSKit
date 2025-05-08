// Copyright 2020 megasuraman.
/**
 * @file CSKit_BrainQueryTest_TargetRight.h
 * @brief BrainQueryの対象の右側度合いテスト
 * @author megasuraman
 * @date 2025/05/05
 */
#pragma once

#include "CoreMinimal.h"
#include "AI/BrainQuery/Test/CSKit_BrainQueryTest_Base.h"
#include "CSKit_BrainQueryTest_TargetRight.generated.h"

UCLASS()
class CSKIT_API UCSKit_BrainQueryTest_TargetRight : public UCSKit_BrainQueryTest_Base
{
	GENERATED_BODY()

public:
	virtual	float	CalcScore(const ACSKit_AIController& InOwner, const FCSKit_BrainQueryTestNode& InTestNode) const override;
};
