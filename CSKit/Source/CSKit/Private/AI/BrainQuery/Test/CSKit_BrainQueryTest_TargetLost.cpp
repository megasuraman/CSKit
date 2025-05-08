// Copyright 2020 megasuraman.
/**
 * @file CSKit_BrainQueryTest_TargetLost.cpp
 * @brief BrainQueryの対象見失いテスト
 * @author megasuraman
 * @date 2025/05/05
 */
#include "AI/BrainQuery/Test/CSKit_BrainQueryTest_TargetLost.h"

#include "AI/CSKit_AIController.h"
#include "AI/Notice/CSKit_NoticeComponent.h"
#include "AI/Recognition/CSKit_RecognitionComponent.h"

float UCSKit_BrainQueryTest_TargetLost::CalcScore(const ACSKit_AIController& InOwner, const FCSKit_BrainQueryTestNode& InTestNode) const
{
	const UCSKit_NoticeComponent* NoticeComponent = InOwner.GetCSKitNotice();
	const UCSKit_RecognitionComponent* RecognitionComponent = InOwner.GetCSKitRecognition();
	if (NoticeComponent == nullptr
		|| RecognitionComponent == nullptr)
	{
		return 0.f;
	}
	const AActor* SelectTarget = NoticeComponent->GetSelectTarget();
	if (SelectTarget == nullptr)
	{
		return 1.f;
	}
	const FCSKitRecognitionTarget* RecognitionTarget = RecognitionComponent->FindRecognitionTarget(SelectTarget);
	if (RecognitionTarget == nullptr)
	{
		return 0.f;
	}
	const float LostBorderTime = RecognitionComponent->GetCompletelyLostBorderTime();
	if (LostBorderTime > 0.f)
	{
		return FMath::Clamp((RecognitionTarget->mLostSec / LostBorderTime), 0.f, 1.f);
	}
	return 1.f;
}

