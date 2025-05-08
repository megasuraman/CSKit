// Copyright 2020 megasuraman.
/**
 * @file CSKit_CommunityManager.cpp
 * @brief AIのチーム制御管理
 * @author megasuraman
 * @date 2025/05/06
 */
#include "AI/Community/CSKit_CommunityManager.h"

#include "AI/Community/CSKit_CommunityNodeBase.h"
#include "AI/Community/CSKit_CommunityComponent.h"


 void UCSKit_CommunityManager::Update(const float InDeltaSec)
 {
 	TArray< UCSKit_CommunityNodeBase*> DeleteList;
 	for (UCSKit_CommunityNodeBase* Node : mNodeList)
 	{
 		Node->Tick(InDeltaSec);
 		if (Node->IsRequestDelete())
 		{
 			DeleteList.Add(Node);
 		}
 	}
 
 	for (UCSKit_CommunityNodeBase* Node : DeleteList)
 	{
 		DeleteCommunity(Node);
 	}
 }

/* ------------------------------------------------------------
   !Community生成
------------------------------------------------------------ */
UCSKit_CommunityNodeBase* UCSKit_CommunityManager::CreateCommunity(UCSKit_CommunityComponent* InMember)
{
	if (InMember
		&& !InMember->IsOwnCommunity())
	{
		UCSKit_CommunityNodeBase* CommunityNode;
		if (const UClass* NodeClass = InMember->GetNodeClass().Get())
		{
			CommunityNode = NewObject<UCSKit_CommunityNodeBase>(this, NodeClass);
		}
		else
		{
			CommunityNode = NewObject<UCSKit_CommunityNodeBase>(this);
		}
		CommunityNode->BeginPlay();
		CommunityNode->Entry(InMember);
		mNodeList.Add(CommunityNode);
		return CommunityNode;
	}
	return nullptr;
}
/* ------------------------------------------------------------
   !Community削除
------------------------------------------------------------ */
void UCSKit_CommunityManager::DeleteCommunity(UCSKit_CommunityNodeBase* InCommunityNode)
{
	for (int32 i = 0; i < mNodeList.Num(); ++i)
	{
		if (mNodeList[i] == InCommunityNode)
		{
			mNodeList.RemoveAt(i);
			break;
		}
	}

	if (InCommunityNode->GetMemberNum() > 0)
	{
		InCommunityNode->ExitAll();
	}
}

#if USE_CSKIT_DEBUG
/* ------------------------------------------------------------
   !デバッグ表示
------------------------------------------------------------ */
void	UCSKit_CommunityManager::DebugDraw(UCanvas* InCanvas)
{
	if(mbDebugDrawNodeList)
	{
		for (const UCSKit_CommunityNodeBase* Node : mNodeList)
		{
			Node->DebugDraw(InCanvas);
		}
	}
}
#endif