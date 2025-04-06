// Copyright 2020 megasuraman.
/**
 * @file CSKit_RecastNavMesh.cpp
 * @brief 
 * @author megasuraman
 * @date 2025/04/06
 */
#include "NavigationSystem/CSKit_RecastNavMesh.h"

#include "DrawDebugHelpers.h"
#include "NavigationSystem.h"
#include "AI/NavDataGenerator.h"
#include "AI/NavigationSystemBase.h"
#include "Debug/DebugDrawService.h"
#include "Engine/Canvas.h"
#include "Engine/LevelScriptBlueprint.h"
#include "NavigationSystem/CSKit_NavigationSafePoint.h"
#include "Framework/Notifications/NotificationManager.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationSystem/NavLinkProxy/CSKit_NavLinkProxyDoor.h"
#include "Widgets/Notifications/SNotificationList.h"
#include "Internationalization\Internationalization.h"

#if USE_CSKIT_DEBUG
#include "ScreenWindow/CSKitDebug_ScreenWindowText.h"
#endif

ACSKit_RecastNavMesh::ACSKit_RecastNavMesh(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

/* ------------------------------------------------------------
  !
------------------------------------------------------------ */
void ACSKit_RecastNavMesh::BeginPlay()
{
	Super::BeginPlay();
	
#if USE_CSKIT_DEBUG
	DebugRequestDraw(true);
#endif
}

/* ------------------------------------------------------------
  !
------------------------------------------------------------ */
void ACSKit_RecastNavMesh::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

#if USE_CSKIT_DEBUG
	DebugRequestDraw(false);
#endif
}

/* ------------------------------------------------------------
   !
------------------------------------------------------------ */
void ACSKit_RecastNavMesh::RequestRebuild()
{
	if (const UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld()))
	{
		const FBox BoundBox = NavSys->GetWorldBounds();
		TArray<FNavigationDirtyArea> DirtyAreaList;
		DirtyAreaList.Add(FNavigationDirtyArea(BoundBox, ENavigationDirtyFlag::All));
		RebuildDirtyAreas(DirtyAreaList);
	}
}

#if USE_CSKIT_DEBUG
/* ------------------------------------------------------------
   !PolyEdgesを元に座標リスト取得
------------------------------------------------------------ */
bool ACSKit_RecastNavMesh::DebugCollectPolyEdgePoint(TArray<FVector>& OutList, const float InInterval) const
{
#if ENGINE_MAJOR_VERSION == 4
	const int32 TileNum = GetNavMeshTilesCount();
	for(int32 i=0; i<TileNum; ++i)
	{
		FRecastDebugGeometry NavMeshGeometry;
		BeginBatchQuery();
		GetDebugGeometry(NavMeshGeometry, i);
		FinishBatchQuery();
#else
	TArray<FNavTileRef> AllNavMeshTileList;
	GetAllNavMeshTiles(AllNavMeshTileList);
	for(const FNavTileRef& NavTileRef : AllNavMeshTileList)
	{
		FRecastDebugGeometry NavMeshGeometry;
		BeginBatchQuery();
		GetDebugGeometryForTile(NavMeshGeometry, NavTileRef);
		FinishBatchQuery();
#endif
		const TArray<FVector>& MeshPosList = NavMeshGeometry.MeshVerts;
		for (int32 AreaIdx = 0; AreaIdx < RECAST_MAX_AREAS; ++AreaIdx)
		{
			const TArray<int32>& MeshIndices = NavMeshGeometry.AreaIndices[AreaIdx];
			for (int32 Idx = 0; Idx < MeshIndices.Num(); Idx += 3)
			{
				TArray<FVector> TrianglePosList;
				TrianglePosList.Add(MeshPosList[MeshIndices[Idx + 0]]);
				TrianglePosList.Add(MeshPosList[MeshIndices[Idx + 1]]);
				TrianglePosList.Add(MeshPosList[MeshIndices[Idx + 2]]);
				const FVector CenterPos = (TrianglePosList[0] + TrianglePosList[1] + TrianglePosList[2])*0.333;
				OutList.Add(CenterPos);
				
				for (const FVector& Pos : TrianglePosList)
				{
					const FVector TargetPos = Pos;
					OutList.Add(TargetPos);
			
					const float TargetDistance = FVector::Distance(CenterPos, TargetPos);
					FVector TargetNV = TargetPos - CenterPos;
					TargetNV.Normalize();
					float OffsetLength = 0.f;
					while(OffsetLength < TargetDistance)
					{
						OutList.Add(CenterPos + TargetNV*OffsetLength);
						OffsetLength += InInterval;
					}
				}
			}
		}
	}

	return OutList.Num() > 0;
}

/* ------------------------------------------------------------
   !NavigationSafePointの影響範囲表示on/off
------------------------------------------------------------ */
void ACSKit_RecastNavMesh::DebugRequestDrawSafeNavMesh(const bool bInDraw)
{
	if(mbDebugDrawSafeNavMesh == bInDraw)
	{
		return;
	}
	mbDebugDrawSafeNavMesh = bInDraw;
	mbDebugFinishCollectSafeNavMesh = false;
	mDebugNextCollectSafeNavMeshTileIndex = 0;
	mDebugSafeNavMeshTrianglePointList.Empty();
	mDebugSafePosList.Empty();

	if(mbDebugDrawSafeNavMesh)
	{
		DebugRequestDraw(true);
	}
}

/* ------------------------------------------------------------
   !デバッグ描画on/off
------------------------------------------------------------ */
void ACSKit_RecastNavMesh::DebugRequestDraw(const bool bInActive)
{
	if (bInActive)
	{
		if (!mDebugDrawHandle.IsValid())
		{
			const auto DebugDrawDelegate = FDebugDrawDelegate::CreateUObject(this, &ACSKit_RecastNavMesh::DebugDraw);
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
   !デバッグ表示
------------------------------------------------------------ */
void ACSKit_RecastNavMesh::DebugDraw(UCanvas* InCanvas, class APlayerController* InPlayerController)
{
	if(mbDebugDrawSafeNavMesh)
	{
		DebugUpdateSafeNavMeshTriangle();
		DebugDrawSafeNavMesh();
	}

#if WITH_EDITOR
	if(mbEditorDrawTriangle)
	{
		DebugDrawNavmeshTriangle(InCanvas);
	}

	if(mbEditorDrawPlyEdgePoint)
	{
		TArray<FVector> PosList;
		DebugCollectPolyEdgePoint(PosList, 100.f);
		const FVector ViewOrigin = InCanvas->SceneView->ViewMatrices.GetViewOrigin();
		for(const FVector& Pos : PosList)
		{
			if (FVector::DistSquared(ViewOrigin, Pos) > FMath::Square(3000.f))
			{
				continue;
			}
			DrawDebugPoint(GetWorld(), Pos, 5.f, FColor::Blue, false, -1.f, 255);
		}
	}
	if(mbEditorDrawNavMeshEdge)
	{
		EditorDrawNavMeshEdge(InCanvas);
	}
#if 0
	FVector BasePos;
	BasePos.InitFromString(TEXT("(X=-158.81597892420288, Y=1562.5429244187615, Z=395)"));
	FVector TargetPos;
	TargetPos.InitFromString(TEXT("(X=-158.81597892420288, Y=1562.5429244187615, Z=-305)"));
	DrawDebugLine(GetWorld(), BasePos, TargetPos, FColor::Blue, false, -1.f, 0, 1.f);
	
	const float CheckRadius = AgentRadius;
	FCollisionShape CheckShape;
	CheckShape.SetSphere(CheckRadius);
	FHitResult HitResult;
	UWorld* World = GetWorld();
	if(World->SweepSingleByChannel(
		HitResult,
		BasePos,
		TargetPos,
		FQuat::Identity,
		ECC_CSKit_CharacterBlocking,
		CheckShape)
		)
	{
		DrawDebugLine(World, BasePos, HitResult.ImpactPoint, FColor::Red, false, -1.f, 0, 2.f);
	}
#endif
#endif
}

/* ------------------------------------------------------------
   !
------------------------------------------------------------ */
void ACSKit_RecastNavMesh::DebugDrawNavmeshTriangle(UCanvas* InCanvas) const
{
	constexpr int32 ColorListNum = 8;
	const FColor ColorList[ColorListNum] = {
		FColor::Red,
		FColor::Blue,
		FColor::Green,
		FColor::Orange,
		FColor::White,
		FColor::Purple,
		FColor::Yellow,
		FColor::Magenta,
	};
	const FVector ViewOrigin = InCanvas->SceneView->ViewMatrices.GetViewOrigin();

#if ENGINE_MAJOR_VERSION == 4
	const int32 TileNum = GetNavMeshTilesCount();
	for(int32 i=0; i<TileNum; ++i)
	{
		FRecastDebugGeometry NavMeshGeometry;
		BeginBatchQuery();
		GetDebugGeometry(NavMeshGeometry, i);
		FinishBatchQuery();
#else
	TArray<FNavTileRef> AllNavMeshTileList;
	GetAllNavMeshTiles(AllNavMeshTileList);
	for(const FNavTileRef& NavTileRef : AllNavMeshTileList)
	{
		FRecastDebugGeometry NavMeshGeometry;
		BeginBatchQuery();
		GetDebugGeometryForTile(NavMeshGeometry, NavTileRef);
		FinishBatchQuery();
#endif

		const TArray<FVector>& MeshPosList = NavMeshGeometry.MeshVerts;
		for (int32 AreaIdx = 0; AreaIdx < RECAST_MAX_AREAS; ++AreaIdx)
		{
			const TArray<int32>& MeshIndices = NavMeshGeometry.AreaIndices[AreaIdx];
			for (int32 Idx = 0; Idx < MeshIndices.Num(); Idx += 3)
			{
				const FColor Color = ColorList[Idx%ColorListNum];
				const FVector PosA = MeshPosList[MeshIndices[Idx + 0]];
				const FVector PosB = MeshPosList[MeshIndices[Idx + 1]];
				const FVector PosC = MeshPosList[MeshIndices[Idx + 2]];
				const FVector CenterPos = (PosA + PosB + PosC)*0.333;
				if (FVector::DistSquared(ViewOrigin, CenterPos) > FMath::Square(3000.f))
				{
					continue;
				}
				
				DrawDebugLine(GetWorld(),PosA,PosB,Color,false,-1.f,255,2.f);
				DrawDebugLine(GetWorld(),PosB,PosC,Color,false,-1.f,255,2.f);
				DrawDebugLine(GetWorld(),PosC,PosA,Color,false,-1.f,255,2.f);
				
				FCSKitDebug_ScreenWindowText ScreenWindowText;
				ScreenWindowText.SetWindowFrameColor(Color);
#if ENGINE_MAJOR_VERSION == 4
				ScreenWindowText.AddText(FString::Printf(TEXT("%d, %d"), i, AreaIdx));
#else
				ScreenWindowText.AddText(FString::Printf(TEXT("%d, %d"), GetTypeHash(NavTileRef), AreaIdx));
#endif
				ScreenWindowText.Draw(InCanvas, CenterPos, 1000.f);
			}
		}
	}
}

/* ------------------------------------------------------------
   !ACSKit_NavigationSafePointの影響範囲を表示するための情報取得
------------------------------------------------------------ */
void ACSKit_RecastNavMesh::DebugUpdateSafeNavMeshTriangle()
{
	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	if(NavSys == nullptr)
	{
		return;
	}
	if (NavSys->GetNumRunningBuildTasks() > 0
		|| NavSys->GetNumRemainingBuildTasks() > 0)
	{
		mbDebugFinishCollectSafeNavMesh = false;
		mDebugNextCollectSafeNavMeshTileIndex = 0;
		mDebugSafeNavMeshTrianglePointList.Empty();
		return;
	}

	TArray<FVector> SafePosList;
	TArray<AActor*> ActorList;
	UGameplayStatics::GetAllActorsOfClass(this, ACSKit_NavigationSafePoint::StaticClass(), ActorList);
	for(const AActor* Actor: ActorList)
	{
		SafePosList.Add(Actor->GetActorLocation());
	}
	if(SafePosList.Num() == 0)
	{
		return;
	}
	if(mDebugSafePosList.Num() == 0)
	{
		mDebugSafePosList = SafePosList;
	}
	else if(mDebugSafePosList.Num() != SafePosList.Num())
	{
		mbDebugFinishCollectSafeNavMesh = false;
		mDebugNextCollectSafeNavMeshTileIndex = 0;
		mDebugSafeNavMeshTrianglePointList.Empty();
		mDebugSafePosList.Empty();
		return;
	}
	else
	{
		for(int32 i=0; i<mDebugSafePosList.Num(); ++i)
		{
			if(FVector::DistSquared(mDebugSafePosList[i], SafePosList[i]) > FMath::Square(10.f))
			{
				mbDebugFinishCollectSafeNavMesh = false;
				mDebugNextCollectSafeNavMeshTileIndex = 0;
				mDebugSafeNavMeshTrianglePointList.Empty();
				mDebugSafePosList.Empty();
				return;
			}
		}
	}

#if ENGINE_MAJOR_VERSION == 4
	const int32 TileCount = GetNavMeshTilesCount();
#else
	TArray<FNavTileRef> AllNavMeshTileList;
	GetAllNavMeshTiles(AllNavMeshTileList);
	const int32 TileCount = AllNavMeshTileList.Num();
#endif
	if( mbDebugFinishCollectSafeNavMesh
		|| mDebugNextCollectSafeNavMeshTileIndex >= TileCount)
	{
		return;
	}
	
	auto CheckNavPath = [&](const FVector& InTargetPos)
	{
		for(const FVector& SafePos : SafePosList)
		{
			FPathFindingQuery Query(this, *this, SafePos, InTargetPos);
			Query.SetAllowPartialPaths(false);
			const FPathFindingResult FindResult = NavSys->FindPathSync(Query);
			if(FindResult.Result == ENavigationQueryResult::Success )
			{
				return true;
			}
		}
		return false;
	};
	
	const int32 BeginIndex = mDebugNextCollectSafeNavMeshTileIndex;
	int32 CheckTileNum = 0;
	for(int32 i=BeginIndex; i<TileCount; ++i)
	{
		constexpr int32 CheckTileNumMax = 50;
		FRecastDebugGeometry NavMeshGeometry;
		BeginBatchQuery();
#if ENGINE_MAJOR_VERSION == 4
		GetDebugGeometry(NavMeshGeometry, i);
#else
		GetDebugGeometryForTile(NavMeshGeometry, AllNavMeshTileList[i]);
#endif
		FinishBatchQuery();

		const TArray<FVector>& MeshPosList = NavMeshGeometry.MeshVerts;
		for (int32 AreaIdx = 0; AreaIdx < RECAST_MAX_AREAS; ++AreaIdx)
		{
			const TArray<int32>& MeshIndices = NavMeshGeometry.AreaIndices[AreaIdx];
			for (int32 Idx = 0; Idx < MeshIndices.Num(); Idx += 3)
			{
				TArray<FVector> TrianglePosList;
				TrianglePosList.Add(MeshPosList[MeshIndices[Idx + 0]]);
				TrianglePosList.Add(MeshPosList[MeshIndices[Idx + 1]]);
				TrianglePosList.Add(MeshPosList[MeshIndices[Idx + 2]]);
				const FVector CenterPos = (TrianglePosList[0] + TrianglePosList[1] + TrianglePosList[2])*0.333;

				if(!CheckNavPath(CenterPos))
				{
					continue;
				}
				mDebugSafeNavMeshTrianglePointList.Append(TrianglePosList);
			}
		}

		mDebugNextCollectSafeNavMeshTileIndex = i;
		++CheckTileNum;
		if(CheckTileNum >= CheckTileNumMax)
		{
			break;
		}
	}

	if(mDebugNextCollectSafeNavMeshTileIndex + 1 >= TileCount)
	{
		mbDebugFinishCollectSafeNavMesh = true;
	}
}
/* ------------------------------------------------------------
   !
------------------------------------------------------------ */
void ACSKit_RecastNavMesh::DebugDrawSafeNavMesh()
{
	TArray<int32> IndexList;
	IndexList.Add(0);
	IndexList.Add(1);
	IndexList.Add(2);
	
	for(int32 i=0; i<mDebugSafeNavMeshTrianglePointList.Num(); i+=3)
	{
		TArray<FVector> TrianglePosList;
		TrianglePosList.Add(mDebugSafeNavMeshTrianglePointList[i + 0]);
		TrianglePosList.Add(mDebugSafeNavMeshTrianglePointList[i + 1]);
		TrianglePosList.Add(mDebugSafeNavMeshTrianglePointList[i + 2]);

		DrawDebugMesh(GetWorld(), TrianglePosList, IndexList, FColor::Blue);
	}
}
#endif

#if WITH_EDITOR
/* ------------------------------------------------------------
   !デバッグ表示
------------------------------------------------------------ */
void ACSKit_RecastNavMesh::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	DebugRequestDraw(mbEditorPossibleToDraw);
}

/* ------------------------------------------------------------
   !NavLink生成
------------------------------------------------------------ */
void ACSKit_RecastNavMesh::EditorGenerateNavLink()
{
	if (EditorIsGeneratingNavLink())
	{
		return;
	}
	EditorSetGenerateNavLinkPhase( ECSKit_GenerateNavLinkPhase::Init );
	//実際の処理はUpdateBuildNavLinkAsyncPhase() UpdateBuildNavLink() で行う
	if (!mEditorTickDelegateHandle.IsValid())
	{
#if ENGINE_MAJOR_VERSION == 4
		mEditorTickDelegateHandle = FTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateUObject(this, &ACSKit_RecastNavMesh::EditorTick));
#else
		mEditorTickDelegateHandle = FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateUObject(this, &ACSKit_RecastNavMesh::EditorTick));
#endif
	}

	mEditorNotificationGenerateNavLink.Reset();
	if (GetWorld()->WorldType == EWorldType::Editor)
	{
		//通知ウィンドウ作成
		FFormatNamedArguments Args;
		Args.Add(TEXT("RemainingTasks"), FText::AsNumber(0));
		const FText DispText = FText::Format(NSLOCTEXT("GenerateNavLink", "GenerateNavLinkInProgress", "Generate NavLink Start ({RemainingTasks})"), Args);
		FNotificationInfo Info(DispText);
		Info.bFireAndForget = false;
		Info.FadeOutDuration = 0.0f;
		Info.ExpireDuration = 0.0f;
		mEditorNotificationGenerateNavLink = FSlateNotificationManager::Get().AddNotification(Info);
		if (mEditorNotificationGenerateNavLink.IsValid())
		{
			mEditorNotificationGenerateNavLink.Pin()->SetCompletionState(SNotificationItem::CS_Pending);
		}
	}
}

/* ------------------------------------------------------------
   !NavLink生成中
------------------------------------------------------------ */
bool ACSKit_RecastNavMesh::EditorIsGeneratingNavLink() const
{
	return mEditorGenerateNavLinkPhase != ECSKit_GenerateNavLinkPhase::Invalid;
}

/* ------------------------------------------------------------
   !
------------------------------------------------------------ */
bool ACSKit_RecastNavMesh::EditorTick(float InDeltaSec)
{
	switch (mEditorGenerateNavLinkPhase)
	{
	case ECSKit_GenerateNavLinkPhase::Init:
	{
		if (EditorGenerateNavLink_Init())
		{
			EditorSetGenerateNavLinkPhase( ECSKit_GenerateNavLinkPhase::WaitBuildNavmesh );
		}
		break;
	}
	case ECSKit_GenerateNavLinkPhase::WaitBuildNavmesh:
	{
		if (NavDataGenerator.IsValid()
			&& NavDataGenerator->GetNumRemaningBuildTasks() == 0)
		{
			EditorSetGenerateNavLinkPhase(ECSKit_GenerateNavLinkPhase::GenerateNavLinkDoor);
		}
		break;
	}
	case ECSKit_GenerateNavLinkPhase::GenerateNavLinkDoor:
	{
		if (EditorGenerateNavLink_Door())
		{
			EditorSetGenerateNavLinkPhase(ECSKit_GenerateNavLinkPhase::SetupNavMeshEdge);
		}
		break;
	}
	case ECSKit_GenerateNavLinkPhase::SetupNavMeshEdge:
	{//EditorIsSafePath()の判定をしたいのでDoorとか終わってからEdge収集
		if (EditorSetupNavMeshEdge())
		{
			EditorSetGenerateNavLinkPhase(ECSKit_GenerateNavLinkPhase::WaitBuildNavmeshFinal);
		}
		break;
	}
	case ECSKit_GenerateNavLinkPhase::WaitBuildNavmeshFinal:
	{
		if (NavDataGenerator.IsValid()
			&& NavDataGenerator->GetNumRemaningBuildTasks() == 0)
		{
			EditorSetGenerateNavLinkPhase(ECSKit_GenerateNavLinkPhase::Finish);
		}
		break;
	}
	case ECSKit_GenerateNavLinkPhase::Finish:
	{
		if (EditorGenerateNavLink_Finish())
		{
			EditorSetGenerateNavLinkPhase( ECSKit_GenerateNavLinkPhase::Invalid );
		}
		break;
	}
	default:
		break;
	}
	return true;
}

/* ------------------------------------------------------------
   !
------------------------------------------------------------ */
void ACSKit_RecastNavMesh::EditorSetGenerateNavLinkPhase(ECSKit_GenerateNavLinkPhase InPhase)
{
	if(mEditorGenerateNavLinkPhase == InPhase)
	{
		return;
	}
	mEditorGenerateNavLinkPhase = InPhase;
	switch (mEditorGenerateNavLinkPhase)
	{
	case ECSKit_GenerateNavLinkPhase::Init:
		mEditorGenerateNavLinkMessage = FString(TEXT("Init"));
		break;
	case ECSKit_GenerateNavLinkPhase::WaitBuildNavmesh:
		RequestRebuild();
		mEditorGenerateNavLinkMessage = FString(TEXT("BuildNavmesh"));
		break;
	case ECSKit_GenerateNavLinkPhase::GenerateNavLinkDoor:
		mEditorGenerateNavLinkMessage = FString(TEXT("Door"));
		break;
	case ECSKit_GenerateNavLinkPhase::WaitBuildNavmeshFinal:
		RequestRebuild();
		mEditorGenerateNavLinkMessage = FString(TEXT("BuildNavmeshFinal"));
		break;
	case ECSKit_GenerateNavLinkPhase::Finish:
		mEditorGenerateNavLinkMessage = FString(TEXT("Finish"));
		break;
	default:
		break;
	}
	
	const TSharedPtr<SNotificationItem> NotificationItem = mEditorNotificationGenerateNavLink.Pin();
	if (NotificationItem.IsValid())
	{
		const FString NotificationMessage = FString::Printf(TEXT("GenerateNavLink_%s"), *mEditorGenerateNavLinkMessage);
		NotificationItem->SetText(
#if ENGINE_MAJOR_VERSION == 4
			FText::FromString(NotificationMessage)
#else
			FText::AsLocalizable_Advanced(*NotificationMessage, TEXT("GenerateNavLink"), *NotificationMessage)
#endif
		);
	}
}


/* ------------------------------------------------------------
   !
------------------------------------------------------------ */
bool ACSKit_RecastNavMesh::EditorGenerateNavLink_Init()
{
	mEditorGenerateNavLinkStartTime = FPlatformTime::Seconds();
	EditorSetupSpawnLevel();
	EditorSetupFolderPath();
	mEditorTileNavMeshEdgeMap.Empty();
	mEditorNavMeshEdgeNextTileIndex = 0;
	mEditorNavMeshEdgeUIDSource = 0;
	EditorSetupSafePosList();
	EditorDeleteNavLinkProxyDoor();
	return true;
}

/* ------------------------------------------------------------
   !
------------------------------------------------------------ */
void ACSKit_RecastNavMesh::EditorSetupSpawnLevel()
{
	const FString TargetLevelName(TEXT("_LD"));
	const TArray<ULevel*> Levels = GetWorld()->GetLevels();
	for (ULevel* Level : Levels)
	{
		if(!Level->bIsVisible)
		{
			continue;
		}
		if (const ULevelScriptBlueprint* LevelScriptBlueprint = Level->GetLevelScriptBlueprint())
		{
			const FString LevelName = LevelScriptBlueprint->GetName();
			if (LevelName.Find(TargetLevelName, ESearchCase::IgnoreCase) != -1)
			{
				mEditorSpawnLevel = Level;
				break;
			}
		}
	}
}

/* ------------------------------------------------------------
   !
------------------------------------------------------------ */
void ACSKit_RecastNavMesh::EditorSetupFolderPath()
{
	mEditorSpawnFolderPath.Empty();
	if (const ULevel* SpawnLevel = mEditorSpawnLevel.Get())
	{
		const FString TargetFolderName(TEXT("Placement"));
		TArray<AActor*> ActorList;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), ActorList);
		for (const AActor* Actor : ActorList)
		{
			if (Actor->GetLevel() == SpawnLevel
				&& Actor->GetFolderPath().IsValid()
				&& !Actor->GetFolderPath().IsNone()
				)
			{
				const FString TargetActorPath = Actor->GetFolderPath().ToString();
				if(TargetActorPath.Find(TargetFolderName, ESearchCase::IgnoreCase) == -1)
				{
					continue;
				}
				TArray<FString> PathNodeList;
				TargetActorPath.ParseIntoArray(PathNodeList, TEXT("/"), true);
				for (const FString& PathNode : PathNodeList)
				{
					mEditorSpawnFolderPath += PathNode;
					mEditorSpawnFolderPath += TEXT("/");
					if (PathNode.Find(TargetFolderName, ESearchCase::IgnoreCase) != -1)
					{
						break;
					}
				}
				break;
			}
		}
	}
	mEditorSpawnFolderPath += TEXT("NavigationData");
}

/* ------------------------------------------------------------
   !
------------------------------------------------------------ */
void ACSKit_RecastNavMesh::EditorSetupSafePosList()
{
	mEditorSafePosList.Empty();
	TArray<AActor*> ActorList;
	UGameplayStatics::GetAllActorsOfClass(this, ACSKit_NavigationSafePoint::StaticClass(), ActorList);
	for(const AActor* Actor: ActorList)
	{
		mEditorSafePosList.Add(Actor->GetActorLocation());
	}
}

/* ------------------------------------------------------------
   !自動生成したNavLinkProxy削除
------------------------------------------------------------ */
void ACSKit_RecastNavMesh::EditorDeleteNavLinkProxyDoor() const
{
	TArray<AActor*> ActorList;
	UGameplayStatics::GetAllActorsOfClass(this, ACSKit_NavLinkProxyDoor::StaticClass(), ActorList);
	for (AActor* Actor : ActorList)
	{
		GetWorld()->DestroyActor(Actor);
	}
}

/* ------------------------------------------------------------
   !
------------------------------------------------------------ */
bool ACSKit_RecastNavMesh::EditorSetupNavMeshEdge()
{
	if (!NavDataGenerator.IsValid()
		|| NavDataGenerator->GetNumRemaningBuildTasks() > 0)
	{
		return false;
	}

	const double StartTime = FPlatformTime::Seconds();

#if ENGINE_MAJOR_VERSION == 4
	const int32 TileCount = GetNavMeshTilesCount();
#else
	TArray<FNavTileRef> AllNavMeshTileList;
	GetAllNavMeshTiles(AllNavMeshTileList);
	const int32 TileCount = AllNavMeshTileList.Num();
#endif
	while (mEditorNavMeshEdgeNextTileIndex < TileCount)
	{
		TArray<FCSKit_NavMeshEdge>& EdgeList = mEditorTileNavMeshEdgeMap.Add(mEditorNavMeshEdgeNextTileIndex);
		// NavMeshのジオメトリ情報を取得
		FRecastDebugGeometry DebugNavGeo;
		DebugNavGeo.bGatherNavMeshEdges = true;
		BeginBatchQuery();
#if ENGINE_MAJOR_VERSION == 4
		GetDebugGeometry(DebugNavGeo, mEditorNavMeshEdgeNextTileIndex);
#else
		GetDebugGeometryForTile(DebugNavGeo, AllNavMeshTileList[mEditorNavMeshEdgeNextTileIndex]);
#endif
		FinishBatchQuery();

		const float BoarderLengthSq = FMath::Square(AgentRadius*1.3f);//狭い幅の地面でもLinkを敷くために、少し小さくする
		const int32 EdgeNum = DebugNavGeo.NavMeshEdges.Num();
		for (int32 Index = 0; Index < EdgeNum; Index += 2)
		{
			const FVector BeginPos = DebugNavGeo.NavMeshEdges[Index];
			const FVector EndPos = DebugNavGeo.NavMeshEdges[Index + 1];
			if (FVector::DistSquared(BeginPos, EndPos) < BoarderLengthSq)
			{
				continue;
			}

			if(!EditorIsSafePath(BeginPos))
			{
				continue;
			}
			
			EdgeList.Add(FCSKit_NavMeshEdge(BeginPos, EndPos, mEditorNavMeshEdgeNextTileIndex, mEditorNavMeshEdgeUIDSource));
			if (mEditorNavMeshEdgeUIDSource < UINT32_MAX)
			{
				++mEditorNavMeshEdgeUIDSource;
			}
		}

		++mEditorNavMeshEdgeNextTileIndex;

		if (FPlatformTime::Seconds() - StartTime > 0.1f)
		{
			return false;
		}
	}
	return true;
}

/* ------------------------------------------------------------
   !SafePosから経路が繋がってるかどうか
------------------------------------------------------------ */
bool ACSKit_RecastNavMesh::EditorIsSafePath(const FVector& InTargetPos) const
{
	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	if(NavSys == nullptr)
	{
		return false;
	}
	for(const FVector& SafePos : mEditorSafePosList)
	{
		FPathFindingQuery Query(this, *this, SafePos, InTargetPos);
		Query.SetAllowPartialPaths(false);
		const FPathFindingResult FindResult = NavSys->FindPathSync(Query);
		if(FindResult.Result == ENavigationQueryResult::Success )
		{
			return true;
		}
	}
	return false;
}

/* ------------------------------------------------------------
   !
------------------------------------------------------------ */
bool ACSKit_RecastNavMesh::EditorGenerateNavLink_Door() const
{
	if (mDoorClass == nullptr)
	{
		return true;
	}
	TArray<ACSKit_NavLinkProxySimpleLink::FDebugPointLink> PointLinkList;
	TArray<AActor*> ActorList;
	UGameplayStatics::GetAllActorsOfClass(this, mDoorClass->GetClass(), ActorList);
	for (AActor* Actor : ActorList)
	{
		constexpr float PointDistance = 150.f;
		const FTransform CenterPoint = Actor->GetActorTransform();
		const FVector BasePos = CenterPoint.GetLocation();
		const FRotator BaseRot = CenterPoint.GetRotation().Rotator();
		FVector Left = BasePos + BaseRot.RotateVector(FVector(PointDistance,0.f,0.f));
		FVector Right = BasePos + BaseRot.RotateVector(FVector(-PointDistance, 0.f, 0.f));
		ACSKit_NavLinkProxySimpleLink::FDebugPointLink PointLink;
		PointLink.mLeft = ACSKit_NavLinkProxySimpleLink::DebugGetGroundPos(GetWorld(), Left);
		PointLink.mRight = ACSKit_NavLinkProxySimpleLink::DebugGetGroundPos(GetWorld(), Right);
		PointLinkList.Add(PointLink);
	}

	if (PointLinkList.Num() == 0)
	{
		return true;
	}

	FActorSpawnParameters SpawnParam;
	SpawnParam.OverrideLevel = mEditorSpawnLevel.Get();
	if (ACSKit_NavLinkProxyDoor* NavLinkProxyDoor = GetWorld()->SpawnActor<ACSKit_NavLinkProxyDoor>(ACSKit_NavLinkProxyDoor::StaticClass(), SpawnParam))
	{
		NavLinkProxyDoor->DebugAssignLink(PointLinkList);
#if WITH_EDITOR
		NavLinkProxyDoor->SetFolderPath(*mEditorSpawnFolderPath);
#endif
	}
	return true;
}

/* ------------------------------------------------------------
   !
------------------------------------------------------------ */
bool ACSKit_RecastNavMesh::EditorGenerateNavLink_Finish() const
{
	CollectGarbage(GARBAGE_COLLECTION_KEEPFLAGS, true);

	UE_LOG(LogNavigation, Log, TEXT("GenerateNavLink TotalTime : %.2lf"), FPlatformTime::Seconds() - mEditorGenerateNavLinkStartTime);

	const TSharedPtr<SNotificationItem> NotificationItem = mEditorNotificationGenerateNavLink.Pin();
	if (NotificationItem.IsValid())
	{
		NotificationItem->SetText(NSLOCTEXT("GenerateNavLink", "GenerateNavLinkComplete", "NavLink生成完了!"));
		NotificationItem->SetCompletionState(SNotificationItem::CS_Success);
		NotificationItem->ExpireAndFadeout();
	}

	return true;
}

/* ------------------------------------------------------------
   !
------------------------------------------------------------ */
void ACSKit_RecastNavMesh::EditorDrawNavMeshEdge(UCanvas* InCanvas) const
{
	if (mEditorGenerateNavLinkPhase != ECSKit_GenerateNavLinkPhase::Invalid)
	{
		return;
	}
	const FSceneView* View = InCanvas->SceneView;
	const FVector ViewPos = View->ViewMatrices.GetViewOrigin();
	const float DispBorderDistanceSq = ARecastNavMesh::GetDrawDistanceSq();

	for (const auto& MapElement : mEditorTileNavMeshEdgeMap)
	{
		const TArray<FCSKit_NavMeshEdge>& NavMeshEdgeList = MapElement.Value;
		for (const FCSKit_NavMeshEdge& Edge : NavMeshEdgeList)
		{
			if (FVector::DistSquared(Edge.GetCenter(), ViewPos) > DispBorderDistanceSq)
			{
				continue;;
			}

			FCSKitDebug_ScreenWindowText ScreenWindowText;
			ScreenWindowText.SetWindowFrameColor(FLinearColor::White);
			ScreenWindowText.AddText(FString::Printf(TEXT("Begin : %s"), *Edge.mBegin.ToString()));
			ScreenWindowText.AddText(FString::Printf(TEXT("End : %s"), *Edge.mEnd.ToString()));
			ScreenWindowText.AddText(FString::Printf(TEXT("TileIndex : %d"), Edge.mTileIndex));
			ScreenWindowText.AddText(FString::Printf(TEXT("UID : %d"), Edge.mUID));
			ScreenWindowText.Draw(InCanvas, Edge.GetCenter(), 500.f);

			DrawDebugLine(GetWorld(), Edge.mBegin, Edge.GetCenter(), FColor::Blue);
			DrawDebugLine(GetWorld(), Edge.GetCenter(), Edge.mEnd, FColor::Orange);
			DrawDebugLine(GetWorld(), Edge.GetCenter(), Edge.GetCenter()+Edge.mOutsideNV*50.f, FColor::White);
		}
	}
}
#endif
