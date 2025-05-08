// Copyright 2020 megasuraman.
/**
 * @file CSKit_BrainQueryTest_TargetNotice.cpp
 * @brief BrainQueryの対象持ちテスト
 * @author megasuraman
 * @date 2025/05/05
 */
#include "AI/BrainQuery/Test/CSKit_BrainQueryTest_TargetNotice.h"

#include "AI/CSKit_AIController.h"
#include "AI/Notice/CSKit_NoticeComponent.h"

float UCSKit_BrainQueryTest_TargetNotice::CalcScore(const ACSKit_AIController& InOwner, const FCSKit_BrainQueryTestNode& InTestNode) const
{
	const UCSKit_NoticeComponent* NoticeComponent = InOwner.GetCSKitNotice();
	if (NoticeComponent == nullptr)
	{
		return 0.f;
	}
	if (NoticeComponent->GetSelectTarget() == nullptr)
	{
		return 0.f;
	}
	return 1.f;
}
