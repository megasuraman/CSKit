// Copyright 2020 megasuraman.
/**
 * @file CSKit_BrainQueryTest_PriorityTerritory.h
 * @brief 対象との位置関係を加味したTerritory優先度テスト
 * @author megasuraman
 * @date 2025/05/05
 */
#pragma once

#include "CoreMinimal.h"
#include "AI/BrainQuery/Test/CSKit_BrainQueryTest_Base.h"
#include "CSKit_BrainQueryTest_PriorityTerritory.generated.h"

UCLASS()
class CSKIT_API UCSKit_BrainQueryTest_PriorityTerritory : public UCSKit_BrainQueryTest_Base
{
	GENERATED_BODY()

public:
	virtual	float	CalcScore(const ACSKit_AIController& InOwner, const FCSKit_BrainQueryTestNode& InTestNode) const override;
};
