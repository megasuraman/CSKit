// Copyright 2020 megasuraman.
/**
 * @file CSKit_BrainQueryTest_Random.cpp
 * @brief 乱数テスト
 * @author megasuraman
 * @date 2025/05/05
 */
#include "AI/BrainQuery/Test/CSKit_BrainQueryTest_Random.h"

#include "CSKit_Subsystem.h"
#include "AI/BrainQuery/CSKit_BrainQueryDataTable.h"

float UCSKit_BrainQueryTest_Random::CalcScore(const ACSKit_AIController& InOwner, const FCSKit_BrainQueryTestNode& InTestNode) const
{
	if(InTestNode.mCalcScoreRangeValueMin > InTestNode.mCalcScoreRangeValueMax)
	{
		return 0.f;
	}
	else if(InTestNode.mCalcScoreRangeValueMin == InTestNode.mCalcScoreRangeValueMax)
	{
		return InTestNode.mCalcScoreRangeValueMin;
	}
	return UCSKit_Subsystem::msRand.FRandRange(InTestNode.mCalcScoreRangeValueMin, InTestNode.mCalcScoreRangeValueMax);
}