// Copyright 2020 megasuraman.
#include "SoftBody/CSKit_SoftBodySolver.h"

#include "DrawDebugHelpers.h"

FCSKit_SoftBodyPoint::FCSKit_SoftBodyPoint(const FVector &InLocalPos, const FTransform &InBaseTransform)
	:mLocalPos(InLocalPos)
{
	mPos = InBaseTransform.TransformPosition(InLocalPos);
	mOriginalPos = mPos;
}

/**
 * @brief 
 */
void CSKit_SoftBodySolver::SetSpringSetting(const FCSKit_SoftBodySpringSetting &InSpringSetting)
{
	mSpringSetting = InSpringSetting;
}

/**
 * @brief 頂点情報設定
 */
void CSKit_SoftBodySolver::SetPointList(const TArray<FCSKit_SoftBodyPoint> &InPoints)
{
	mPointList = InPoints;
	mBaseVolume = CalcVolumeFake();
}
void CSKit_SoftBodySolver::SetPointList(const TArray<FCSKit_SoftBodyPoint> &InPoints, const TArray<FCSKit_SoftBodyPointLink> &InLinks)
{
	mPointList = InPoints;
	mBaseVolume = CalcVolumeFake();
	mLinkList = InLinks;
}

/**
 * @brief 頂点更新
 */
void CSKit_SoftBodySolver::Update(const UWorld * InWorld, const float InDeltaTime)
{
	UpdatePressureForce();
	UpdateShapeForce();
	UpdateLinkForce();
	UpdatePointPos(InWorld, InDeltaTime);
	ResetForce();
}

/**
 * @brief 全体の基準姿勢指定
 */
void CSKit_SoftBodySolver::SetBaseTransform(const FTransform &InBaseTransform)
{
	for (FCSKit_SoftBodyPoint &Point : mPointList)
	{
		Point.mOriginalPos = InBaseTransform.TransformPosition(Point.mLocalPos);
	}
}

/**
 * @brief 頂点に力を加える
 */
void CSKit_SoftBodySolver::AddForce(const FVector &InForce)
{
	for (FCSKit_SoftBodyPoint &Point : mPointList)
	{
		Point.mForce += InForce;
	}
}
void CSKit_SoftBodySolver::AddForce(const FVector &InForce, const int32 InPointIndex)
{
	if (InPointIndex >= mPointList.Num())
	{
		return;
	}
	mPointList[InPointIndex].mForce += InForce;
}
/**
 * @brief 体積値を嘘計算
 */
float CSKit_SoftBodySolver::CalcVolumeFake() const
{
	FVector Centroid = FVector::ZeroVector;
	for (const FCSKit_SoftBodyPoint &Point : mPointList)
	{
		Centroid += Point.mPos;
	}
	if (mPointList.Num() > 0)
	{
		Centroid /= mPointList.Num();
	}
	float Volume = 0.f;
	for (const FCSKit_SoftBodyPoint &Point : mPointList)
	{
		Volume += FVector::Dist(Point.mPos, Centroid);
	}
	Volume /= mPointList.Num();
	return Volume;
}
/**
 * @brief 圧力モデルによる力を適用
 */
void CSKit_SoftBodySolver::UpdatePressureForce()
{
	const float Volume = CalcVolumeFake();
	const float PressureForce = mSpringSetting.mPressureStiffness * (mBaseVolume - Volume);
	// 重心
	FVector Centroid = FVector::ZeroVector;
	for (FCSKit_SoftBodyPoint &Point : mPointList)
	{
		Centroid += Point.mPos;
	}
	if (mPointList.Num() > 0)
	{
		Centroid /= mPointList.Num();
	}
	
	for (FCSKit_SoftBodyPoint &Point : mPointList)
	{
		// 外向きに圧力をかける
		const FVector DirectionFromCenter = (Point.mPos - Centroid).GetSafeNormal();
		Point.mForce += DirectionFromCenter * PressureForce;
	}
}
/**
 * @brief 各頂点の減衰力、復元力更新
 */
void CSKit_SoftBodySolver::UpdateShapeForce()
{
	for (FCSKit_SoftBodyPoint &Point : mPointList)
	{
		Point.mForce += -mSpringSetting.mDamping * Point.mSpeed;
		
		//元の位置に戻ろうとする力
		FVector Displacement = Point.mPos - Point.mOriginalPos;
		Point.mForce += -mSpringSetting.mShapeStiffness * Displacement;
		
		// 重力を与えたい場合はここで追加します
		// Point.mForce += FVector(0, 0, -980.0f) * Mass;
	}
}
/**
 * @brief 繋がっている頂点同士のバネの力を適用
 */
void CSKit_SoftBodySolver::UpdateLinkForce()
{
	for (const FCSKit_SoftBodyPointLink& Link : mLinkList)
	{
		FVector PosA = mPointList[Link.mIndexA].mPos;
		FVector PosB = mPointList[Link.mIndexB].mPos;

		FVector Delta = PosB - PosA;
		float CurrentDist = Delta.Size();

		// 距離が0によるゼロ除算を防ぐ
		if (CurrentDist > 0.001f)
		{
			FVector Direction = Delta / CurrentDist; // AからBへの向き
            
			// フックの法則: F = k * (L - BaseL)
			float SpringForceScalar = mSpringSetting.mLinkStiffness * (CurrentDist - Link.mBaseLength);
			FVector ForceOnA = Direction * SpringForceScalar;

			// AはBに引っ張られ、BはAに引っ張られる（作用・反作用）
			mPointList[Link.mIndexA].mForce += ForceOnA;
			mPointList[Link.mIndexB].mForce -= ForceOnA;
		}
	}
}
void CSKit_SoftBodySolver::UpdatePointPos(const UWorld *InWorld, const float InDeltaTime)
{
	// 速度と位置を更新
	for (int32 i=0; i<mPointList.Num(); ++i)
	{
		constexpr float MaxVelocity = 3000.0f;
		FCSKit_SoftBodyPoint &Point = mPointList[i];
		if (Point.mbIsPinned)
		{
			continue;
		}

		FVector Acceleration = Point.mForce / FMath::Max(mSpringSetting.mMass, 0.001f);
		Point.mSpeed += Acceleration * InDeltaTime;
		if (Point.mSpeed.SizeSquared() > FMath::Square(MaxVelocity))
		{
			Point.mSpeed = Point.mSpeed.GetSafeNormal() * MaxVelocity;
		}
		FVector PointPos = Point.mPos + Point.mSpeed * InDeltaTime;
		
		FHitResult HitResult;
		FCollisionQueryParams Params;
            
		if (InWorld->LineTraceSingleByChannel(
		    HitResult, 
		    Point.mPos, 
		    PointPos, 
		    mCollisionChannel, 
		    Params) )
		{
			//少し押し返した位置
			PointPos = HitResult.ImpactPoint + HitResult.ImpactNormal * 2.0f;
			
			const FVector VelocityIntoWall = Point.mSpeed.ProjectOnToNormal(HitResult.ImpactNormal);
			Point.mSpeed -= VelocityIntoWall;
        
			// 跳ね返らせたい場合
			// Point.mSpeed -= VelocityIntoWall * 1.5f;
		}
		
		Point.mPos = PointPos;
	}
}
/**
 * @brief 力リセット
 */
void CSKit_SoftBodySolver::ResetForce()
{
	for (FCSKit_SoftBodyPoint &Point : mPointList)
	{
		Point.mForce = FVector::ZeroVector;
	}
}

#if !UE_BUILD_SHIPPING
/**
 * @brief デバッグ表示
 */
void CSKit_SoftBodySolver::DebugDraw(const UWorld * InWorld, UCanvas *InCanvas) const
{
	for (const FCSKit_SoftBodyPointLink &Link : mLinkList)
	{
		DrawDebugLine(InWorld, mPointList[Link.mIndexA].mPos, mPointList[Link.mIndexB].mPos, FColor::Orange, false, 0.0f, 0.0f, 2.0f);
	}
	for (const FCSKit_SoftBodyPoint &Point : mPointList)
	{
		DrawDebugPoint(InWorld, Point.mOriginalPos, 5.0f, FColor::White);
		DrawDebugPoint(InWorld, Point.mPos, 20.0f, FColor::Blue);
	}
}
#endif