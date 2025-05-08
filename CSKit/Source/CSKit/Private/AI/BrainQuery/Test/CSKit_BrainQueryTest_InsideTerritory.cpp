// Copyright 2020 megasuraman.
/**
 * @file CSKit_BrainQueryTest_InsideTerritory.cpp
 * @brief 自分がTerritory内かどうかテスト
 * @author megasuraman
 * @date 2025/05/05
 */
#include "AI/BrainQuery/Test/CSKit_BrainQueryTest_InsideTerritory.h"

#include "AI/CSKit_AIController.h"
#include "AI/Territory/CSKit_TerritoryComponent.h"

float UCSKit_BrainQueryTest_InsideTerritory::CalcScore(const ACSKit_AIController& InOwner, const FCSKit_BrainQueryTestNode& InTestNode) const
{
	const APawn* OwnerPawn = InOwner.GetPawn();
	const UCSKit_NoticeComponent* NoticeComponent = InOwner.GetCSKitNotice();
	if (OwnerPawn == nullptr
		|| NoticeComponent == nullptr)
	{
		return 0.f;
	}
	if(const UCSKit_TerritoryComponent* TerritoryComponent = InOwner.GetCSKitTerritoryComponent())
	{
		if(TerritoryComponent->IsOwnTerritoryVolume()
			&& !TerritoryComponent->IsTouchTerritoryVolume(OwnerPawn->GetActorLocation(),0.f))
		{
			return 0.f;
		}
	}
	return 1.f;
}
