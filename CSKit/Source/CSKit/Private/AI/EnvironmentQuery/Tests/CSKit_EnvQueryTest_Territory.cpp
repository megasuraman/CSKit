// Copyright 2020 megasuraman.
/**
 * @file CSKit_EnvQueryTest_Territory.cpp
 * @brief EnvQueryTest Territory内かどうか
 * @author megasuraman
 * @date 2025/06/07
 */
#include "AI/EnvironmentQuery/Tests/CSKit_EnvQueryTest_Territory.h"

#include "AI/CSKit_AIController.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_ActorBase.h"
#include "GameFramework/Character.h"
#include "AI/Territory/CSKit_TerritoryComponent.h"

UCSKit_EnvQueryTest_Territory::UCSKit_EnvQueryTest_Territory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	Cost = EEnvTestCost::Low;
	ValidItemType = UEnvQueryItemType_VectorBase::StaticClass();
}


/**
 * @brief EQSのアイテムをスコアリング
 */
void	UCSKit_EnvQueryTest_Territory::RunTest(FEnvQueryInstance& QueryInstance) const
{
	const ACharacter* OwnerCharacter = Cast<ACharacter>(QueryInstance.Owner.Get());
	if (OwnerCharacter == nullptr)
	{
		return;
	}
	const ACSKit_AIController* AIController = Cast<ACSKit_AIController>(OwnerCharacter->GetController());
	if(AIController == nullptr)
	{
		return;
	}
	
	FloatValueMin.BindData(OwnerCharacter, QueryInstance.QueryID);
	FloatValueMax.BindData(OwnerCharacter, QueryInstance.QueryID);
	const float MinValue = FloatValueMin.GetValue();
	const float MaxValue = FloatValueMax.GetValue();
	for (FEnvQueryInstance::ItemIterator It(this, QueryInstance); It; ++It)
	{
		const FVector ItemLocation = GetItemLocation(QueryInstance, It.GetIndex());
		float Score = 1.f;
		if(const UCSKit_TerritoryComponent* TerritoryComponent = AIController->GetCSKitTerritoryComponent())
		{
			if(TerritoryComponent->IsOwnTerritoryVolume()
				&& !TerritoryComponent->IsTouchTerritoryVolume(ItemLocation,0.f))
			{
				Score = 0.f;
			}
		}
	
		It.SetScore(TestPurpose, FilterType, Score, MinValue, MaxValue);
	}
}

/**
 * @brief 
 */
FText UCSKit_EnvQueryTest_Territory::GetDescriptionTitle() const
{
	return FText::FromString(FString(TEXT("CSKit_Territory")));
}

/**
 * @brief 
 */
FText UCSKit_EnvQueryTest_Territory::GetDescriptionDetails() const
{
	return FText::FromString(FString(TEXT("CSKit_Territory")));
}
