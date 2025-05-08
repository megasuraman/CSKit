// Copyright 2020 megasuraman.
/**
 * @file CSKit_BrainQueryTest_AIAbility.cpp
 * @brief BrainQueryの能力保持テスト
 * @author megasuraman
 * @date 2025/05/05
 */
#include "AI/BrainQuery/Test/CSKit_BrainQueryTest_AIAbility.h"

#include "AI/CSKit_AIController.h"
#include "AI/BrainQuery/CSKit_BrainQueryDataTable.h"

float UCSKit_BrainQueryTest_AIAbility::CalcScore(const ACSKit_AIController& InOwner, const FCSKit_BrainQueryTestNode& InTestNode) const
{
	float Score = 0.f;
	if(InOwner.IsOwnAbility(InTestNode.mBrainQueryTestOption_Name))
	{
		Score = 1.f;
	}
	Score = CheckScoreRangeValue(Score, InTestNode);
	return Score;
}