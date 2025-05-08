// Copyright 2020 megasuraman
/**
 * @file CSKit_Math.h
 * @brief 計算関数等のまとめ
 * @author megasuraman
 * @date 2022/3/19
 */

#pragma once

#include "CoreMinimal.h"
#include "CSKit_Math.generated.h"

class APawn;
struct FEnvQueryInstance;


UCLASS()
class CSKIT_API UCSKit_Math : public UObject
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "CSKit_Math")
	static float CalcSimpleEaseInOut(float InRatio);
	
	static FVector CalcQuadraticBezier(const FVector& InPosA, const FVector& InPosB, const FVector& InPosC, float InRatio);

	struct FCapsule
	{
		FVector mPos = FVector::ZeroVector;
		FRotator mRot = FRotator::ZeroRotator;
		float	mHalfHeight = 100.f;
		float	mRadius = 50.f;
		FCapsule() {};
		FCapsule(const FVector& InPos, const FRotator& InRot, const float InHalfHeight, const float InRadius)
			:mPos(InPos)
			, mRot(InRot)
			, mHalfHeight(InHalfHeight)
			, mRadius(InRadius)
		{}
		void	Draw(const UWorld* InWorld, const FColor InColor) const;
	};
	static bool	CapsuleCapsuleIntersection(const FCapsule& InBaseCapsule, const FCapsule& InTargetCapsule);

	struct FTriangle
	{
		static constexpr int32 sTrianglePointNum = 3;
		FVector	mPointList[sTrianglePointNum];
		FTriangle()
		{
			mPointList[0] = FVector::ZeroVector;
			mPointList[1] = FVector(100.f, 0.f, 0.f);
			mPointList[2] = FVector(0.f, 100.f, 0.f);
		};
		FTriangle(const FVector& InA, const FVector& InB, const FVector& InC)
		{
			mPointList[0] = InA;
			mPointList[1] = InB;
			mPointList[2] = InC;
		}
		void	Draw(const UWorld* InWorld, const FColor InColor) const;
	};
	static bool	CapsuleTriangleIntersection(const FCapsule& InCapsule, const FTriangle& InTriangle);

	static bool CapsuleSweepCapsuleIntersection(const FCapsule& InFixCapsule, const FCapsule& InSweepCapsuleStart, const FCapsule& InSweepCapsuleEnd);
};
