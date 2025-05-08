// Copyright 2020 megasuraman.
/**
 * @file CSKit_BrainQueryTest_TargetFront.h
 * @brief BrainQueryの対象正面度合いテスト
 * @author megasuraman
 * @date 2025/05/05
 */
#pragma once

#include "CoreMinimal.h"
#include "AI/BrainQuery/Test/CSKit_BrainQueryTest_Base.h"
#include "CSKit_BrainQueryTest_TargetFront.generated.h"

UCLASS()
class CSKIT_API UCSKit_BrainQueryTest_TargetFront : public UCSKit_BrainQueryTest_Base
{
	GENERATED_BODY()

public:
	virtual	float	CalcScore(const ACSKit_AIController& InOwner, const FCSKit_BrainQueryTestNode& InTestNode) const override;
};
