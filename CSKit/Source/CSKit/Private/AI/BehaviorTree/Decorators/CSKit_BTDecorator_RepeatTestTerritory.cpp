// Copyright 2020 megasuraman.
/**
 * @file CSKit_BTDecorator_RepeatTestTerritory.cpp
 * @brief BTDecorator Territory判定
 * @author megasuraman
 * @date 2025/05/19
 */
#include "AI/BehaviorTree/Decorators/CSKit_BTDecorator_RepeatTestTerritory.h"

#include "AI/CSKit_AIController.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "BehaviorTree/BTCompositeNode.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AI/Territory/CSKit_TerritoryComponent.h"

UCSKit_BTDecorator_RepeatTestTerritory::UCSKit_BTDecorator_RepeatTestTerritory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	NodeName = "CSKit_TargetTerritory";
	mBBKey_Target.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UCSKit_BTDecorator_RepeatTestTerritory, mBBKey_Target));
	mBBKey_Target.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UCSKit_BTDecorator_RepeatTestTerritory, mBBKey_Target), AActor::StaticClass());
}

/* ------------------------------------------------------------
   !FBlackboardKeySelectorの初期化のために
------------------------------------------------------------ */
void UCSKit_BTDecorator_RepeatTestTerritory::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);

	if (const UBlackboardData* BBAsset = GetBlackboardAsset())
	{
		mBBKey_Target.ResolveSelectedKey(*BBAsset);
	}
	else
	{
		UE_LOG(LogBehaviorTree, Warning, TEXT("Can't initialize %s due to missing blackboard data."), *GetName());
		mBBKey_Target.InvalidateResolvedKey();
	}
}

/* ------------------------------------------------------------
   !条件判定
------------------------------------------------------------ */
bool UCSKit_BTDecorator_RepeatTestTerritory::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	const ACSKit_AIController* AIController = Cast<ACSKit_AIController>( OwnerComp.GetAIOwner() );
	if (AIController == nullptr)
	{
		return false;
	}
	const UCSKit_TerritoryComponent* TerritoryComponent = AIController->GetCSKitTerritoryComponent();
	if (TerritoryComponent == nullptr)
	{
		return false;
	}

	if(!TerritoryComponent->IsOwnTerritoryVolume())
	{
		return false;
	}

	switch (mJudge)
	{
	case ECSKit_BTDecoratorTerritoryJudge::Owner:
		return TerritoryComponent->IsInside();
	case ECSKit_BTDecoratorTerritoryJudge::NoticeTarget:
		if(const AActor* NoticeTarget = AIController->GetNoticeTarget())
		{
			return TerritoryComponent->IsTouchTerritoryVolume(NoticeTarget->GetActorLocation(), mJudgeRadius);
		}
		break;
	case ECSKit_BTDecoratorTerritoryJudge::BBKey:
		{
			FVector TargetPos = FAISystem::InvalidLocation;
			const UBlackboardComponent* MyBlackboard = OwnerComp.GetBlackboardComponent();
			if(mBBKey_Target.SelectedKeyType == UBlackboardKeyType_Vector::StaticClass())
			{
				TargetPos = MyBlackboard->GetValue<UBlackboardKeyType_Vector>(mBBKey_Target.GetSelectedKeyID());
			}
			else if(mBBKey_Target.SelectedKeyType == UBlackboardKeyType_Object::StaticClass())
			{
				if (const AActor* TargetActor = Cast<AActor>(MyBlackboard->GetValue<UBlackboardKeyType_Object>(mBBKey_Target.GetSelectedKeyID())))
				{
					TargetPos = TargetActor->GetActorLocation();
				}
			}
			if(FAISystem::IsValidLocation(TargetPos))
			{
				return TerritoryComponent->IsTouchTerritoryVolume(TargetPos, mJudgeRadius);
			}
			break;
		}
	default:
		break;
	}

	return false;
}