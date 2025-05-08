// Copyright 2020 megasuraman.
/**
 * @file CSKit_BrainQueryTest_TargetInsideTerritory.cpp
 * @brief 対象がTerritory内かどうかテスト
 * @author megasuraman
 * @date 2025/05/05
 */
#include "AI/BrainQuery/Test/CSKit_BrainQueryTest_TargetInsideTerritory.h"

#include "AI/CSKit_AIController.h"
#include "AI/BrainQuery/CSKit_BrainQueryDataTable.h"
#include "AI/Recognition/CSKit_RecognitionComponent.h"

float UCSKit_BrainQueryTest_TargetInsideTerritory::CalcScore(const ACSKit_AIController& InOwner, const FCSKit_BrainQueryTestNode& InTestNode) const
{
	const UCSKit_RecognitionComponent* RecognitionComponent = InOwner.GetCSKitRecognition();
	if (RecognitionComponent == nullptr)
	{
		return 0.f;
	}
	const FCSKitRecognitionTarget* RecognitionTarget = RecognitionComponent->FindRecognitionTarget(InOwner.GetNoticeTarget());
	if (RecognitionTarget == nullptr)
	{
		return 0.f;
	}
	const float TouchTerritorySec = RecognitionTarget->mTouchTerritoryGauge;
	
	if (InTestNode.mCalcScoreRangeValueMax > InTestNode.mCalcScoreRangeValueMin
		&& InTestNode.mCalcScoreRangeValueMax > 0.f)
	{
		return CheckScoreRangeValue(TouchTerritorySec, InTestNode);
	}
	
	if(TouchTerritorySec <= 0.f)
	{
		return 0.f;
	}
	return 1.f;
}
