// Copyright 2020 megasuraman.
/**
 * @file CSKit_CommunityComponent.cpp
 * @brief AIのチーム制御用Component
 * @author megasuraman
 * @date 2025/05/06
 */
#include "AI/Community/CSKit_CommunityComponent.h"

#include "AI/CSKit_AIController.h"
#include "CSKit_Subsystem.h"
#include "AIController.h"
#include "AI/Community/CSKit_CommunityManager.h"
#include "AI/Community/CSKit_CommunityNodeBase.h"


// Sets default values for this component's properties
UCSKit_CommunityComponent::UCSKit_CommunityComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UCSKit_CommunityComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UCSKit_CommunityComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	RequestExitCommunity();
}

/* ------------------------------------------------------------
   !
------------------------------------------------------------ */
// ReSharper disable once CppMemberFunctionMayBeStatic
void UCSKit_CommunityComponent::Update(const float InDeltaSec)
{
}

/* ------------------------------------------------------------
   !チーム化リクエスト
------------------------------------------------------------ */
void UCSKit_CommunityComponent::RequestEntryCommunity(UCSKit_CommunityComponent* InPartner)
{
	if (InPartner == nullptr)
	{
		return;
	}

	const bool bOwnCommunity = IsOwnCommunity();
	const bool bOwnCommunityPartner = InPartner->IsOwnCommunity();
	if (bOwnCommunity
		&& bOwnCommunityPartner)
	{
		return;
	}
	
	if (bOwnCommunity)
	{
		if (UCSKit_CommunityNodeBase* CommunityNode = GetEntryNode())
		{
			CommunityNode->Entry(InPartner);
		}
	}
	else if (bOwnCommunityPartner)
	{
		if (UCSKit_CommunityNodeBase* CommunityNode = InPartner->GetEntryNode())
		{
			CommunityNode->Entry(this);
		}
	}
	else
	{
		const UCSKit_Subsystem* CSKitSubsystem = GetWorld()->GetSubsystem<UCSKit_Subsystem>();
		if (CSKitSubsystem == nullptr)
		{
			return;
		}
		UCSKit_CommunityManager* CommunityManager = CSKitSubsystem->GetCommunityManager();
		if (CommunityManager == nullptr)
		{
			return;
		}
		if (UCSKit_CommunityNodeBase* CommunityNode = CommunityManager->CreateCommunity(this))
		{
			CommunityNode->Entry(InPartner);
		}
	}
}

/* ------------------------------------------------------------
   !チームを抜けるリクエスト
------------------------------------------------------------ */
void UCSKit_CommunityComponent::RequestExitCommunity()
{
	if (UCSKit_CommunityNodeBase* CommunityNode = mEntryCommunityNode.Get())
	{
		if (CommunityNode->GetMemberNum() <= 2)
		{
			const UCSKit_Subsystem* CSKitSubsystem = GetWorld()->GetSubsystem<UCSKit_Subsystem>();
			if (CSKitSubsystem == nullptr)
			{
				return;
			}
			UCSKit_CommunityManager* CommunityManager = CSKitSubsystem->GetCommunityManager();
			if (CommunityManager == nullptr)
			{
				return;
			}
			//一人だけチームに残さないように
			CommunityManager->DeleteCommunity(CommunityNode);
		}
		else
		{
			CommunityNode->Exit(this);
		}
	}
}

/* ------------------------------------------------------------
   !チーム人数
------------------------------------------------------------ */
int32 UCSKit_CommunityComponent::GetMemberNum() const
{
	if (const UCSKit_CommunityNodeBase* CommunityNode = mEntryCommunityNode.Get())
	{
		return CommunityNode->GetMemberNum();
	}
	return 0;
}

/* ------------------------------------------------------------
   !一番多かったチーム人数
------------------------------------------------------------ */
int32 UCSKit_CommunityComponent::GetLogMemberNumMax() const
{
	if (const UCSKit_CommunityNodeBase* CommunityNode = mEntryCommunityNode.Get())
	{
		return CommunityNode->GetLogMemberNumMax();
	}
	return 0;
}

/* ------------------------------------------------------------
   !自分がリーダーかどうか
------------------------------------------------------------ */
bool UCSKit_CommunityComponent::IsLeader() const
{
	if (const UCSKit_CommunityNodeBase* CommunityNode = mEntryCommunityNode.Get())
	{
		return CommunityNode->IsLeader(this);
	}
	return false;
}
/* ------------------------------------------------------------
   !メンバーかどうか
------------------------------------------------------------ */
bool UCSKit_CommunityComponent::IsMember(const UCSKit_CommunityComponent* InTarget) const
{
	if (const UCSKit_CommunityNodeBase* CommunityNode = mEntryCommunityNode.Get())
	{
		return CommunityNode->IsOwn(InTarget);
	}
	return false;
}
bool UCSKit_CommunityComponent::IsMemberActor(const AActor* InTarget) const
{
	if(InTarget == nullptr)
	{
		return false;
	}
	const ACSKit_AIController* AIController = Cast<ACSKit_AIController>(InTarget);
	if(AIController == nullptr)
	{
		if(const APawn* Pawn = Cast<APawn>(InTarget))
		{
			AIController = Cast<ACSKit_AIController>(Pawn->GetController());
		}
		else if(const APawn* OwnerPawn = Cast<APawn>(InTarget->GetOwner()))
		{
			AIController = Cast<ACSKit_AIController>(OwnerPawn->GetController());
		}
	}
	
	if(AIController)
	{
		return IsMember(AIController->GetCSKitCommunity());
	}
	return false;
}

/* ------------------------------------------------------------
   !リーダー変更リクエスト
------------------------------------------------------------ */
void UCSKit_CommunityComponent::RequestChangeLeader() const
{
	if (UCSKit_CommunityNodeBase* CommunityNode = mEntryCommunityNode.Get())
	{
		CommunityNode->RequestChangeLeader();
	}
}
/* ------------------------------------------------------------
   !座標取得
------------------------------------------------------------ */
FVector	UCSKit_CommunityComponent::GetOwnerPos() const
{
	if (const AActor* OwnerActor = GetOwner())
	{
		if (const AAIController* AIController = Cast<AAIController>(OwnerActor))
		{
			if (const APawn* Pawn = AIController->GetPawn())
			{
				return Pawn->GetActorLocation();
			}
		}
		return OwnerActor->GetActorLocation();
	}

	return FVector::ZeroVector;
}

/* ------------------------------------------------------------
   !リーダーのPawn取得
------------------------------------------------------------ */
APawn* UCSKit_CommunityComponent::GetLeaderPawn() const
{
	if (const UCSKit_CommunityNodeBase* CommunityNode = mEntryCommunityNode.Get())
	{
		if(const UCSKit_CommunityComponent* LeaderComponent = CommunityNode->GetLeader())
		{
			if(const AAIController* LeaderAIComponent = Cast<AAIController>(LeaderComponent->GetOwner()))
			{
				return LeaderAIComponent->GetPawn();
			}
		}
	}
	return nullptr;
}

/* ------------------------------------------------------------
   !
------------------------------------------------------------ */
void UCSKit_CommunityComponent::AddDelegateOnEntryCommunity(const FCSKit_OnEntryCommunity::FDelegate& InDelegate)
{
	mDelegateOnEntryCommunity.Add(InDelegate);
}

/* ------------------------------------------------------------
   !
------------------------------------------------------------ */
void UCSKit_CommunityComponent::AddDelegateOnExitCommunity(const FCSKit_OnExitCommunity::FDelegate& InDelegate)
{
	mDelegateOnExitCommunity.Add(InDelegate);
}

/* ------------------------------------------------------------
   !チームに入る(UCSKit_CommunityNodeBaseから操作用)
------------------------------------------------------------ */
bool UCSKit_CommunityComponent::EntryCommunityByNode(UCSKit_CommunityNodeBase* InNode)
{
	//多重登録認めない
	if (mEntryCommunityNode.Get() == nullptr)
	{
		mEntryCommunityNode = InNode;
		OnEntryCommunity();
		return true;
	}
	return false;
}
/* ------------------------------------------------------------
   !チームを抜ける(UCSKit_CommunityNodeBaseから操作用)
------------------------------------------------------------ */
bool UCSKit_CommunityComponent::ExitCommunityByNode(const UCSKit_CommunityNodeBase* InNode)
{
	if (mEntryCommunityNode.Get() == InNode)
	{
		OnExitCommunity();
		mEntryCommunityNode = nullptr;
		return true;
	}
	return false;
}

/* ------------------------------------------------------------
   !
------------------------------------------------------------ */
void UCSKit_CommunityComponent::OnEntryCommunity() const
{
	mDelegateOnEntryCommunity.Broadcast();
}

/* ------------------------------------------------------------
   !
------------------------------------------------------------ */
void UCSKit_CommunityComponent::OnExitCommunity() const
{
	mDelegateOnExitCommunity.Broadcast();
}
