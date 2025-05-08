// Copyright 2020 megasuraman.
/**
 * @file CSKit_BrainQueryTest_TargetRight.cpp
 * @brief BrainQueryの対象の右側度合いテスト
 * @author megasuraman
 * @date 2025/05/05
 */
#include "AI/BrainQuery/Test/CSKit_BrainQueryTest_TargetRight.h"

#include "AI/CSKit_AIController.h"

float UCSKit_BrainQueryTest_TargetRight::CalcScore(const ACSKit_AIController& InOwner, const FCSKit_BrainQueryTestNode& InTestNode) const
{
	const AActor* OwnerPawn = InOwner.GetPawn();
	const AActor* TargetActor = InOwner.GetNoticeTarget();
	if (OwnerPawn == nullptr
		|| TargetActor == nullptr)
	{
		return 0.f;
	}
	const FVector RightNV = OwnerPawn->GetActorRightVector();
	const FVector TargetNV = FVector(TargetActor->GetActorLocation() - OwnerPawn->GetActorLocation()).GetSafeNormal();
	const float RightDot = FVector::DotProduct(RightNV,TargetNV);
	float Score = (RightDot + 1.f)*0.5f;
	Score = CheckScoreRangeValue(Score, InTestNode);
	return Score;
}