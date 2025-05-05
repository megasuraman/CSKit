// Copyright 2020 megasuraman.
/**
 * @file CSKit_AIFlow.cpp
 * @brief 配置での行動指定用Actor
 * @author megasuraman
 * @date 2025/05/05
 */
#include "AI/AIFlow/CSKit_AIFlow.h"

#include "CSKit_Math.h"
#include "CSKit_SceneComponent.h"
#include "DrawDebugHelpers.h"
#include "AI/AIFlow/CSKit_AIFlowNode.h"
#include "Components/BillboardComponent.h"
#include "Debug/DebugDrawService.h"
#include "Misc/UObjectToken.h"

#if USE_CSKIT_DEBUG
#include "CSKitDebug_Draw.h"
#include "ScreenWindow/CSKitDebug_ScreenWindowText.h"
#endif

ACSKit_AIFlow::ACSKit_AIFlow(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	UCSKit_SceneComponent* CSKitSceneComponent = ObjectInitializer.CreateDefaultSubobject<UCSKit_SceneComponent>(this, TEXT("SceneComponent"));
	RootComponent = CSKitSceneComponent;
	RootComponent->Mobility = EComponentMobility::Static;
#if WITH_EDITOR
	const auto ChangedDelegate = FCSKit_OnAddChildDelegate::CreateUObject(this, &ACSKit_AIFlow::EditorOnAddChild);
	CSKitSceneComponent->EditorSetOnAddChildDelegate(ChangedDelegate);
#endif
	
	bNetLoadOnClient = false;
	
	
#if WITH_EDITORONLY_DATA
	SpriteComponent = CreateEditorOnlyDefaultSubobject<UBillboardComponent>(TEXT("Sprite"));

	if (!IsRunningCommandlet())
	{
		// Structure to hold one-time initialization
		struct FConstructorStatics
		{
			ConstructorHelpers::FObjectFinderOptional<UTexture2D> TargetIconSpawnObject;
			ConstructorHelpers::FObjectFinderOptional<UTexture2D> TargetIconObject;
			FName ID_TargetPoint;
			FText NAME_TargetPoint;
			FConstructorStatics()
				: TargetIconSpawnObject(TEXT("/Engine/EditorMaterials/TargetIconSpawn"))
				, TargetIconObject(TEXT("/Engine/EditorResources/S_ReflActorIcon"))
				, ID_TargetPoint(TEXT("TargetPoint"))
				, NAME_TargetPoint(NSLOCTEXT("SpriteCategory", "TargetPoint", "Target Points"))
			{
			}
		};

		if (SpriteComponent)
		{
			static FConstructorStatics ConstructorStatics;
			SpriteComponent->Sprite = ConstructorStatics.TargetIconObject.Get();
			SpriteComponent->SetRelativeScale3D_Direct(FVector(0.5f, 0.5f, 0.5f));
			SpriteComponent->SpriteInfo.Category = ConstructorStatics.ID_TargetPoint;
			SpriteComponent->SpriteInfo.DisplayName = ConstructorStatics.NAME_TargetPoint;
			SpriteComponent->bIsScreenSizeScaled = true;

			SpriteComponent->SetupAttachment(RootComponent);
		}
	}
#endif
}

#if ENGINE_MAJOR_VERSION == 5
void ACSKit_AIFlow::PreSave(FObjectPreSaveContext ObjectSaveContext)
{
	Super::PreSave(ObjectSaveContext);
	
#if WITH_EDITOR
	if(!ObjectSaveContext.IsCooking()
		&& GetWorld() != nullptr)
	{
		EditorCheckError();
	}
#endif
}
#endif

/* ------------------------------------------------------------
  !
------------------------------------------------------------ */
const FCSKit_AIFlowNodeData* ACSKit_AIFlow::GetNodeData(const int32 InIndex) const
{
	if(InIndex >= 0
		&& InIndex < mNodeDataList.Num())
	{
		return &mNodeDataList[InIndex];
	}
	return nullptr;
}

/* ------------------------------------------------------------
  !
------------------------------------------------------------ */
void ACSKit_AIFlow::PostInitProperties()
{
	Super::PostInitProperties();

// #if WITH_EDITOR
// 	if(const UWorld* World = GetWorld())
// 	{
// 		if(World->IsEditorWorld())
// 		{
// 			SetActorTransform(FTransform::Identity);
// 		}
// 	}
// #endif
#if USE_CSKIT_DEBUG
	DebugRequestDraw(true);
#endif
}

/* ------------------------------------------------------------
  !
------------------------------------------------------------ */
void ACSKit_AIFlow::PostLoad()
{
	Super::PostLoad();
	
#if USE_CSKIT_DEBUG
	DebugRequestDraw(true);
#endif
}

/* ------------------------------------------------------------
  !
------------------------------------------------------------ */
void ACSKit_AIFlow::BeginPlay()
{
	Super::BeginPlay();
	
	const FTransform& Transform = GetActorTransform();
	for (FCSKit_AIFlowNodeData& NodeData : mNodeDataList)
	{
		NodeData.mPos = Transform.TransformPosition(NodeData.mPos);
	}
}

/* ------------------------------------------------------------
  !
------------------------------------------------------------ */
void ACSKit_AIFlow::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

/* ------------------------------------------------------------
  !
------------------------------------------------------------ */
void ACSKit_AIFlow::BeginDestroy()
{
	Super::BeginDestroy();
#if USE_CSKIT_DEBUG
	DebugRequestDraw(false);
#endif
}

#if USE_CSKIT_DEBUG
/* ------------------------------------------------------------
  !
------------------------------------------------------------ */
void ACSKit_AIFlow::DebugDrawCall(UCanvas* InCanvas) const
{
	mDebugDrawSec += GetWorld()->GetDeltaSeconds();
	if(mDebugDrawSec > 1.f)
	{
		mDebugDrawSec = 0.f;
	}
	mDebugDrawRatio = UCSKit_Math::CalcSimpleEaseInOut(mDebugDrawSec);

	DebugDrawInfo(InCanvas);
	DebugDrawNodeList(InCanvas);
}

/* ------------------------------------------------------------
  !
------------------------------------------------------------ */
void ACSKit_AIFlow::DebugRequestDraw(const bool bInActive)
{
	if (bInActive)
	{
		if (!mDebugDrawHandle.IsValid())
		{
			const auto DebugDrawDelegate = FDebugDrawDelegate::CreateUObject(this, &ACSKit_AIFlow::DebugDraw);
			if (DebugDrawDelegate.IsBound())
			{
				mDebugDrawHandle = UDebugDrawService::Register(TEXT("GameplayDebug"), DebugDrawDelegate);
			}
		}
	}
	else
	{
		if (mDebugDrawHandle.IsValid())
		{
			UDebugDrawService::Unregister(mDebugDrawHandle);
			mDebugDrawHandle.Reset();
		}
	}
}
/* ------------------------------------------------------------
  !
------------------------------------------------------------ */
void ACSKit_AIFlow::DebugDraw(UCanvas* InCanvas, class APlayerController* InPlayerController)
{
#if WITH_EDITOR
	EditorCheckLastSelectedNode();
#endif

	if(!DebugIsWishDraw())
	{
		return;
	}
#if WITH_EDITOR
	if(mbEditorRequestSetupNodeDataList)
	{
		EditorSetupNodeDataList();
		mbEditorRequestSetupNodeDataList = false;	
	}
#endif
	DebugDrawCall(InCanvas);
}
/* ------------------------------------------------------------
  !
------------------------------------------------------------ */
void ACSKit_AIFlow::DebugDrawInfo(UCanvas* InCanvas) const
{
	FCSKitDebug_ScreenWindowText ScreenWindowText;
	ScreenWindowText.SetWindowName(FString(TEXT("AIFlow")));
	ScreenWindowText.AddText(FString::Printf(TEXT("NodeNum : %d"), mNodeDataList.Num()));
	ScreenWindowText.Draw(InCanvas, GetActorLocation());
}
/* ------------------------------------------------------------
  !
------------------------------------------------------------ */
void ACSKit_AIFlow::DebugDrawNodeList(UCanvas* InCanvas) const
{
	FTransform BaseTransform = GetActorTransform();
	if(GetWorld()->IsGameWorld())
	{
		BaseTransform = FTransform::Identity;
	}
	for(const FCSKit_AIFlowNodeData& Data : mNodeDataList)
	{
		const FVector NodePos = BaseTransform.TransformPosition(Data.mPos);
		FColor Color = FColor::Green;
#if WITH_EDITOR
		if(Data.mbEditorGeneratedByRoute)
		{
			Color = FColor::Orange;
		}
#endif
		//Nodeの矢印表示
		UCSKitDebug_Draw::OctahedronArrow OctahedronArrow;
		OctahedronArrow.mBasePos = NodePos + FVector(0.f, 0.f, 100.f);
#if WITH_EDITOR
		if(Data.mbEditorGeneratedByRoute)
		{
			OctahedronArrow.mBasePos = NodePos + FVector(50.f, 50.f, 50.f);
		}
		else if(Data.mEditorGenerateRouteChildIndex != INDEX_NONE)
		{
			OctahedronArrow.mBasePos = NodePos + FVector(-50.f, -50.f, 50.f);
		}
#endif
		OctahedronArrow.mTargetPos = NodePos;
		OctahedronArrow.Draw(GetWorld(), Color, 0, 1.f);

		//Node情報表示
		FCSKitDebug_ScreenWindowText ScreenWindowText;
		if (mbDrawAllNodeInfo
			//|| mEditorLastSelectedNode == Data.mEditorOriginalAIFlowNode
			)
		{
			ScreenWindowText.SetWindowName(FString::Printf(TEXT("Node[%d]"), Data.mIndex));
#if WITH_EDITOR
			if(const ACSKit_AIFlowNode* AIFlowNode = Data.mEditorOriginalAIFlowNode.Get())
			{
				ScreenWindowText.AddText(AIFlowNode->GetActorLabel());
			}
#endif
			ScreenWindowText.AddText(FString::Printf(TEXT("Action : %s"), *Data.mActionName.ToString()));
			ScreenWindowText.AddText(FString::Printf(TEXT("mWaitTimeAfterAction : %.1f"), Data.mWaitTimeAfterAction));
			ScreenWindowText.AddText(FString::Printf(TEXT("LinkIndexList(%d)"), Data.mLinkIndexList.Num()));
			FString LinkIndexListString;
			for(const int32& Index : Data.mLinkIndexList)
			{
				LinkIndexListString += FString::Printf(TEXT("%d "), Index);
			}
			ScreenWindowText.AddText(FString::Printf(TEXT("   %s"), *LinkIndexListString));
		}
		else
		{
			ScreenWindowText.AddText(FString::Printf(TEXT("Node[%d]"), Data.mIndex));
		}
		ScreenWindowText.SetWindowFrameColor(Color);
		ScreenWindowText.Draw(InCanvas, OctahedronArrow.mBasePos);

		//Link先と結ぶ
		for(const int32& Index : Data.mLinkIndexList)
		{
			const FCSKit_AIFlowNodeData* LinkData = GetNodeData(Index);
			if(LinkData == nullptr)
			{
				continue;
			}
			const FVector LinkPos = BaseTransform.TransformPosition(LinkData->mPos);
			DrawDebugLine(GetWorld(), OctahedronArrow.mBasePos, LinkPos, Color, false, -1.f, 0, 2.f);
			const FVector AnimationPos = FMath::Lerp(OctahedronArrow.mBasePos, LinkPos, mDebugDrawRatio);
			DrawDebugPoint(GetWorld(), AnimationPos, 10.f, Color);
		}
	}
}

/* ------------------------------------------------------------
  !
------------------------------------------------------------ */
bool ACSKit_AIFlow::DebugIsWishDraw() const
{
#if WITH_EDITOR
	if(IsSelectedInEditor())
	{
		return true;
	}
	TArray<AActor*> ChildActorList;
	GetAttachedActors(ChildActorList);
	for(const AActor* ChildActor : ChildActorList)
	{
		if(const ACSKit_AIFlowNode* AIFlowNode = Cast<ACSKit_AIFlowNode>(ChildActor))
		{
			if(AIFlowNode->IsSelectedInEditor())
			{
				return true;
			}
		}
	}
#endif
	return false;
}
#endif

#if WITH_EDITOR
/* ------------------------------------------------------------
  !
------------------------------------------------------------ */
void ACSKit_AIFlow::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	//SetActorTransform(FTransform::Identity);

	EditorSetupNodeDataList();
}

/* ------------------------------------------------------------
  !
------------------------------------------------------------ */
void ACSKit_AIFlow::EditorSetupNodeDataListButton()
{
	EditorSetupNodeDataList();
	EditorCheckError();
}

/* ------------------------------------------------------------
  !
------------------------------------------------------------ */
void ACSKit_AIFlow::EditorSetupNodeDataList()
{
	TArray<const ACSKit_AIFlowNode*> AIFlowNodeActorList;
	EditorGetNodeActorList(AIFlowNodeActorList);
	EditorSortNodeActorList(AIFlowNodeActorList);
	EditorSetupNodeActorList(AIFlowNodeActorList);
	EditorGenerateRoute();
}

/* ------------------------------------------------------------
  !
------------------------------------------------------------ */
void ACSKit_AIFlow::EditorUpdateNodeDataList(const ACSKit_AIFlowNode* InNode)
{
	if(InNode == nullptr)
	{
		return;
	}
	for(FCSKit_AIFlowNodeData& Data : mNodeDataList)
	{
		if(Data.mEditorOriginalAIFlowNode.Get() != InNode)
		{
			continue;
		}

		InNode->EditorGetAIFlowNodeData(Data);
	}
}

/* ------------------------------------------------------------
  !
------------------------------------------------------------ */
bool ACSKit_AIFlow::EditorCalcNodeBoundingSphere(FVector& OutPos, float& OutRadius) const
{
	TArray<const ACSKit_AIFlowNode*> AIFlowNodeActorList;
	EditorGetNodeActorList(AIFlowNodeActorList);

	const int32 NodeNum = AIFlowNodeActorList.Num();
	if(NodeNum <= 0)
	{
		return false;
	}
	OutPos = FVector::ZeroVector;
	for(const ACSKit_AIFlowNode* Node : AIFlowNodeActorList)
	{
		if(Node)
		{
			OutPos += Node->GetActorLocation();
		}
	}
	OutPos = OutPos / static_cast<double>(NodeNum);

	OutRadius = 0.f;
	for(const ACSKit_AIFlowNode* Node : AIFlowNodeActorList)
	{
		if(Node)
		{
			const float Distance = FVector::Distance(OutPos, Node->GetActorLocation());
			if(Distance > OutRadius)
			{
				OutRadius = Distance;
			}
		}
	}
	return true;
}

/* ------------------------------------------------------------
  !
------------------------------------------------------------ */
void ACSKit_AIFlow::EditorGenerateRoute()
{
	switch (mEditorGenerateRouteType)
	{
	case ECSKit_AIFlowGenerateRouteType::Loop:
		EditorGenerateRoute_Loop();
	default:
		break;
	}
}

/* ------------------------------------------------------------
  !
------------------------------------------------------------ */
void ACSKit_AIFlow::EditorGenerateRoute_Loop()
{
	//自動生成されたのは一旦削除
	EditorDeleteNodeGeneratedByRoute();
	
	//相互リンクはNG
	for(const FCSKit_AIFlowNodeData& NodeData : mNodeDataList)
	{
		for(const int32 LinkIndex : NodeData.mLinkIndexList)
		{
			const FCSKit_AIFlowNodeData& LinkNodeData = mNodeDataList[LinkIndex];
			if(LinkNodeData.mLinkIndexList.Find(NodeData.mIndex) != INDEX_NONE)
			{
				const FString ErrorMessage = FString::Printf(TEXT("相互リンクがある状態でLoopは使えません"));
				FMessageLog EnemyCheck("CSKit");
				EnemyCheck.Warning()
					->AddToken(FUObjectToken::Create(this))
					->AddToken(FTextToken::Create(FText::FromString(ErrorMessage)))
					;
				EnemyCheck.Open();
				return;;
			}
		}
	}

	//まずはコピーを作成
	for(int32 i=0; i<mNodeDataList.Num(); ++i)
	{
		FCSKit_AIFlowNodeData& NodeData = mNodeDataList[i];
		if(NodeData.mbEditorGeneratedByRoute)
		{
			break;
		}
		FCSKit_AIFlowNodeData AIFlowNodeData = NodeData;
		AIFlowNodeData.mIndex = mNodeDataList.Num();
		AIFlowNodeData.mLinkIndexList.Empty();
		AIFlowNodeData.mbEditorGeneratedByRoute = true;
		AIFlowNodeData.mEditorGenerateRouteParentIndex = NodeData.mIndex;

		NodeData.mEditorGenerateRouteChildIndex = AIFlowNodeData.mIndex;
		
		mNodeDataList.Add(AIFlowNodeData);
	}

	TMap<int32,int32> EndNodeLinkMap;
	for(int32 i=0; i<mNodeDataList.Num(); ++i)
	{
		const FCSKit_AIFlowNodeData& NodeData = mNodeDataList[i];
		if(NodeData.mbEditorGeneratedByRoute)
		{
			break;
		}
		//自分に対してリンクしてるNodeIndexを得る
		TArray<int32> LinkIndexList;
		for(const FCSKit_AIFlowNodeData& LinkNodeData : mNodeDataList)
		{
			if(LinkNodeData.mbEditorGeneratedByRoute)
			{
				continue;
			}
			if(LinkNodeData.mLinkIndexList.Find(NodeData.mIndex) != INDEX_NONE)
			{
				LinkIndexList.Add(LinkNodeData.mEditorGenerateRouteChildIndex);
			}
		}
		
		FCSKit_AIFlowNodeData& ChildNodeData = mNodeDataList[NodeData.mEditorGenerateRouteChildIndex];
		ChildNodeData.mLinkIndexList = LinkIndexList;
		
		if(ChildNodeData.mLinkIndexList.Num() == 0)
		{
			EndNodeLinkMap.Add(ChildNodeData.mIndex, NodeData.mIndex);
		}
		else if(NodeData.mLinkIndexList.Num() == 0)
		{
			EndNodeLinkMap.Add(NodeData.mIndex, ChildNodeData.mIndex);
		}
	}

	if(EndNodeLinkMap.Num() < 2)
	{
		EditorDeleteNodeGeneratedByRoute();

		const FString ErrorMessage = FString::Printf(TEXT("終端ノードがないためLoop化できない"));
		FMessageLog EnemyCheck("CSKit");
		EnemyCheck.Warning()
			->AddToken(FUObjectToken::Create(this))
			->AddToken(FTextToken::Create(FText::FromString(ErrorMessage)))
			;
		EnemyCheck.Open();
		return;
	}

	for(const auto& MapElement : EndNodeLinkMap)
	{
		mNodeDataList[MapElement.Key].mLinkIndexList.Add(MapElement.Value);
	}
}

/* ------------------------------------------------------------
  !自動生成のNodeを削除
------------------------------------------------------------ */
void ACSKit_AIFlow::EditorDeleteNodeGeneratedByRoute()
{
	while(true)
	{
		bool bFinish = true;
		for(int32 i=0; i<mNodeDataList.Num(); ++i)
		{
			if(mNodeDataList[i].mbEditorGeneratedByRoute)
			{
				mNodeDataList.RemoveAt(i);
				bFinish = false;
				break;
			}
		}

		if(bFinish)
		{
			break;
		}
	}
}

/* ------------------------------------------------------------
  !エラーチェック
------------------------------------------------------------ */
void ACSKit_AIFlow::EditorCheckError() const
{
	if(!GetWorld()->IsEditorWorld())
	{
		return;
	}

	if(mNodeDataList.Num() == 0)
	{
		const FString ErrorMessage = FString::Printf(TEXT("NodeDataListが空"));
		FMessageLog EnemyCheck("CSKit");
		EnemyCheck.Warning()
			->AddToken(FUObjectToken::Create(this))
			->AddToken(FTextToken::Create(FText::FromString(ErrorMessage)))
			;
		EnemyCheck.Open();
	}
	{
		int32 NoLinkNodeNum = 0;
		for(const FCSKit_AIFlowNodeData& NodeData : mNodeDataList)
		{
			if(NodeData.mLinkIndexList.Num() == 0)
			{
				++NoLinkNodeNum;
			}
		}
		if(NoLinkNodeNum > 1)
		{
			const FString ErrorMessage = FString::Printf(TEXT("繋がってないNodeが複数ある(%d)"), NoLinkNodeNum-1);
			FMessageLog EnemyCheck("CSKit");
			EnemyCheck.Warning()
				->AddToken(FUObjectToken::Create(this))
				->AddToken(FTextToken::Create(FText::FromString(ErrorMessage)))
				;
			EnemyCheck.Open();
		}
	}
}

/* ------------------------------------------------------------
  !
------------------------------------------------------------ */
void ACSKit_AIFlow::EditorGetNodeActorList(TArray<const ACSKit_AIFlowNode*>& OutList) const
{
	TArray<AActor*> ChildActorList;
	GetAttachedActors(ChildActorList);
	for(const AActor* ChildActor : ChildActorList)
	{
		if(const ACSKit_AIFlowNode* AIFlowNode = Cast<ACSKit_AIFlowNode>(ChildActor))
		{
			OutList.Add(AIFlowNode);
		}
	}
}

/* ------------------------------------------------------------
  !
------------------------------------------------------------ */
void ACSKit_AIFlow::EditorSortNodeActorList(TArray<const ACSKit_AIFlowNode*>& OutList)
{
	OutList.Sort([&](const ACSKit_AIFlowNode& Item1, const ACSKit_AIFlowNode& Item2) {
		return Item1.GetLinkList().Num() > Item2.GetLinkList().Num();
		});
}

/* ------------------------------------------------------------
  !
------------------------------------------------------------ */
void ACSKit_AIFlow::EditorSetupNodeActorList(const TArray<const ACSKit_AIFlowNode*>& InList)
{
	mNodeDataList.Empty();
	for(int32 i=0; i<InList.Num(); ++i)
	{
		const ACSKit_AIFlowNode* AIFlowNode = InList[i];
		FCSKit_AIFlowNodeData AIFlowNodeData;
		AIFlowNode->EditorGetAIFlowNodeData(AIFlowNodeData);
		AIFlowNodeData.mIndex = i;

		for(const TSoftObjectPtr<ACSKit_AIFlowNode>& LinkActor : AIFlowNode->GetLinkList())
		{
			const int32 LinkIndex = InList.Find(LinkActor.Get());
			if(LinkIndex != INDEX_NONE)
			{
				AIFlowNodeData.mLinkIndexList.Add(LinkIndex);
			}
		}

		AIFlowNodeData.mEditorOriginalAIFlowNode = FSoftObjectPath(AIFlowNode);
		
		mNodeDataList.Add(AIFlowNodeData);
	}
}

/* ------------------------------------------------------------
  !
------------------------------------------------------------ */
void ACSKit_AIFlow::EditorOnAddChild(USceneComponent* InChild)
{
	//EditorSetupNodeDataList();
	mbEditorRequestSetupNodeDataList = true;
}

/* ------------------------------------------------------------
  !
------------------------------------------------------------ */
void ACSKit_AIFlow::EditorCheckLastSelectedNode()
{
	mEditorLastSelectedNode = nullptr;
	TArray<AActor*> ChildActorList;
	GetAttachedActors(ChildActorList);
	for(const AActor* ChildActor : ChildActorList)
	{
		if(const ACSKit_AIFlowNode* AIFlowNode = Cast<ACSKit_AIFlowNode>(ChildActor))
		{
			if(AIFlowNode->IsSelectedInEditor())
			{
				mEditorLastSelectedNode = FSoftObjectPath(AIFlowNode);
			}
		}
	}
}
#endif
