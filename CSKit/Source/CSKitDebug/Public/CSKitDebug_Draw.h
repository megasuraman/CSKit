// Copyright 2020 megasuraman
/**
 * @file CSKitDebug_Draw.h
 * @brief Debug表示用のまとめクラス
 * @author megasuraman
 * @date 2022/5/12
 */

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CSKitDebug_Draw.generated.h"

class UCanvas;
class AAIController;

/**
 * 
 */
UCLASS()
class CSKITDEBUG_API UCSKitDebug_Draw : public UObject
{
	GENERATED_BODY()

#if USE_CSKIT_DEBUG
public:
	//八面体矢印線
	struct CSKITDEBUG_API OctahedronArrow
	{
		FVector	mBasePos = FVector::ZeroVector;
		FVector mTargetPos = FVector::ZeroVector;
		float	mRadius = 10.f;
		float	mQadCenterRatio = 0.25f;
		void	Draw(UWorld* InWorld, const FColor& InColor, const uint8 InDepthPriority = 0, const float InThickness = 0.f) const;
	};

	//扇形
	struct CSKITDEBUG_API FanShape
	{
		FVector mPos = FVector::ZeroVector;
		FRotator mRot = FRotator::ZeroRotator;
		float mRadius = 1000.f;
		float mAngle = 45.f;
		uint32 mEdgePointNum = 16;

		virtual void	Draw(UWorld* InWorld, const FColor& InColor, const uint8 InDepthPriority = 0, const float InThickness = 0.f) const;
	};
	//先端を削った扇形
	struct CSKITDEBUG_API FanShapeClipTip : public FanShape
	{
		float mNearClipRadius = 300.f;

		virtual void	Draw(UWorld* InWorld, const FColor& InColor, const uint8 InDepthPriority = 0, const float InThickness = 0.f) const override;
	};

	static void DrawBrushMesh(const UWorld* InWorld, const ABrush* InBrush, const FColor InColor);
	static void DrawBrushWire(const UWorld* InWorld, const ABrush* InBrush, const FColor InColor, const uint8 InDepthPriority=0, const float InThickness=0.f, const float InLifeTime=-1.f);
	
	static void DrawPathFollowRoute(UWorld* InWorld, UCanvas* InCanvas, const AAIController* InAIController, const bool bInShowDetail);
	static void DrawLastEQS(UWorld* InWorld, UCanvas* InCanvas, const AAIController* InAIController, const float InShowDetailDistance=500.f);

	static void DrawCanvasQuadrangle(UCanvas* InCanvas, const FVector2D& InCenterPos, const FVector2D& InExtent, const FLinearColor InColor);
	static void DrawCanvasQuadrangle(UCanvas* InCanvas, const FVector& InPos, const FVector2D& InExtent, const FLinearColor InColor);

#endif//USE_CSKIT_DEBUG
};
