// Copyright 2020 megasuraman.
/**
 * @file CSKit_NoticeComponent.cpp
 * @brief 全AIのNoticeTarget情報の取りまとめ
 * @author megasuraman
 * @date 2025/05/05
 */
#include "AI/Notice/CSKit_NoticeTargetManager.h"

#include "DrawDebugHelpers.h"
#include "AI/CSKit_AIController.h"
#include "Engine/World.h"
#include "AI/Notice/CSKit_NoticeComponent.h"
#include "ScreenWindow/CSKitDebug_ScreenWindowManager.h"

#if USE_CSKIT_DEBUG
#include "ScreenWindow/CSKitDebug_ScreenWindowText.h"
#endif


void FCSKit_SameNoticeMember::Update()
{
	if(const ACSKit_AIController* AIController = mAIController.Get())
	{
		if(const UCSKit_NoticeComponent* NoticeComponent = AIController->GetCSKitNotice())
		{
			mScore = NoticeComponent->GetSameTargetPriorityScore();
		}
	}
}

void FCSKit_SameNoticeGroup::Update()
{
	for(FCSKit_SameNoticeMember& Member : mMemberList)
	{
		Member.Update();
	}
	
	mMemberList.Sort([](const FCSKit_SameNoticeMember& Base, const FCSKit_SameNoticeMember& Target)
	{
		return (Base.mScore > Target.mScore);
	}
	);
}

UCSKit_NoticeTargetManager::UCSKit_NoticeTargetManager()
{
}

/* ------------------------------------------------------------
   !更新処理
------------------------------------------------------------ */
void UCSKit_NoticeTargetManager::Update(const float InDeltaSec)
{
	if (GetWorld()->IsNetMode(NM_Client))
	{
		return;
	}

	for(auto& MapElement : mSameNoticeGroupMap)
	{
		FCSKit_SameNoticeGroup& SameNoticeGroup = MapElement.Value;
		SameNoticeGroup.Update();
	}
}

/* ------------------------------------------------------------
   !mNoticeTargetMapへの登録と解除
------------------------------------------------------------ */
void UCSKit_NoticeTargetManager::Entry(AActor* InTarget, ACSKit_AIController* InAIController)
{
	if(InTarget == nullptr
		|| InAIController == nullptr)
	{
		return;
	}
	FCSKit_SameNoticeGroup& SameNoticeGroup = mSameNoticeGroupMap.FindOrAdd(InTarget);
	for(FCSKit_SameNoticeMember& Member : SameNoticeGroup.mMemberList)
	{
		if(Member.mAIController.Get() == InAIController)
		{
			return;;
		}
	}
	FCSKit_SameNoticeMember NewMember;
	NewMember.mAIController = InAIController;
	SameNoticeGroup.mMemberList.Add(NewMember);
}
void UCSKit_NoticeTargetManager::Exit(AActor* InTarget, ACSKit_AIController* InAIController)
{
	if(FCSKit_SameNoticeGroup* SameNoticeGroup = mSameNoticeGroupMap.Find(InTarget))
	{
		for(int32 i=0; i<SameNoticeGroup->mMemberList.Num(); ++i)
		{
			if(SameNoticeGroup->mMemberList[i].mAIController.Get() == InAIController)
			{
				SameNoticeGroup->mMemberList.RemoveAtSwap(i);
				break;
			}
		}
	}
}

/* ------------------------------------------------------------
   !
------------------------------------------------------------ */
int32 UCSKit_NoticeTargetManager::CalcMemberNum(AActor* InTarget, const ACSKit_AIController* InIgnoreAIController) const
{
	const FCSKit_SameNoticeGroup* SameNoticeGroup = mSameNoticeGroupMap.Find(InTarget);
	if(SameNoticeGroup == nullptr)
	{
		return 0;
	}
	int32 MemberNum = 0;
	for(const FCSKit_SameNoticeMember& Member : SameNoticeGroup->mMemberList)
	{
		if(Member.mAIController.Get() != InIgnoreAIController)
		{
			++MemberNum;
		}
	}
	return MemberNum;
}

/* ------------------------------------------------------------
   !
------------------------------------------------------------ */
const FCSKit_SameNoticeGroup* UCSKit_NoticeTargetManager::FindSameNoticeGroup(AActor* InTarget) const
{
	return mSameNoticeGroupMap.Find(InTarget);
}

#if USE_CSKIT_DEBUG
/* ------------------------------------------------------------
   !デバッグ表示
------------------------------------------------------------ */
void UCSKit_NoticeTargetManager::DebugDraw(UCanvas* InCanvas) const
{
	if (mbDebugDrawInfo)
	{
		DebugDrawInfo(InCanvas);
		DebugDrawSameNoticeGroupInfo(InCanvas);
	}
}
/* ------------------------------------------------------------
   !デバッグ表示
------------------------------------------------------------ */
void UCSKit_NoticeTargetManager::DebugDrawInfo(UCanvas* InCanvas) const
{
	if(UCSKitDebug_ScreenWindowManager* DebugWindowManager = UCSKitDebug_ScreenWindowManager::Get(this))
	{
		FString Message;
		Message += FString::Printf(TEXT("SameNoticeGroupNum : %d\n"), mSameNoticeGroupMap.Num());
		DebugWindowManager->AddWindow(TEXT("NoticeTargetManager"), Message);
	}
}

/* ------------------------------------------------------------
   !FCSKit_NoticeMemberInfoデバッグ表示
------------------------------------------------------------ */
void UCSKit_NoticeTargetManager::DebugDrawSameNoticeGroupInfo(UCanvas* InCanvas) const
{
	static constexpr int32 ColorNum = 4;
	static FColor ColorList[ColorNum] = {FColor::Red, FColor::Green, FColor::Yellow, FColor::Blue};
	int32 ColorIndex = 0;
	for(const auto& MapElement : mSameNoticeGroupMap)
	{
		const AActor* NoticeTarget = MapElement.Key.Get();
		if(NoticeTarget == nullptr)
		{
			continue;
		}
		DebugDrawSameNoticeGroupInfo(InCanvas, *NoticeTarget, MapElement.Value, ColorList[ColorIndex]);

		ColorIndex = (ColorIndex + 1) % ColorNum;
	}
}
void UCSKit_NoticeTargetManager::DebugDrawSameNoticeGroupInfo(UCanvas* InCanvas, const AActor& InTarget, const FCSKit_SameNoticeGroup& InGroup, const FColor InColor) const
{
	const FVector TargetPos = InTarget.GetActorLocation();
	FCSKitDebug_ScreenWindowText ScreenWindowText;
	ScreenWindowText.SetWindowName(TEXT("NoticeTarget"));
	ScreenWindowText.SetWindowFrameColor(InColor);
	ScreenWindowText.AddText(FString::Printf(TEXT("MemberNum : %d"), InGroup.mMemberList.Num()));
	
	for(int32 i=0; i<InGroup.mMemberList.Num(); ++i)
	{
		const FCSKit_SameNoticeMember& Member = InGroup.mMemberList[i];
		const ACSKit_AIController* AIController = Member.mAIController.Get();
		if(AIController == nullptr)
		{
			continue;
		}
		const APawn* MemberPawn = AIController->GetPawn();
		if(MemberPawn == nullptr)
		{
			continue;
		}
		const FVector MemberPos = MemberPawn->GetActorLocation();
		DrawDebugLine(GetWorld(), TargetPos, MemberPos, InColor, false, -1.f, 255, 3.f);
		
		FCSKitDebug_ScreenWindowText MemberScreenWindowText;
		MemberScreenWindowText.SetWindowName(TEXT("Member"));
		MemberScreenWindowText.SetWindowFrameColor(InColor);
		MemberScreenWindowText.AddText(MemberPawn->GetName());
		MemberScreenWindowText.AddText(FString::Printf(TEXT("Ranking : %d"), i+1));
		MemberScreenWindowText.AddText(FString::Printf(TEXT("Score : %.1f"), Member.mScore));
		MemberScreenWindowText.Draw(InCanvas, MemberPos);
	}

	ScreenWindowText.Draw(InCanvas, TargetPos);
}
#endif
