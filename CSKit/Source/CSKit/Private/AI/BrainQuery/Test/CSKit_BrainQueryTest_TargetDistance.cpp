// Copyright 2020 megasuraman.
/**
 * @file CSKit_BrainQueryTest_TargetDistance.cpp
 * @brief BrainQueryの対象距離テスト
 * @author megasuraman
 * @date 2025/05/05
 */
#include "AI/BrainQuery/Test/CSKit_BrainQueryTest_TargetDistance.h"

#include "AI/CSKit_AIController.h"
#include "AI/BrainQuery/CSKit_BrainQueryDataTable.h"

float UCSKit_BrainQueryTest_TargetDistance::CalcScore(const ACSKit_AIController& InOwner, const FCSKit_BrainQueryTestNode& InTestNode) const
{
	const AActor* OwnerPawn = InOwner.GetPawn();
	const AActor* TargetActor = InOwner.GetNoticeTarget();
	if (OwnerPawn == nullptr
		|| TargetActor == nullptr
		|| InTestNode.mCalcScoreRangeValueMin > InTestNode.mCalcScoreRangeValueMax
		|| InTestNode.mCalcScoreRangeValueMax <= 0.f)
	{
		return 0.f;
	}
	const float Distance = FVector::Distance(OwnerPawn->GetActorLocation(), TargetActor->GetActorLocation());
	float Score = (Distance - InTestNode.mCalcScoreRangeValueMin) / (InTestNode.mCalcScoreRangeValueMax - InTestNode.mCalcScoreRangeValueMin);
	Score = FMath::Clamp(Score, 0.f, 1.f);
	return Score;
}