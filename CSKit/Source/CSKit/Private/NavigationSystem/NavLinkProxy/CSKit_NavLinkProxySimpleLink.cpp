// Copyright 2020 megasuraman.
/**
 * @file CSKit_NavLinkProxySimpleLink.cpp
 * @brief 
 * @author megasuraman
 * @date 2025/04/06
 */
#include "NavigationSystem/NavLinkProxy/CSKit_NavLinkProxySimpleLink.h"

#include "DrawDebugHelpers.h"
#include "NavigationSystem.h"
#include "AI/AISystemBase.h"
#include "Engine/Canvas.h"
#include "NavigationSystem/NavArea/CSKit_NavAreaBase.h"

#if USE_CSKIT_DEBUG
#include "ScreenWindow/CSKitDebug_ScreenWindowText.h"
#endif

ACSKit_NavLinkProxySimpleLink::ACSKit_NavLinkProxySimpleLink(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bSmartLinkIsRelevant = false;
	bNetLoadOnClient = false;
}

void ACSKit_NavLinkProxySimpleLink::PostInitProperties()
{
	Super::PostInitProperties();

	bNetLoadOnClient = false;
#if USE_CSKIT_DEBUG
	if (UAISystemBase::ShouldInstantiateInNetMode(ENetMode::NM_Client))
	{
		bNetLoadOnClient = true;
	}
#endif
}

#if USE_CSKIT_DEBUG
/* ------------------------------------------------------------
   !
------------------------------------------------------------ */
FVector ACSKit_NavLinkProxySimpleLink::DebugGetGroundPos(const UWorld* InWorld, const FVector& InPos)
{
	const FVector CheckOffsetUp(0.f, 0.f, 100.f);
	const FVector CheckOffsetDown(0.f, 0.f, -100.f);
	FHitResult Result;
	if (InWorld->LineTraceSingleByChannel(Result, InPos + CheckOffsetUp, InPos + CheckOffsetDown, ECC_Pawn))
	{
		FVector GroundPos = Result.ImpactPoint;
		GroundPos.Z += 20.f;
		return GroundPos;
	}
	return InPos;
}

/* ------------------------------------------------------------
   !
------------------------------------------------------------ */
void	ACSKit_NavLinkProxySimpleLink::DebugAssignLink(const TArray<FDebugPointLink>& InList)
{
	PointLinks.Empty();

	for (const FDebugPointLink& PointLink : InList)
	{
		FNavigationLink Link;
		Link.Left = PointLink.mLeft;
		Link.Right = PointLink.mRight;
		Link.SetAreaClass(DebugGetAssignNavArea());
		Link.Direction = DebugGetAssignDirection();
		PointLinks.Add(Link);
	}

	if (FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld()) != nullptr)
	{//UE5.5になって、これがないとNavMesh側が認知できなくなったっぽい
		FNavigationSystem::UpdateActorData(*this);
	}
}
/* ------------------------------------------------------------
   !
------------------------------------------------------------ */
void	ACSKit_NavLinkProxySimpleLink::DebugDraw(UCanvas* InCanvas) const
{
	const FTransform& ActorTransform = GetTransform();
	const FVector UpOffsetV(0.f, 0.f, 50.f);
	const FColor Color = FColor::White;
	int32 Index = 0;
	for (const FNavigationLink& Link : PointLinks)
	{
		const FVector LeftPos = ActorTransform.TransformPosition(Link.Left);
		const FVector RightPos = ActorTransform.TransformPosition(Link.Right);
		DrawDebugLine(GetWorld(), LeftPos, LeftPos + UpOffsetV, Color);
		DrawDebugLine(GetWorld(), RightPos, RightPos + UpOffsetV, Color);
		DrawDebugLine(GetWorld(), LeftPos + UpOffsetV, RightPos + UpOffsetV, Color);

		DrawDebugPoint(GetWorld(), LeftPos, 10.f, Color);
		DrawDebugPoint(GetWorld(), RightPos, 10.f, Color);

		const FVector CenterPos = (LeftPos + RightPos)*0.5f + UpOffsetV;
		FCSKitDebug_ScreenWindowText DebugInfoWindow;
		DebugInfoWindow.SetWindowFrameColor(Color);
		//DebugInfoWindow.AddText(FString::Printf(TEXT("UnitName : %s"), *UnitName));
		DebugInfoWindow.AddText(FString::Printf(TEXT("Index : %d"), Index));
		if (DebugInfoWindow.Draw(InCanvas, CenterPos, 500.f).IsZero())
		{
			if (InCanvas->SceneView->ViewFrustum.IntersectSphere(CenterPos, 1.0f))
			{
				constexpr uint32 WindowPointListSize = 4;
				constexpr float ScreenExtent = 10.f;
				const FVector ScreenPos = InCanvas->Project(CenterPos);
				const FVector2D WindowPointList[WindowPointListSize] = {
					FVector2D(ScreenPos.X, ScreenPos.Y),//左上
					FVector2D(ScreenPos.X, ScreenPos.Y + ScreenExtent),//左下
					FVector2D(ScreenPos.X + ScreenExtent, ScreenPos.Y + ScreenExtent),//右下
					FVector2D(ScreenPos.X + ScreenExtent, ScreenPos.Y)//右上
				};
				for (uint32 i = 0; i < WindowPointListSize; ++i)
				{//枠
					DrawDebugCanvas2DLine(InCanvas, WindowPointList[i], WindowPointList[(i + 1) % WindowPointListSize], Color);
				}
			}
		}
		++Index;
	}
}
/* ------------------------------------------------------------
   !
------------------------------------------------------------ */
UClass* ACSKit_NavLinkProxySimpleLink::DebugGetAssignNavArea() const
{
	return UCSKit_NavAreaBase::StaticClass();
}

/* ------------------------------------------------------------
   !
------------------------------------------------------------ */
ENavLinkDirection::Type ACSKit_NavLinkProxySimpleLink::DebugGetAssignDirection() const
{
	return ENavLinkDirection::BothWays;
}
#endif
