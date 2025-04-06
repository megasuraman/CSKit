// Copyright 2020 megasuraman
/**
 * @file CSKit_NavigationSystem.cpp
 * @brief 
 * @author megasuraman
 * @date 2025/04/06
 */
#include "NavigationSystem/CSKit_NavigationSystem.h"

#include "NavigationSystem/CSKit_RecastNavMesh.h"

UCSKit_NavigationSystem::UCSKit_NavigationSystem(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, mbRequestCheckFirstCreate(false)
	, mbFinishedFirstCreate(false)
{
}

/* ------------------------------------------------------------
   !
------------------------------------------------------------ */
void UCSKit_NavigationSystem::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if(mbRequestCheckFirstCreate)
	{
		UpdateCheckFirstCreate(DeltaSeconds);
	}
}

/* ------------------------------------------------------------
   !
------------------------------------------------------------ */
void UCSKit_NavigationSystem::RequestRebuild()
{
	for (ANavigationData* NavData : NavDataSet)
	{
		if (ACSKit_RecastNavMesh* CSKit_RecastNavMesh = Cast<ACSKit_RecastNavMesh>(NavData))
		{
			CSKit_RecastNavMesh->RequestRebuild();
		}
	}
	RequestCheckFirstCreate();
}
/* ------------------------------------------------------------
   !Navmesh生成終了チェック開始(ステージロード後必須)
------------------------------------------------------------ */
void UCSKit_NavigationSystem::RequestCheckFirstCreate()
{
	mbRequestCheckFirstCreate = true;
	mbFinishedFirstCreate = false;
#if USE_CSKIT_DEBUG
	mDebugFirstCreateCheckTotalSec = 0.f;
#endif
}

/* ------------------------------------------------------------
   !
------------------------------------------------------------ */
void UCSKit_NavigationSystem::UpdateCheckFirstCreate(float DeltaSeconds)
{
#if USE_CSKIT_DEBUG
	mDebugFirstCreateCheckTotalSec += DeltaSeconds;
#endif

	if (GetNumRunningBuildTasks() > 0
		|| GetNumRemainingBuildTasks() > 0)
	{
		mFirstCreateNoTaskSec = 0.f;
		return;
	}
	mFirstCreateNoTaskSec += DeltaSeconds;
	if (mFirstCreateNoTaskSec < 1.f)
	{
		return;
	}

	mbFinishedFirstCreate = true;
	mbRequestCheckFirstCreate = false;
}

/* ------------------------------------------------------------
   !Stageロード完了時
------------------------------------------------------------ */
void UCSKit_NavigationSystem::OnLoadedStage()
{
	RequestCheckFirstCreate();
}

#if USE_CSKIT_DEBUG
/* ------------------------------------------------------------
   !
------------------------------------------------------------ */
void UCSKit_NavigationSystem::DebugRequestDrawSafeNavMesh(const bool bInDraw)
{
	for (ANavigationData* NavData : NavDataSet)
	{
		if (ACSKit_RecastNavMesh* CSKit_RecastNavMesh = Cast<ACSKit_RecastNavMesh>(NavData))
		{
			CSKit_RecastNavMesh->DebugRequestDrawSafeNavMesh(bInDraw);
		}
	}
}

/* ------------------------------------------------------------
   !
------------------------------------------------------------ */
void UCSKit_NavigationSystem::DebugResetCheckFirstCreate()
{
	mbRequestCheckFirstCreate = false;
	mbFinishedFirstCreate = false;
}
#endif

#if WITH_EDITOR
/* ------------------------------------------------------------
   !
------------------------------------------------------------ */
void UCSKit_NavigationSystem::EditorRequestGenerateNavLink()
{
	for (ANavigationData* NavData : NavDataSet)
	{
		if (ACSKit_RecastNavMesh* CSKit_RecastNavMesh = Cast<ACSKit_RecastNavMesh>(NavData))
		{
			CSKit_RecastNavMesh->EditorGenerateNavLink();
		}
	}
}

/* ------------------------------------------------------------
   !
------------------------------------------------------------ */
bool UCSKit_NavigationSystem::EditorIsGeneratingNavLink() const
{
	for (const ANavigationData* NavData : NavDataSet)
	{
		if (const ACSKit_RecastNavMesh* CSKit_RecastNavMesh = Cast<ACSKit_RecastNavMesh>(NavData))
		{
			if(CSKit_RecastNavMesh->EditorIsGeneratingNavLink())
			{
				return true;
			}
		}
	}
	return false;
}
#endif