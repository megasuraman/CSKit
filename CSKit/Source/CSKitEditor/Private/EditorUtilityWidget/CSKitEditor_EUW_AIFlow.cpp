// Copyright 2020 megasuraman.
/**
 * @file CSKitEditor_EUW_AIFlow.cpp
 * @brief AIFlow操作
 * @author megasuraman
 * @date 2025/05/06
 */ 
#include "EditorUtilityWidget/CSKitEditor_EUW_AIFlow.h"

#include "Editor.h"
#include "EngineUtils.h"
#include "AI/AIFlow/CSKit_AIFlow.h"
#include "Engine/Engine.h"

UCSKitEditor_EUW_AIFlow::UCSKitEditor_EUW_AIFlow()
{
}

void UCSKitEditor_EUW_AIFlow::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
}

void UCSKitEditor_EUW_AIFlow::NativeConstruct()
{
	Super::NativeConstruct();
	SetActiveDraw(true);
}
/* ------------------------------------------------------------
   !NativeTick呼べないので
------------------------------------------------------------ */
void UCSKitEditor_EUW_AIFlow::FakeTick()
{
	UpdateSelectNodeList();
}

/* ------------------------------------------------------------
   !
------------------------------------------------------------ */
void UCSKitEditor_EUW_AIFlow::Draw(UCanvas* InCanvas, APlayerController* InPlayerController)
{
	Super::Draw(InCanvas, InPlayerController);

	if(mbDrawAllAIFlow)
	{
		DrawAllAIFlow(InCanvas);
	}
}

/* ------------------------------------------------------------
   !
------------------------------------------------------------ */
void UCSKitEditor_EUW_AIFlow::RequestSetupAllAIFlowData()
{
	UWorld* World = GetWorld_Editor();
	for (TActorIterator<ACSKit_AIFlow> It(World); It; ++It)
	{
		if(ACSKit_AIFlow* AIFlow = *It)
		{
			AIFlow->EditorSetupNodeDataListButton();
		}
	}
}

/* ------------------------------------------------------------
   !複数選択した順にリンクを設定
------------------------------------------------------------ */
void UCSKitEditor_EUW_AIFlow::RequestSetLinkSelectNodeStraight()
{
	ACSKit_AIFlowNode* BaseNode = nullptr;
	for(TSoftObjectPtr<ACSKit_AIFlowNode>& ObjectPtr : mSelectNodeList)
	{
		ACSKit_AIFlowNode* TargetNode = ObjectPtr.Get();
		if(BaseNode
			&& TargetNode)
		{
			BaseNode->EditorAddLinkNode(TargetNode);
		}
		
		BaseNode = TargetNode;
	}
	RequestSetupAllAIFlowData();
}

/* ------------------------------------------------------------
   !
------------------------------------------------------------ */
void UCSKitEditor_EUW_AIFlow::DrawAllAIFlow(UCanvas* InCanvas) const
{
	UWorld* World = GetWorld_Editor();
	if(UWorld* GameWorld = GetWorld_GameServer())
	{
		World = GameWorld;
	}
	for (TActorIterator<ACSKit_AIFlow> It(World); It; ++It)
	{
		if(const ACSKit_AIFlow* AIFlow = *It)
		{
			AIFlow->DebugDrawCall(InCanvas);
		}
	}
}

/* ------------------------------------------------------------
   !
------------------------------------------------------------ */
void UCSKitEditor_EUW_AIFlow::UpdateSelectNodeList()
{
	TArray<TSoftObjectPtr<ACSKit_AIFlowNode>> AllSelectNodeList;
	UWorld* World = GetWorld_Editor();
	const ACSKit_AIFlow* BaseAIFlow = nullptr;
	for (TActorIterator<ACSKit_AIFlowNode> It(World); It; ++It)
	{
		if(const ACSKit_AIFlowNode* AIFlowNode = *It)
		{
			if(BaseAIFlow == nullptr)
			{
				BaseAIFlow = Cast<ACSKit_AIFlow>(AIFlowNode->GetAttachParentActor());
			}
			else if(BaseAIFlow != Cast<ACSKit_AIFlow>(AIFlowNode->GetAttachParentActor()))
			{
				continue;
			}

			if(AIFlowNode->IsSelectedInEditor())
			{
				AllSelectNodeList.Add(TSoftObjectPtr<ACSKit_AIFlowNode>(FSoftObjectPath(AIFlowNode)));
			}
		}
	}

	if(AllSelectNodeList.Num() == 0)
	{
		mSelectNodeList.Empty();
		return;
	}

	TArray<TSoftObjectPtr<ACSKit_AIFlowNode>> OldSelectNodeList = mSelectNodeList;
	mSelectNodeList.Empty();
	//まず現在選択中で以前のリストにあるものを入れる
	for(TSoftObjectPtr<ACSKit_AIFlowNode>& ObjectPtr : OldSelectNodeList)
	{
		if(AllSelectNodeList.Find(ObjectPtr) != INDEX_NONE)
		{
			mSelectNodeList.AddUnique(ObjectPtr);
		}
	}
	//残りを入れる
	for(TSoftObjectPtr<ACSKit_AIFlowNode>& ObjectPtr : AllSelectNodeList)
	{
		mSelectNodeList.AddUnique(ObjectPtr);
	}
}
