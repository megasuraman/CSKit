// Copyright 2020 megasuraman
/**
 * @file CSKit_Math.cpp
 * @brief 計算関数等のまとめ
 * @author megasuraman
 * @date 2022/3/19
 */
#include "CSKit_Math.h"

#include "DrawDebugHelpers.h"

 /**
  * @brief	3次式の単純な補間関数(FMath::InterpEaseInOut<float>(0, 1, InRatio, 2.0)に近い)
  */
float UCSKit_Math::CalcSimpleEaseInOut(float InRatio)
{
	InRatio = FMath::Clamp(InRatio, 0.f, 1.f);
	return (3.f - 2.f * InRatio) * FMath::Square(InRatio);
}

/**
 * @brief	PosA-PosB, PosB-PosCによる2次ベジェ曲線の座標取得(PosBが制御点)
 */
FVector UCSKit_Math::CalcQuadraticBezier(const FVector& InPosA, const FVector& InPosB, const FVector& InPosC, float InRatio)
{
	const FVector PosAB = FMath::Lerp(InPosA, InPosB, InRatio);
	const FVector PosBC = FMath::Lerp(InPosB, InPosC, InRatio);
	const FVector PosAB_BC = FMath::Lerp(PosAB, PosBC, InRatio);
	return PosAB_BC;
}



void UCSKit_Math::FCapsule::Draw(const UWorld* InWorld, const FColor InColor) const
{
	DrawDebugCapsule(InWorld, mPos, mHalfHeight, mRadius, mRot.Quaternion(), InColor);
}

void UCSKit_Math::FTriangle::Draw(const UWorld* InWorld, const FColor InColor) const
{
	for (int32 i = 0; i < sTrianglePointNum; ++i)
	{
		const FVector BasePos = mPointList[i];
		const FVector NextPos = mPointList[(i + 1) % sTrianglePointNum];
		DrawDebugLine(InWorld, BasePos, NextPos, InColor);
	}
}

/* ------------------------------------------------------------
   !カプセル同士の衝突判定(Engine側にあるはずだけど見つからないので)
------------------------------------------------------------ */
bool UCSKit_Math::CapsuleCapsuleIntersection(const FCapsule& InBaseCapsule, const FCapsule& InTargetCapsule)
{
	const float BaseCapsuleInsideHalfHeight = FMath::Max(InBaseCapsule.mHalfHeight - InBaseCapsule.mRadius, 0.f);
	const FVector BaseCapsuleInsideHalfHeightV = InBaseCapsule.mRot.RotateVector(FVector(0.f, 0., BaseCapsuleInsideHalfHeight));
	const FVector BaseCapsuleInsidePosTop = InBaseCapsule.mPos + BaseCapsuleInsideHalfHeightV;
	const FVector BaseCapsuleInsidePosBottom = InBaseCapsule.mPos - BaseCapsuleInsideHalfHeightV;

	const float TargetCapsuleInsideHalfHeight = FMath::Max(InTargetCapsule.mHalfHeight - InTargetCapsule.mRadius, 0.f);
	const FVector TargetCapsuleInsideHalfHeightV = InTargetCapsule.mRot.RotateVector(FVector(0.f, 0., TargetCapsuleInsideHalfHeight));
	const FVector TargetCapsuleInsidePosTop = InTargetCapsule.mPos + TargetCapsuleInsideHalfHeightV;
	const FVector TargetCapsuleInsidePosBottom = InTargetCapsule.mPos - TargetCapsuleInsideHalfHeightV;

	FVector NearBasePos = FVector::ZeroVector;
	FVector NearTargetPos = FVector::ZeroVector;
	FMath::SegmentDistToSegmentSafe(
		BaseCapsuleInsidePosBottom,
		BaseCapsuleInsidePosTop,
		TargetCapsuleInsidePosBottom,
		TargetCapsuleInsidePosTop,
		NearBasePos,
		NearTargetPos
	);
	if (FVector::DistSquared(NearBasePos, NearTargetPos) < FMath::Square(InBaseCapsule.mRadius + InTargetCapsule.mRadius))
	{
		return true;
	}
	return false;
}

/* ------------------------------------------------------------
   !三角形とカプセルの衝突判定(Engine側にあるはずだけど見つからないので)
------------------------------------------------------------ */
bool UCSKit_Math::CapsuleTriangleIntersection(const FCapsule& InCapsule, const FTriangle& InTriangle)
{
	const FVector CapsuleHalfHeightV = InCapsule.mRot.RotateVector(FVector(0.f, 0.f, InCapsule.mHalfHeight));
	const FVector CapsulePosTop = InCapsule.mPos + CapsuleHalfHeightV;
	const FVector CapsulePosBottom = InCapsule.mPos - CapsuleHalfHeightV;

	FVector IntersectPoint = FVector::ZeroVector;
	FVector IntersectNormal = FVector::UpVector;
	const bool bHit = FMath::SegmentTriangleIntersection(
		CapsulePosBottom,
		CapsulePosTop,
		InTriangle.mPointList[0], InTriangle.mPointList[1], InTriangle.mPointList[2],
		IntersectPoint,
		IntersectNormal
	);

	if (bHit)
	{
		return true;
	}

	const float CapsuleInsideHalfHeight = FMath::Max(InCapsule.mHalfHeight - InCapsule.mRadius, 0.f);
	const FVector CapsuleInsideHalfHeightV = InCapsule.mRot.RotateVector(FVector(0.f, 0., CapsuleInsideHalfHeight));
	const FVector CapsuleInsidePosTop = InCapsule.mPos + CapsuleInsideHalfHeightV;
	const FVector CapsuleInsidePosBottom = InCapsule.mPos - CapsuleInsideHalfHeightV;

	const FVector CapsuleInsideTopClosestTrianglePos = FMath::ClosestPointOnTriangleToPoint(
		CapsulePosTop, InTriangle.mPointList[0], InTriangle.mPointList[1], InTriangle.mPointList[2]
	);
	if (FVector::DistSquared(CapsuleInsideTopClosestTrianglePos, CapsuleInsidePosTop) < InCapsule.mRadius)
	{
		return true;
	}

	const FVector CapsuleInsideBottomClosestTrianglePos = FMath::ClosestPointOnTriangleToPoint(
		CapsulePosBottom, InTriangle.mPointList[0], InTriangle.mPointList[1], InTriangle.mPointList[2]
	);
	if (FVector::DistSquared(CapsuleInsideBottomClosestTrianglePos, CapsuleInsidePosBottom) < InCapsule.mRadius)
	{
		return true;
	}

	for (int32 i = 0; i < FTriangle::sTrianglePointNum; ++i)
	{
		const FVector BasePos = InTriangle.mPointList[i];
		const FVector NextPos = InTriangle.mPointList[(i + 1) % FTriangle::sTrianglePointNum];
		FVector NearPosCapsule = FVector::ZeroVector;
		FVector NearPosTriangle = FVector::ZeroVector;
		FMath::SegmentDistToSegmentSafe(CapsuleInsidePosBottom, CapsuleInsidePosTop, BasePos, NextPos, NearPosCapsule, NearPosTriangle);
		if (FVector::DistSquared(NearPosCapsule, NearPosTriangle) < FMath::Square(InCapsule.mRadius))
		{
			return true;
		}
	}
	return false;
}

/* ------------------------------------------------------------
   !移動カプセルとカプセルとの衝突判定(Engine側にあるはずだけど見つからないので)
------------------------------------------------------------ */
bool UCSKit_Math::CapsuleSweepCapsuleIntersection(
	const FCapsule& InFixCapsule,
	const FCapsule& InSweepCapsuleStart,
	const FCapsule& InSweepCapsuleEnd
)
{
	FCapsule CheckCapsule = InFixCapsule;
	CheckCapsule.mRadius += InSweepCapsuleStart.mRadius;

	const float StartCapsuleInsideHalfHeight = FMath::Max(InSweepCapsuleStart.mHalfHeight - InSweepCapsuleStart.mRadius, 0.f);
	const FVector StartCapsuleInsideHalfHeightV = InSweepCapsuleStart.mRot.RotateVector(FVector(0.f, 0., StartCapsuleInsideHalfHeight));
	const FVector StartCapsuleInsidePosTop = InSweepCapsuleStart.mPos + StartCapsuleInsideHalfHeightV;
	const FVector StartCapsuleInsidePosBottom = InSweepCapsuleStart.mPos - StartCapsuleInsideHalfHeightV;

	const float EndCapsuleInsideHalfHeight = FMath::Max(InSweepCapsuleEnd.mHalfHeight - InSweepCapsuleEnd.mRadius, 0.f);
	const FVector EndCapsuleInsideHalfHeightV = InSweepCapsuleEnd.mRot.RotateVector(FVector(0.f, 0., EndCapsuleInsideHalfHeight));
	const FVector EndCapsuleInsidePosTop = InSweepCapsuleEnd.mPos + EndCapsuleInsideHalfHeightV;
	const FVector EndCapsuleInsidePosBottom = InSweepCapsuleEnd.mPos - EndCapsuleInsideHalfHeightV;

	const FTriangle TriangleA(StartCapsuleInsidePosTop, StartCapsuleInsidePosBottom, EndCapsuleInsidePosTop);
	if (CapsuleTriangleIntersection(CheckCapsule, TriangleA))
	{
		return true;
	}

	const FTriangle TriangleB(EndCapsuleInsidePosTop, EndCapsuleInsidePosBottom, StartCapsuleInsidePosBottom);
	if (CapsuleTriangleIntersection(CheckCapsule, TriangleB))
	{
		return true;
	}

	return  false;
}