// Copyright 2020 megasuraman.
/**
 * @file CSKit_CommunityComponent.cpp
 * @brief AIのチーム制御ノード
 * @author megasuraman
 * @date 2025/05/06
 */
#include "AI/Community/CSKit_CommunityNodeBase.h"

#include "AI/CSKit_AIController.h"
#include "AI/Community/CSKit_CommunityComponent.h"
#include "DrawDebugHelpers.h"

#if USE_CSKIT_DEBUG
#include "ScreenWindow/CSKitDebug_ScreenWindowText.h"
#endif

UCSKit_CommunityNodeBase::UCSKit_CommunityNodeBase(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
	,mbRequestDelete(false)
	,mbOwnNoticeTarget(false)
{

}

void	UCSKit_CommunityNodeBase::Tick(const float InDeltaSec)
{
	mCenterPos = CalcCenterPos();
	mCenterRadius = CalcCenterRadius();
	CheckOwnNoticeTarget();
}

/* ------------------------------------------------------------
   !チームに入れる
------------------------------------------------------------ */
bool UCSKit_CommunityNodeBase::Entry(UCSKit_CommunityComponent* InComponent)
{
	if (InComponent
		&& !IsOwn(InComponent))
	{
		if (InComponent->EntryCommunityByNode(this))
		{
			if (mMemberList.Num() == 0)
			{
				mLeader = InComponent;
			}
			mMemberList.Add(InComponent);
			mLogMemberNumMax = FMath::Max(mLogMemberNumMax, mMemberList.Num());
			OnEntry(InComponent);
			return true;
		}
	}
	return false;
}
/* ------------------------------------------------------------
   !チームから出す
------------------------------------------------------------ */
bool UCSKit_CommunityNodeBase::Exit(UCSKit_CommunityComponent* InComponent)
{
	OnExit(InComponent);
	if (InComponent
		&& IsOwn(InComponent))
	{
		if (InComponent->ExitCommunityByNode(this))
		{
			for (int32 i = 0; i < mMemberList.Num(); ++i)
			{
				if (mMemberList[i].Get() == InComponent)
				{
					mMemberList.RemoveAt(i);
					if (IsLeader(InComponent))
					{
						if (mMemberList.Num() > 0)
						{
							mLeader = mMemberList[0];
						}
						else
						{
							mLeader = nullptr;
						}
					}
					return true;
				}
			}
		}
	}
	return false;
}
/* ------------------------------------------------------------
   !全員チームから出す
------------------------------------------------------------ */
void UCSKit_CommunityNodeBase::ExitAll()
{
	OnPreExitAll();
	
	while(mMemberList.Num() > 0)
	{
		Exit(mMemberList[0].Get());
	}
	mMemberList.Empty();
}
/* ------------------------------------------------------------
   !チームに居るかどうか
------------------------------------------------------------ */
bool UCSKit_CommunityNodeBase::IsOwn(const UCSKit_CommunityComponent* InComponent) const
{
	for (const auto& WeakPtr : mMemberList)
	{
		if (WeakPtr.Get() == InComponent)
		{
			return true;
		}
	}
	return false;
}
/* ------------------------------------------------------------
   !リーダー変更
------------------------------------------------------------ */
void UCSKit_CommunityNodeBase::RequestChangeLeader()
{
	const int32 MemberNum = mMemberList.Num();
	if (MemberNum <= 1)
	{
		return;
	}

	int32 LeaderIndex = 0;
	for (int32 i=0; i<MemberNum; ++i)
	{
		if (IsLeader(mMemberList[i].Get()))
		{
			LeaderIndex = i;
			break;
		}
	}

	const int32 NextLeaderIndex = (LeaderIndex + 1) % MemberNum;
	mLeader = mMemberList[NextLeaderIndex];
}

/* ------------------------------------------------------------
   !リーダー取得
------------------------------------------------------------ */
UCSKit_CommunityComponent* UCSKit_CommunityNodeBase::GetLeader() const
{
	return mLeader.Get();
}


/* ------------------------------------------------------------
   !チームのAIControllerList
------------------------------------------------------------ */
void UCSKit_CommunityNodeBase::GetMemberAIControllerList(TArray<ACSKit_AIController*>& OutList) const
{
	for (const auto& WeakPtr : mMemberList)
	{
		const UCSKit_CommunityComponent* Member = WeakPtr.Get();
		if (Member == nullptr)
		{
			continue;
		}
		if(ACSKit_AIController* MemberAIController = Cast<ACSKit_AIController>(Member->GetOwner()))
		{
			OutList.Add(MemberAIController);
		}
	}
}

/* ------------------------------------------------------------
   !チームのPawnList
------------------------------------------------------------ */
void UCSKit_CommunityNodeBase::GetMemberPawnList(TArray<APawn*>& OutList) const
{
	for (const auto& WeakPtr : mMemberList)
	{
		const UCSKit_CommunityComponent* Member = WeakPtr.Get();
		if (Member == nullptr)
		{
			continue;
		}
		const AAIController* MemberAIController = Cast<AAIController>(Member->GetOwner());
		if(MemberAIController == nullptr)
		{
			continue;
		}
		if(APawn* MemberPawn = MemberAIController->GetPawn())
		{
			OutList.Add(MemberPawn);
		}
	}
}
/* ------------------------------------------------------------
	!メンバー同士のコリジョン無効化
------------------------------------------------------------ */
void UCSKit_CommunityNodeBase::SetIgnoreCollisionMember(const bool bInIgnore) const
{
	TArray<APawn*> MemberPawnList;
	GetMemberPawnList(MemberPawnList);
	for(APawn* Base : MemberPawnList)
	{
		for(APawn* Target : MemberPawnList)
		{
			if(Base == Target)
			{
				continue;
			}
			if(bInIgnore)
			{
				Base->MoveIgnoreActorAdd(Target);
			}
			else
			{
				Base->MoveIgnoreActorRemove(Target);
			}
		}
	}
}

/* ------------------------------------------------------------
   !チームの中心座標取得
------------------------------------------------------------ */
FVector	UCSKit_CommunityNodeBase::CalcCenterPos() const
{
	int32 MemberNum = 0;
	FVector CenterPos = FVector::ZeroVector;
	for (const auto& WeakPtr : mMemberList)
	{
		if (const UCSKit_CommunityComponent* Member = WeakPtr.Get())
		{
			CenterPos += Member->GetOwnerPos();
			++MemberNum;
		}
	}

	if (MemberNum > 0)
	{
		return CenterPos / static_cast<float>(MemberNum);
	}
	return FVector::ZeroVector;
}

/* ------------------------------------------------------------
   !チームの中心からの最大半径
------------------------------------------------------------ */
float UCSKit_CommunityNodeBase::CalcCenterRadius() const
{
	float RadiusMaxSq = 0.f;
	for (const auto& WeakPtr : mMemberList)
	{
		if (const UCSKit_CommunityComponent* Member = WeakPtr.Get())
		{
			const float DistanceSq = FVector::DistSquared(Member->GetOwnerPos(), mCenterPos);
			if(DistanceSq > RadiusMaxSq)
			{
				RadiusMaxSq = DistanceSq;
			}
		}
	}
	return FMath::Sqrt(RadiusMaxSq);
}

/* ------------------------------------------------------------
   !
------------------------------------------------------------ */
void UCSKit_CommunityNodeBase::CheckOwnNoticeTarget()
{
	TArray<ACSKit_AIController*> MemberList;
	GetMemberAIControllerList(MemberList);
	bool bOwnNoticeTarget = false;
	for(const ACSKit_AIController* AIController : MemberList)
	{
		if(AIController->GetNoticeTarget() != nullptr)
		{
			bOwnNoticeTarget = true;
			break;
		}
	}

	if(mbOwnNoticeTarget != bOwnNoticeTarget)
	{
		mbOwnNoticeTarget = bOwnNoticeTarget;
		OnChangeOwnNoticeTarget(mbOwnNoticeTarget);
	}
}

#if USE_CSKIT_DEBUG
/* ------------------------------------------------------------
   !デバッグ表示
------------------------------------------------------------ */
void	UCSKit_CommunityNodeBase::DebugDraw(class UCanvas* InCanvas) const
{
	const FVector CenterPos = mCenterPos;
	const FColor LineColor = FColor::White;
	DrawDebugPoint(GetWorld(), CenterPos, 10.f, LineColor, false, -1.f, 255);

	FCSKitDebug_ScreenWindowText DebugInfoWindow;
	DebugInfoWindow.SetWindowName(FString::Printf(TEXT("CommunityNode")));
	DebugInfoWindow.SetWindowFrameColor(LineColor);
	DebugAddDrawInfo(DebugInfoWindow);
	DebugInfoWindow.Draw(InCanvas, CenterPos);

	for (const auto& WeakPtr : mMemberList)
	{
		if (const UCSKit_CommunityComponent* Member = WeakPtr.Get())
		{
			const FVector TargetPos = Member->GetOwnerPos();
			DrawDebugLine(GetWorld(), CenterPos, TargetPos, LineColor, false, -1.f, 255, 2.f);
			FColor PointColor = LineColor;
			if (IsLeader(Member))
			{
				PointColor = FColor::Red;
			}
			DrawDebugPoint(GetWorld(), TargetPos, 10.f, PointColor, false, -1.f, 255);
		}
	}
}
#endif