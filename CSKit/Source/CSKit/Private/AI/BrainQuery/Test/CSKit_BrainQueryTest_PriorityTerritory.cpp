// Copyright 2020 megasuraman.
/**
 * @file CSKit_BrainQueryTest_PriorityTerritory.cpp
 * @brief 対象との位置関係を加味したTerritory優先度テスト
 * @author megasuraman
 * @date 2025/05/05
 */
#include "AI/BrainQuery/Test/CSKit_BrainQueryTest_PriorityTerritory.h"

#include "AI/CSKit_AIController.h"
#include "AI/Recognition/CSKit_RecognitionComponent.h"
#include "AI/Territory/CSKit_TerritoryComponent.h"

float UCSKit_BrainQueryTest_PriorityTerritory::CalcScore(const ACSKit_AIController& InOwner, const FCSKit_BrainQueryTestNode& InTestNode) const
{
	const UCSKit_TerritoryComponent* TerritoryComponent = InOwner.GetCSKitTerritoryComponent();
	const UCSKit_RecognitionComponent* RecognitionComponent = InOwner.GetCSKitRecognition();
	if (TerritoryComponent == nullptr
		|| RecognitionComponent == nullptr)
	{
		return 0.f;
	}
	const APawn* OwnerPawn = InOwner.GetPawn(); 
	const AActor* NoticeTarget = InOwner.GetNoticeTarget();
	if( OwnerPawn == nullptr
		|| NoticeTarget == nullptr
		|| !TerritoryComponent->IsOwnTerritoryVolume())
	{
		return 0.f;
	}
	
	const FCSKitRecognitionTarget* RecognitionTarget = RecognitionComponent->FindRecognitionTarget(NoticeTarget);
	if (RecognitionTarget == nullptr)
	{
		return 0.f;
	}
	const float TargetTouchTerritoryGauge = RecognitionTarget->mTouchTerritoryGauge;
	//対象が完全にテリトリー内なら意識不要
	if(TargetTouchTerritoryGauge >= 1.f)
	{
		return 0.f;
	}

	const FVector HomePos = InOwner.GetHomePos();
	const FVector OwnerPos = OwnerPawn->GetActorLocation();
	const FVector TargetPos = NoticeTarget->GetActorLocation();
	const FVector Owner2Home_XY_NV = FVector(HomePos-OwnerPos).GetSafeNormal2D();
	const FVector Owner2Target_XY_NV = FVector(TargetPos-OwnerPos).GetSafeNormal2D();
	const float OwnerNearHomeScore = FMath::Clamp(1.f - (FVector::DotProduct(Owner2Home_XY_NV,Owner2Target_XY_NV) + 1.f) * 0.5f, 0.f, 1.f);
	
	float Score = OwnerNearHomeScore;
	//自分がTerritory内の場合は、対象がTerritory内かどうかを加味
	if(TerritoryComponent->IsInside())
	{
		constexpr float TerritoryGaugeRatio = 0.5f;
		constexpr float OwnerNearHomeScoreRatio = 0.5f;
		Score = (1.f-TargetTouchTerritoryGauge)*TerritoryGaugeRatio + OwnerNearHomeScore*OwnerNearHomeScoreRatio;
		Score = CheckScoreRangeValue(Score, InTestNode);
	}
	
	return Score;
}
