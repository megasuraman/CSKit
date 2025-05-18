// Copyright 2020 megasuraman.
/**
 * @file CSKit_NeedLevelManager.cpp
 * @brief AIの処理LOD管理
 * @author megasuraman
 * @date 2025/05/05
 */
#include "AI/CSKit_NeedLevelManager.h"

#include "AI/CSKit_AIController.h"
#include "CSKit_Config.h"
#include "AI/Community/CSKit_CommunityComponent.h"
#include "AI/Community/CSKit_CommunityNodeBase.h"
#include "Engine/World.h"

#if USE_CSKIT_DEBUG
#include "ScreenWindow/CSKitDebug_ScreenWindowManager.h"
#endif

FCSKit_NeedLevelTarget::FCSKit_NeedLevelTarget()
	:mFixedPos(FAISystem::InvalidLocation)
{
}

FCSKit_NeedLevelTarget::FCSKit_NeedLevelTarget(ACSKit_AIController* InAIController)
	: mFixedPos(FAISystem::InvalidLocation)
	, mAIController(InAIController)
{

}

/* ------------------------------------------------------------
   !対象のAIController取得
------------------------------------------------------------ */
ACSKit_AIController* FCSKit_NeedLevelTarget::GetAIController() const
{
	return mAIController.Get();
}
/* ------------------------------------------------------------
   !対象の必要度変更
------------------------------------------------------------ */
bool FCSKit_NeedLevelTarget::SetNeedLevel(const ECSKit_NeedLevel InLevel)
{
	if (mNeedLevel == InLevel)
	{
		return false;
	}

	const ECSKit_NeedLevel OldNeedLevel = mNeedLevel;
	mNeedLevel = InLevel;
	OnChangeNeedLevel(OldNeedLevel, mNeedLevel);
	return true;
}

/**
 * @brief 
 */
bool FCSKit_NeedLevelTarget::IsUseFixedPos() const
{
	return mFixedPos!=FAISystem::InvalidLocation;
}

/* ------------------------------------------------------------
   !対象の必要度変化時
------------------------------------------------------------ */
void FCSKit_NeedLevelTarget::OnChangeNeedLevel(const ECSKit_NeedLevel InOldLevel, const ECSKit_NeedLevel InNewLevel) const
{
	ACSKit_AIController* AIController = GetAIController();
	if (AIController == nullptr)
	{
		return;
	}

	APawn* Pawn = AIController->GetPawn();
	if (Pawn == nullptr)
	{
		return;
	}

	switch (InNewLevel)
	{
	case ECSKit_NeedLevel::Zero:
		SetSleepActor(AIController, true);
		SetSleepActor(Pawn, true);
		break;
	case ECSKit_NeedLevel::Low:
		SetSleepActor(AIController, false);
		SetSleepActor(Pawn, false);
		AIController->SetLowMode(true);
		break;
	case ECSKit_NeedLevel::High:
		SetSleepActor(AIController, false);
		SetSleepActor(Pawn, false);
		AIController->SetLowMode(false);
		break;
	default:
		break;
	}
}

/* ------------------------------------------------------------
   !指定Actorのon/off
------------------------------------------------------------ */
void FCSKit_NeedLevelTarget::SetSleepActor(AActor* InTarget, const bool bInSleep)
{
	InTarget->SetActorTickEnabled(!bInSleep);

	TInlineComponentArray<UActorComponent*> Components;
	InTarget->GetComponents(Components);
	for (UActorComponent* Component : Components)
	{
		if (Component->IsRegistered())
		{
			Component->SetComponentTickEnabled(!bInSleep);
		}
	}
}

UCSKit_NeedLevelManager::UCSKit_NeedLevelManager()
{
	const UCSKit_Config* CSKitConfig = GetDefault<UCSKit_Config>();
	SetNeedLevelDistanceBorder(ECSKit_NeedLevel::Invalid, 0.f);
	SetNeedLevelDistanceBorder(ECSKit_NeedLevel::High, CSKitConfig->mNeedLevelDistanceBorderHigh);
	SetNeedLevelDistanceBorder(ECSKit_NeedLevel::Low, CSKitConfig->mNeedLevelDistanceBorderLow);
	SetNeedLevelDistanceBorder(ECSKit_NeedLevel::Zero, CSKitConfig->mNeedLevelDistanceBorderZero);
}

/* ------------------------------------------------------------
   !更新処理
------------------------------------------------------------ */
void UCSKit_NeedLevelManager::Update(const float InDeltaSec)
{
	if (GetWorld()->IsNetMode(NM_Client))
	{
		return;
	}

	TArray<FVector> FactorPosList;
	if (!CollectFactorPosList(FactorPosList))
	{
		return;
	}

	const int32 TargetListNum = mTargetList.Num();
	if (TargetListNum <= 0)
	{
		return;
	}

	//1フレームにチェックする数を制限
	const int32 BeginListIndex = mNextTargetListIndex;
	int32 ListIndex = BeginListIndex;
	if (ListIndex >= TargetListNum)
	{
		ListIndex = 0;
	}
	int32 CheckTargetNum = 0;
	while (CheckTargetNum < mFrameCheckTargetMaxNum)
	{
		FCSKit_NeedLevelTarget& Target = mTargetList[ListIndex];
		UpdateTarget(Target, FactorPosList);

		++CheckTargetNum;
		++ListIndex;
		if (ListIndex >= TargetListNum)
		{
			ListIndex = 0;
		}
		if (ListIndex == BeginListIndex)
		{
			break;
		}
	}

	mNextTargetListIndex = ListIndex;
}

/* ------------------------------------------------------------
   !必要度を決める要素の登録と解除
------------------------------------------------------------ */
void UCSKit_NeedLevelManager::EntryFactor(const AActor* InActor)
{
	mFactorList.AddUnique(InActor);
}
void UCSKit_NeedLevelManager::ExitFactor(const AActor* InActor)
{
	mFactorList.RemoveSingleSwap(InActor);
}

/* ------------------------------------------------------------
   !必要度をチェックする対象の登録と解除
------------------------------------------------------------ */
void UCSKit_NeedLevelManager::EntryTarget(ACSKit_AIController* InAIController)
{
	const FCSKit_NeedLevelTarget Target(InAIController);
	mTargetList.AddUnique(Target);
}
void UCSKit_NeedLevelManager::ExitTarget(ACSKit_AIController* InAIController)
{
	const FCSKit_NeedLevelTarget Target(InAIController);
	mTargetList.RemoveSingleSwap(Target);
}

/**
 * @brief 
 */
FCSKit_NeedLevelTarget* UCSKit_NeedLevelManager::FindNeedLevelTarget(const ACSKit_AIController* InAIController)
{
	for (FCSKit_NeedLevelTarget& Target : mTargetList)
	{
		if(Target.GetAIController() == InAIController)
		{
			return &Target;
		}
	}
	return nullptr;
}

void UCSKit_NeedLevelManager::SetNeedLevelDistanceBorder(const ECSKit_NeedLevel InLevel, const float InDistance)
{
	if (InLevel != ECSKit_NeedLevel::Num)
	{
		mNeedLevelDistanceBorder[static_cast<uint8>(InLevel)] = InDistance;
	}
}

/* ------------------------------------------------------------
   !必要度を決める要素の座標リスト
------------------------------------------------------------ */
float UCSKit_NeedLevelManager::GetNeedLevelDistanceBorder(const ECSKit_NeedLevel InLevel) const
{
	if (InLevel != ECSKit_NeedLevel::Num)
	{
		return mNeedLevelDistanceBorder[static_cast<uint8>(InLevel)];
	}
	return -1.f;
}

/* ------------------------------------------------------------
   !必要度を決める要素の座標リスト
------------------------------------------------------------ */
bool UCSKit_NeedLevelManager::CollectFactorPosList(TArray<FVector>& OutList) const
{
	for (const auto& Element : mFactorList)
	{
		if (const AActor* Factor = Element.Get())
		{
			OutList.Add(Factor->GetActorLocation());
		}
	}
	return OutList.Num() > 0;
}

/* ------------------------------------------------------------
   !対象の必要度更新
------------------------------------------------------------ */
void UCSKit_NeedLevelManager::UpdateTarget(FCSKit_NeedLevelTarget& InTarget, const TArray<FVector>& InFactorPosList)
{
	const ECSKit_NeedLevel NeedLevel = CalcTargetNeedLevel(InTarget, InFactorPosList);
	if (NeedLevel != ECSKit_NeedLevel::Invalid)
	{
		const ECSKit_NeedLevel OldNeedLevel = InTarget.GetNeedLevel();
		if(InTarget.SetNeedLevel(NeedLevel))
		{
			OnChangedNeedLevel(InTarget, OldNeedLevel);
		}
	}
}

/* ------------------------------------------------------------
   !適切なNeedLevel計算
------------------------------------------------------------ */
ECSKit_NeedLevel UCSKit_NeedLevelManager::CalcTargetNeedLevel(const FCSKit_NeedLevelTarget& InTarget, const TArray<FVector>& InFactorPosList) const
{
#if USE_CSKIT_DEBUG
	if (mDebugFixNeedLevel != ECSKit_NeedLevel::Invalid)
	{
		return mDebugFixNeedLevel;
	}
#endif

	const ACSKit_AIController* AIController = InTarget.GetAIController();
	if (AIController == nullptr)
	{
		return ECSKit_NeedLevel::Invalid;
	}

	const APawn* Pawn = AIController->GetPawn();
	if (Pawn == nullptr)
	{
		return ECSKit_NeedLevel::Invalid;
	}

	if (AIController->GetNoticeTarget() != nullptr)
	{
		return ECSKit_NeedLevel::High;
	}
	const UCSKit_CommunityNodeBase* CommunityNode = nullptr;
	if(const UCSKit_CommunityComponent* CommunityComponent = AIController->GetCSKitCommunity())
	{
		CommunityNode = CommunityComponent->GetEntryNode();
	}

	FVector BasePos = Pawn->GetActorLocation();
	float BaseRadius = 50.f;
	if(InTarget.IsUseFixedPos())
	{
		BasePos = InTarget.GetFixedPos();
	}
	else if(CommunityNode)
	{
		BasePos = CommunityNode->GetCenterPos();
		BaseRadius = CommunityNode->GetCenterRadius();
	}
	BaseRadius += InTarget.GetOffsetRadius();
	
	float MinDistanceSq = FLT_MAX;
	for (const FVector& FactorPos : InFactorPosList)
	{
		const float DistanceSq = FVector::DistSquared(BasePos, FactorPos);
		if (DistanceSq < MinDistanceSq)
		{
			MinDistanceSq = DistanceSq;
		}
	}

	const float MinDistance = FMath::Sqrt(MinDistanceSq);
	ECSKit_NeedLevel SelectLevel;
	if (MinDistance > GetNeedLevelDistanceBorder(ECSKit_NeedLevel::Zero) + BaseRadius)
	{
		SelectLevel = ECSKit_NeedLevel::Zero;
	}
	else if (MinDistance > GetNeedLevelDistanceBorder(ECSKit_NeedLevel::Low) + BaseRadius)
	{
		SelectLevel = ECSKit_NeedLevel::Low;
	}
	else
	{
		SelectLevel = ECSKit_NeedLevel::High;
	}

	if (InTarget.GetNeedLevel() != ECSKit_NeedLevel::Invalid
		&& InTarget.GetNeedLevel() != SelectLevel)
	{
		if (FMath::Abs(MinDistance - GetNeedLevelDistanceBorder(SelectLevel) + BaseRadius) < mNeedLevelDistanceOffset)
		{
			return ECSKit_NeedLevel::Invalid;
		}
	}

	return SelectLevel;
}

#if USE_CSKIT_DEBUG
/* ------------------------------------------------------------
   !デバッグ表示
------------------------------------------------------------ */
void UCSKit_NeedLevelManager::DebugDraw(UCanvas* InCanvas)
{
	if (mbDebugDrawInfo)
	{
		DebugDrawInfo(InCanvas);
	}
	if (mbDebugDrawTargetInfo)
	{
		DebugDrawTargetInfo(InCanvas);
	}
}
/* ------------------------------------------------------------
   !デバッグ表示
------------------------------------------------------------ */
void UCSKit_NeedLevelManager::DebugDrawInfo(UCanvas* InCanvas)
{
	if (UCSKitDebug_ScreenWindowManager* DebugWindowManager = UCSKitDebug_ScreenWindowManager::Get(this))
	{
		FString Message;
		Message += FString::Printf(TEXT("FactorNum : %d\n"), mFactorList.Num());
		int32 NeedLevelHighNum = 0;
		int32 NeedLevelLowNum = 0;
		int32 NeedLevelZeroNum = 0;
		int32 NeedLevelOtherNum = 0;
		for (const FCSKit_NeedLevelTarget& Target : mTargetList)
		{
			switch (Target.GetNeedLevel())
			{
			case ECSKit_NeedLevel::High:
				++NeedLevelHighNum;
				break;
			case ECSKit_NeedLevel::Low:
				++NeedLevelLowNum;
				break;
			case ECSKit_NeedLevel::Zero:
				++NeedLevelZeroNum;
				break;
			default:
				++NeedLevelOtherNum;
				break;
			}
		}
		Message += FString::Printf(TEXT("NeedLevelHighNum : %d\n"), NeedLevelHighNum);
		Message += FString::Printf(TEXT("NeedLevelLowNum : %d\n"), NeedLevelLowNum);
		Message += FString::Printf(TEXT("NeedLevelZeroNum : %d\n"), NeedLevelZeroNum);
		Message += FString::Printf(TEXT("NeedLevelOtherNum : %d\n"), NeedLevelOtherNum);
		Message += FString::Printf(TEXT("mNextTargetListIndex : %d\n"), mNextTargetListIndex);

		DebugWindowManager->AddWindow(TEXT("NeedLevelManager"), Message);
	}
}
/* ------------------------------------------------------------
   !デバッグ表示
------------------------------------------------------------ */
void UCSKit_NeedLevelManager::DebugDrawTargetInfo(UCanvas* InCanvas)
{

}
#endif