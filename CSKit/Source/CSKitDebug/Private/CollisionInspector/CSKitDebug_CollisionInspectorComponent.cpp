// Copyright 2020 megasuraman
/**
 * @file CSKitDebug_CollisionInspectorComponent.cpp
 * @brief コリジョンチェック用Component
 * @author megasuraman
 * @date 2025/12/12
 */

#include "CollisionInspector/CSKitDebug_CollisionInspectorComponent.h"

#include "CSKitDebug_Draw.h"
#include "CSKitDebug_Utility.h"
#include "DrawDebugHelpers.h"
#include "CollisionInspector/CSKitDebug_CollisionInspectorManager.h"
#include "DebugMenu/CSKitDebug_DebugMenuManager.h"
#include "ScreenWindow/CSKitDebug_ScreenWindowText.h"

UCSKitDebug_CollisionInspectorComponent::UCSKitDebug_CollisionInspectorComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

/**
 * @brief	
 */
void UCSKitDebug_CollisionInspectorComponent::BeginPlay()
{
	Super::BeginPlay();
	
	if (UCSKitDebug_CollisionInspectorManager* CollisionInspectorManager = UCSKitDebug_CollisionInspectorManager::sGet(GetWorld()))
	{
		CollisionInspectorManager->Entry(this);
	}
	
	if (const AActor* OwnerActor = GetOwner())
	{
		mLastSafeTransform = OwnerActor->GetActorTransform();
	}
	mVoxelChunk.InitVoxel();
	mVoxelChunk.SetTransform(mLastSafeTransform);
}

/**
 * @brief	
 */
void UCSKitDebug_CollisionInspectorComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	
	if (UCSKitDebug_CollisionInspectorManager* CollisionInspectorManager = UCSKitDebug_CollisionInspectorManager::sGet(GetWorld()))
	{
		CollisionInspectorManager->Exit(this);
	}
}

/**
 * @brief	
 */
void UCSKitDebug_CollisionInspectorComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	UpdateLastSafeTransform();
	if (mbGetLastSafeTransform)
	{
		UpdateVoxel(mVoxelChunk);
		CheckTooTight(mVoxelChunk);
		CheckNoGround(mVoxelChunk);
	}
}

/**
 * @brief	
 */
void UCSKitDebug_CollisionInspectorComponent::DebugDraw(UCanvas* InCanvas) const
{
	if (UCSKitDebug_DebugMenuManager::sGetNodeValue_Bool(this, FString(TEXT("CSKitDebug/CollisionInspector/DrawLastSafePoint"))))
	{
		DrawLastSafeTransform(InCanvas);
	}
	if (UCSKitDebug_DebugMenuManager::sGetNodeValue_Bool(this, FString(TEXT("CSKitDebug/CollisionInspector/DrawVoxel"))))
	{
		DrawVoxel(InCanvas, mVoxelChunk);
	}
	if (UCSKitDebug_DebugMenuManager::sGetNodeValue_Bool(this, FString(TEXT("CSKitDebug/CollisionInspector/DrawError"))))
	{
		mError.Draw(GetWorld(), InCanvas);
	}
}

/**
 * @brief	最終安全地点更新
 */
void UCSKitDebug_CollisionInspectorComponent::UpdateLastSafeTransform()
{
	const AActor* OwnerActor = GetOwner();
	if (OwnerActor == nullptr)
	{
		return;
	}
	if (CheckPassThrough(*OwnerActor))
	{//コリジョン抜けしてる
		mbGetLastSafeTransform = false;
		return;
	}
	if (CheckStuck(*OwnerActor))
	{//Stuckしてる
		mbGetLastSafeTransform = false;
		return;
	}
	
	const FTransform OwnerTransform = OwnerActor->GetActorTransform();
	mLastSafeTransform = OwnerTransform;
	mbGetLastSafeTransform = true;
}

/**
 * @brief	コリジョン抜けチェック
 */
bool UCSKitDebug_CollisionInspectorComponent::CheckPassThrough(const AActor& InOwnerActor)
{
	const FTransform OwnerTransform = InOwnerActor.GetActorTransform();
	{//まずはLineTrace
		FHitResult HitResult;
		if (CheckHitCollision_Line(HitResult, mLastSafeTransform.GetLocation(), OwnerTransform.GetLocation()))
		{
			OnError_PassThrough(HitResult, OwnerTransform);
			return true;
		}
	}
	float CollisionRadius = 0.f;
	float CollisionHalfHeight = 0.f;
	InOwnerActor.GetSimpleCollisionCylinder(CollisionRadius, CollisionHalfHeight);
	if (CollisionRadius > 0.f
		&& CollisionHalfHeight > 0.f)
	{//隙間を通った可能性もあるので球でも判定
		FHitResult HitResult;
		if(CheckHitCollision_Sphere(HitResult, mLastSafeTransform.GetLocation(), OwnerTransform.GetLocation(), CollisionRadius, true))
		{
			OnError_PassThrough(HitResult, OwnerTransform);
			return true;
		}
	}
	
	return false;
}

/**
 * @brief	スタックチェック
 */
bool UCSKitDebug_CollisionInspectorComponent::CheckStuck(const AActor& InOwnerActor)
{
	const FTransform OwnerTransform = InOwnerActor.GetActorTransform();
	float CollisionRadius = 0.f;
	float CollisionHalfHeight = 0.f;
	InOwnerActor.GetSimpleCollisionCylinder(CollisionRadius, CollisionHalfHeight);
	if (CollisionRadius > 0.f
		&& CollisionHalfHeight > 0.f)
	{//現在位置の初期交差チェック
		const FVector OffsetV = OwnerTransform.GetRotation().RotateVector(FVector(0.f,0.f,CollisionHalfHeight*0.5f));
		const FVector BasePos = OwnerTransform.GetLocation() + OffsetV;
		const FVector TargetPos = OwnerTransform.GetLocation() - OffsetV;
		//DrawDebugSphere(GetWorld(), BasePos, CollisionRadius, 32, FColor::Blue, false, -1.f, 0, 2.f);
		FHitResult HitResult;
		if(CheckHitCollision_Sphere(HitResult, BasePos, TargetPos, CollisionRadius, false))
		{
			//DrawDebugSphere(GetWorld(), TargetPos, CollisionRadius, 32, FColor::Red, false, -1.f, 0, 2.f);
			const FVector TargetNV = OffsetV.GetSafeNormal();
			const FVector Center2ImpactNV = FVector(OwnerTransform.GetLocation() - HitResult.ImpactPoint).GetSafeNormal();
			if (FVector::DotProduct(TargetNV, Center2ImpactNV) < 0.f)
			{
				OnError_Stuck(HitResult, OwnerTransform, CollisionRadius);
				return true;
			}
			else
			{
				OnError_Penetrating(HitResult, OwnerTransform, CollisionRadius);
				//軽くめり込んでるだけなのでスルーでいい
			}
		}
		//DrawDebugSphere(GetWorld(), TargetPos, CollisionRadius, 32, FColor::Orange, false, -1.f, 0, 2.f);
	}
	return false;
}

/**
 * @brief	
 */
void UCSKitDebug_CollisionInspectorComponent::UpdateVoxel(FCollisionInspectorVoxelChunk& InVoxelChunk) const
{
	if (!mbGetLastSafeTransform)
	{
		return;
	}
	InVoxelChunk.SetTransform(mLastSafeTransform);
	InVoxelChunk.ClearAllVoxelState();
	TArray<FCollisionInspectorVoxel*> CheckSafeVoxelList;
	const FCollisionInspectorVoxelIndex CenterVoxelIndex(0,0,0);
	if (FCollisionInspectorVoxel* CenterVoxel = InVoxelChunk.FindVoxel(CenterVoxelIndex))
	{
		CenterVoxel->mState = ECollisionInspectorVoxelState::Safe;
		CheckSafeVoxelList.Add(CenterVoxel);
	}
	
	while (CheckSafeVoxelList.Num() != 0)
	{
		FCollisionInspectorVoxel* BaseVoxel = CheckSafeVoxelList[0];

		TArray<FCollisionInspectorVoxelIndex> AroundVoxelIndexList;
		BaseVoxel->GetAroundVoxelList(AroundVoxelIndexList);
		const FVector BasePos = InVoxelChunk.GetVoxelPos(BaseVoxel->mVoxelIndex);
		for (const FCollisionInspectorVoxelIndex& AroundVoxelIndex : AroundVoxelIndexList)
		{
			if (FCollisionInspectorVoxel* AroundVoxel = InVoxelChunk.FindVoxel(AroundVoxelIndex))
			{
				if (AroundVoxel->mState != ECollisionInspectorVoxelState::Invalid)
				{
					continue;
				}
				const FVector TargetPos = InVoxelChunk.GetVoxelPos(AroundVoxelIndex);
				FHitResult HitResult;
				if (CheckHitCollision_Line(HitResult, BasePos, TargetPos))
				{
					AroundVoxel->mState = ECollisionInspectorVoxelState::Hit;
				}
				else
				{
					AroundVoxel->mState = ECollisionInspectorVoxelState::Safe;
					CheckSafeVoxelList.Add(AroundVoxel);
				}
			}
		}
		
		CheckSafeVoxelList.RemoveAt(0);
	}
	
	//InvalidのままのはOutsideにする
	InVoxelChunk.AssignAllInvalidVoxelToOutside();
}

/**
 * @brief	狭すぎるコリジョンエラーチェック
 */
void UCSKitDebug_CollisionInspectorComponent::CheckTooTight(const FCollisionInspectorVoxelChunk& InVoxelChunk)
{
	for (const FCollisionInspectorVoxel& Voxel : InVoxelChunk.GetVoxelList())
	{
		CheckTooTight(InVoxelChunk, Voxel);
	}
}
void UCSKitDebug_CollisionInspectorComponent::CheckTooTight(const FCollisionInspectorVoxelChunk& InVoxelChunk, const FCollisionInspectorVoxel& InVoxel)
{
	//SafeなVoxelがHitに囲まれてる方向を詳細にチェックしてみる
	if (InVoxel.mState != ECollisionInspectorVoxelState::Safe)
	{
		return;
	}
	const FVector BasePos = InVoxelChunk.GetVoxelPos(InVoxel.mVoxelIndex);
	TArray<FCollisionInspectorVoxelIndex> AroundVoxelIndexList;
	InVoxel.GetAroundVoxelList(AroundVoxelIndexList);
	for (const FCollisionInspectorVoxelIndex& AroundVoxelIndex : AroundVoxelIndexList)
	{
		const FCollisionInspectorVoxel* AroundVoxel = InVoxelChunk.FindVoxel(AroundVoxelIndex);
		if (AroundVoxel == nullptr
			|| AroundVoxel->mState != ECollisionInspectorVoxelState::Hit)
		{
			continue;
		}
		
		const int32 AroundIndexShiftX = AroundVoxelIndex.mIndex.mX - InVoxel.mVoxelIndex.mIndex.mX;
		const int32 AroundIndexShiftY = AroundVoxelIndex.mIndex.mY - InVoxel.mVoxelIndex.mIndex.mY;
		const int32 AroundIndexShiftZ = AroundVoxelIndex.mIndex.mZ - InVoxel.mVoxelIndex.mIndex.mZ;
		const int32 OppositeSideIndexX = InVoxel.mVoxelIndex.mIndex.mX - AroundIndexShiftX;
		const int32 OppositeSideIndexY = InVoxel.mVoxelIndex.mIndex.mY - AroundIndexShiftY;
		const int32 OppositeSideIndexZ = InVoxel.mVoxelIndex.mIndex.mZ - AroundIndexShiftZ;
		FCollisionInspectorVoxelIndex OppositeVoxelIndex(OppositeSideIndexX, OppositeSideIndexY, OppositeSideIndexZ);
		const FCollisionInspectorVoxel* OppositeSideVoxel = InVoxelChunk.FindVoxel(OppositeVoxelIndex);
		if (OppositeSideVoxel == nullptr
			|| OppositeSideVoxel->mState != ECollisionInspectorVoxelState::Hit)
		{
			continue;
		}
		const FVector IndexShiftNV = FVector(static_cast<double>(AroundIndexShiftX), static_cast<double>(AroundIndexShiftY), static_cast<double>(AroundIndexShiftZ)).GetSafeNormal();
		const FVector CheckV = InVoxelChunk.GetTransform().TransformVector(IndexShiftNV*InVoxelChunk.GetVoxelLength());
		FHitResult HitResultA;
		if (!CheckHitCollision_Line(HitResultA, BasePos, BasePos+CheckV))
		{
			continue;
		}
		
		const FVector OppositeCheckBeginPos = HitResultA.ImpactPoint + HitResultA.ImpactNormal*10.f;//ちょっと離す
		const FVector OppositeCheckTargetPos = HitResultA.ImpactPoint + HitResultA.ImpactNormal*mTooTightLengthBorder;
		FHitResult HitResultB;
		if (!CheckHitCollision_Line(HitResultB, OppositeCheckBeginPos, OppositeCheckTargetPos))
		{
			continue;
		}
		const float Distance = FVector::Dist(HitResultA.ImpactPoint, HitResultB.ImpactPoint);
		if (Distance > mTooTightLengthBorder)
		{
			continue;
		}
		//法線が全然向き合ってない
		// const float NormalDot = FVector::DotProduct(HitResultA.ImpactNormal, HitResultB.ImpactNormal);
		// if (NormalDot > -0.5f)
		// {
		// 	continue;
		// }
		FCollisionInspectorErrorData_TooTight TooTightInfo;
		TooTightInfo.mCollisionHitDataList.Add(FCollisionInspectorError_CollisionHitData(HitResultA));
		TooTightInfo.mCollisionHitDataList.Add(FCollisionInspectorError_CollisionHitData(HitResultB));
		TooTightInfo.mLength = Distance;
		mError.AddErrorNode(BasePos, TooTightInfo);
	}
}

/**
 * @brief	地面なしエラーチェック
 */
void UCSKitDebug_CollisionInspectorComponent::CheckNoGround(const FCollisionInspectorVoxelChunk& InVoxelChunk)
{
	for (const FCollisionInspectorVoxel& Voxel : InVoxelChunk.GetVoxelList())
	{
		CheckNoGround(InVoxelChunk, Voxel);
	}
}
void UCSKitDebug_CollisionInspectorComponent::CheckNoGround(const FCollisionInspectorVoxelChunk& InVoxelChunk, const FCollisionInspectorVoxel& InVoxel)
{
	//Voxel自体が傾いてることがあるので、SafeなVoxelの直下を調べる形とする
	if (InVoxel.mState != ECollisionInspectorVoxelState::Safe)
	{
		return;
	}
	const FVector BasePos = InVoxelChunk.GetVoxelPos(InVoxel.mVoxelIndex);
	const FVector TargetPos = BasePos - FVector(0.f,0.f,400000.f);
	FHitResult HitResult;
	if (CheckHitCollision_Line(HitResult, BasePos, TargetPos))
	{
		return;
	}
	
	FCollisionInspectorErrorData_NoGround NoGroundInfo;
	NoGroundInfo.mPosString = BasePos.ToString();
	mError.AddErrorNode(BasePos, NoGroundInfo);
}

/**
 * @brief	
 */
bool UCSKitDebug_CollisionInspectorComponent::CheckHitCollision_Line(FHitResult& OutResult, const FVector& InBasePos, const FVector& InTargetPos) const
{
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(GetOwner());
	QueryParams.bTraceComplex = false;
	QueryParams.bIgnoreTouches = true;
	
	return GetWorld()->LineTraceSingleByChannel(
		OutResult,
		InBasePos,
		InTargetPos,
		ECC_WorldStatic,
		QueryParams
	);
}

bool UCSKitDebug_CollisionInspectorComponent::CheckHitCollision_Sphere(
	FHitResult& OutResult,
	const FVector& InBasePos,
	const FVector& InTargetPos,
	const float InRadius,
	const bool bInIgnoreTouches
	) const
{
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(GetOwner());
	QueryParams.bTraceComplex = false;
	QueryParams.bIgnoreTouches = bInIgnoreTouches;
	
	FCollisionShape CollisionShape;
	CollisionShape.SetSphere(InRadius);

	return GetWorld()->SweepSingleByChannel(
		OutResult,
		InBasePos,
		InTargetPos,
		FQuat::Identity,
		ECC_WorldStatic,
		CollisionShape,
		QueryParams
	);
}

/**
 * @brief	コリジョン抜け発生時
 */
void UCSKitDebug_CollisionInspectorComponent::OnError_PassThrough(const FHitResult& InHitResult, const FTransform& InTransform)
{
	if (!mbGetLastSafeTransform)
	{
		return;
	}
	FCollisionInspectorErrorData_PassThrough PassThroughInfo;
	PassThroughInfo.mLastSafePosString = mLastSafeTransform.GetLocation().ToString();
	if (const UPrimitiveComponent* HitComponent = InHitResult.GetComponent())
	{
		PassThroughInfo.mHitCollisionInfo += FString::Printf(TEXT("%s\n"), *UCSKitDebug_Utility::GetObjectDisplayName(HitComponent));
	}
	mError.AddErrorNode(InTransform.GetLocation(), PassThroughInfo);
}

/**
 * @brief	スタック発生時
 */
void UCSKitDebug_CollisionInspectorComponent::OnError_Stuck(const FHitResult& InHitResult, const FTransform& InTransform, const float InRadius)
{
	if (!mbGetLastSafeTransform)
	{
		return;
	}
	FCollisionInspectorErrorData_Stuck StuckInfo;
	StuckInfo.mLastSafePosString = mLastSafeTransform.GetLocation().ToString();
	if (const UPrimitiveComponent* HitComponent = InHitResult.GetComponent())
	{
		StuckInfo.mHitCollisionInfo += FString::Printf(TEXT("%s\n"), *UCSKitDebug_Utility::GetObjectDisplayName(HitComponent));
	}
	StuckInfo.mHitImpactPosString = InHitResult.ImpactPoint.ToString();
	StuckInfo.mCheckPosBeginString = InHitResult.TraceStart.ToString();
	StuckInfo.mCheckPosEndString = InHitResult.TraceEnd.ToString();
	StuckInfo.mCheckRadius = InRadius;
	mError.AddErrorNode(InTransform.GetLocation(), StuckInfo);
}

/**
 * @brief	めり込み発生時
 */
void UCSKitDebug_CollisionInspectorComponent::OnError_Penetrating(const FHitResult& InHitResult, const FTransform& InTransform, const float InRadius)
{
	FCollisionInspectorErrorData_Penetrating PenetratingInfo;
	if (const UPrimitiveComponent* HitComponent = InHitResult.GetComponent())
	{
		PenetratingInfo.mHitCollisionInfo += FString::Printf(TEXT("%s\n"), *UCSKitDebug_Utility::GetObjectDisplayName(HitComponent));
	}
	PenetratingInfo.mHitImpactPosString = InHitResult.ImpactPoint.ToString();
	PenetratingInfo.mCheckPosBeginString = InHitResult.TraceStart.ToString();
	PenetratingInfo.mCheckPosEndString = InHitResult.TraceEnd.ToString();
	PenetratingInfo.mCheckRadius = InRadius;
	mError.AddErrorNode(InTransform.GetLocation(), PenetratingInfo);
}

/**
 * @brief	
 */
void UCSKitDebug_CollisionInspectorComponent::DrawLastSafeTransform(UCanvas* InCanvas) const
{
	const FVector ArrowOffsetV(0.f, 0.f,100.f);
	constexpr float Thickness = 2.f;
	FColor LineColor = FColor::Blue;
	if (!mbGetLastSafeTransform)
	{
		LineColor = FColor::Red;
	}
	const FVector LastSafePos = mLastSafeTransform.GetLocation();
	UCSKitDebug_Draw::OctahedronArrow OctahedronArrow;
	OctahedronArrow.mTargetPos = LastSafePos;
	OctahedronArrow.mBasePos = LastSafePos + ArrowOffsetV;
	OctahedronArrow.Draw(GetWorld(), LineColor, 0, Thickness);

	FCSKitDebug_ScreenWindowText Window;
	Window.SetWindowFrameColor(LineColor);
	Window.SetWindowName(FString(TEXT("LastSafeTransform")));
	Window.AddText(FString::Printf(TEXT("pos : %s"), *mLastSafeTransform.GetLocation().ToString()));
	Window.AddText(FString::Printf(TEXT("rot : %s"), *mLastSafeTransform.GetRotation().ToString()));
	Window.Draw(InCanvas, OctahedronArrow.mBasePos);
}

/**
 * @brief	
 */
void UCSKitDebug_CollisionInspectorComponent::DrawVoxel(UCanvas* InCanvas, const FCollisionInspectorVoxelChunk& InVoxelChunk) const
{
	const float VoxelExtentLen= InVoxelChunk.GetVoxelLength() * 0.5f;
	const float DrawVoxelExtentLen = VoxelExtentLen - 1.f;//重なり回避のために少し小さく
	const FVector DrawVoxelExtentV(DrawVoxelExtentLen, DrawVoxelExtentLen, DrawVoxelExtentLen);

	for (const FCollisionInspectorVoxel& Voxel : InVoxelChunk.GetVoxelList())
	{
		const FVector VoxelPos = InVoxelChunk.GetVoxelPos(Voxel.mVoxelIndex);

		const FColor DrawColor = Voxel.GetColor();
		DrawDebugBox(GetWorld(), VoxelPos, DrawVoxelExtentV, InVoxelChunk.GetTransform().Rotator().Quaternion(), DrawColor);
		
		FCSKitDebug_ScreenWindowText Window;
		Window.SetWindowName(FString::Printf(TEXT("%d, %d, %d"), Voxel.mVoxelIndex.mIndex.mX, Voxel.mVoxelIndex.mIndex.mY, Voxel.mVoxelIndex.mIndex.mZ));
		Window.SetWindowFrameColor(DrawColor);
		Window.AddText(FString::Printf(TEXT("mState : %s"), *Voxel.GetStateString()));
		Window.Draw(InCanvas, VoxelPos, 200.f);
	}
}