// Copyright 2020 megasuraman.
/**
 * @file CSKit_BrainQueryTest_OwnCommunity.h
 * @brief Communityに属してるかどうかテスト
 * @author megasuraman
 * @date 2025/05/05
 */
#pragma once

#include "CoreMinimal.h"
#include "AI/BrainQuery/Test/CSKit_BrainQueryTest_Base.h"
#include "CSKit_BrainQueryTest_OwnCommunity.generated.h"

UCLASS()
class CSKIT_API UCSKit_BrainQueryTest_OwnCommunity : public UCSKit_BrainQueryTest_Base
{
	GENERATED_BODY()

public:
	virtual	float	CalcScore(const ACSKit_AIController& InOwner, const FCSKit_BrainQueryTestNode& InTestNode) const override;
};
