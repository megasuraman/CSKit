// Copyright 2020 megasuraman.
/**
 * @file CSKit_BrainQueryTest_OwnCommunity.cpp
 * @brief Communityに属してるかどうかテスト
 * @author megasuraman
 * @date 2025/05/05
 */
#include "AI/BrainQuery/Test/CSKit_BrainQueryTest_OwnCommunity.h"

#include "AI/CSKit_AIController.h"
#include "AI/Community/CSKit_CommunityComponent.h"

float UCSKit_BrainQueryTest_OwnCommunity::CalcScore(const ACSKit_AIController& InOwner, const FCSKit_BrainQueryTestNode& InTestNode) const
{
	const UCSKit_CommunityComponent* CommunityComponent = InOwner.GetCSKitCommunity();
	if (CommunityComponent == nullptr)
	{
		return 0.f;
	}
	if(!CommunityComponent->IsOwnCommunity())
	{
		return 0.f;
	}
	return 1.f;
}
