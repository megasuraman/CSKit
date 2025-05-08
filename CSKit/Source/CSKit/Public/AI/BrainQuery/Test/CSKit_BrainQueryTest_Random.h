// Copyright 2020 megasuraman.
/**
 * @file CSKit_BrainQueryTest_Random.h
 * @brief 乱数テスト
 * @author megasuraman
 * @date 2025/05/05
 */
#pragma once

#include "CoreMinimal.h"
#include "AI/BrainQuery/Test/CSKit_BrainQueryTest_Base.h"
#include "CSKit_BrainQueryTest_Random.generated.h"

UCLASS()
class CSKIT_API UCSKit_BrainQueryTest_Random : public UCSKit_BrainQueryTest_Base
{
	GENERATED_BODY()

public:
	virtual	float	CalcScore(const ACSKit_AIController& InOwner, const FCSKit_BrainQueryTestNode& InTestNode) const override;
};
