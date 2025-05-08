// Copyright 2020 megasuraman.
/**
 * @file CSKit_BrainQueryTest_Base.cpp
 * @brief BrainQueryで行う各種テストの基礎クラス
 * @author megasuraman
 * @date 2025/05/05
 */ 
#include "AI/BrainQuery/Test/CSKit_BrainQueryTest_Base.h"

#include "AI/BrainQuery/CSKit_BrainQueryDataTable.h"

float UCSKit_BrainQueryTest_Base::CheckScoreRangeValue(const float InScore, const FCSKit_BrainQueryTestNode& InTestNode)
{
	float Score = InScore;
	if (InTestNode.mCalcScoreRangeValueMax > InTestNode.mCalcScoreRangeValueMin
		&& InTestNode.mCalcScoreRangeValueMax > 0.f)
	{
		Score = (Score - InTestNode.mCalcScoreRangeValueMin) / (InTestNode.mCalcScoreRangeValueMax - InTestNode.mCalcScoreRangeValueMin);
	}
	Score = FMath::Clamp(Score, 0.f, 1.f);
	return Score;
}