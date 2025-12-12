// Copyright 2020 megasuraman
/**
 * @file CSKitDebug_CollisionInspectorError.cpp
 * @brief コリジョンチェック用のエラー情報
 * @author megasuraman
 * @date 2025/12/12
 */

#include "CollisionInspector/CSKitDebug_CollisionInspectorError.h"

#include "CSKitDebug_Draw.h"
#include "ScreenWindow/CSKitDebug_ScreenWindowText.h"
#include "CSKitDebug_Utility.h"
#include "DrawDebugHelpers.h"

#if !UE_BUILD_SHIPPING
/**
 * @brief	コリジョンヒット情報表示
 */
void FCollisionInspectorError_CollisionHitData::Set(const FHitResult& InHitResult)
{
	mObjectName = UCSKitDebug_Utility::GetObjectDisplayName(InHitResult.GetComponent());
	mImpactPointString = InHitResult.ImpactPoint.ToString();
	mImpactNormalString = InHitResult.ImpactNormal.ToString();
}

/**
 * @brief	コリジョン抜け情報表示
 */
void FCollisionInspectorErrorData_PassThrough::Draw(
	const UWorld* InWorld,
	UCanvas* InCanvas,
	FCSKitDebug_ScreenWindowText& InWindowText,
	const FCollisionInspectorErrorNode& InErrorNode
	) const
{
	InWindowText.AddText(mHitCollisionInfo);
	DrawDebugLine(InWorld, InErrorNode.GetPos(), GetLastSafePos(), FColor::Orange, false, -1.f, 255, 3.f);
}

/**
 * @brief	スタック情報表示
 */
void FCollisionInspectorErrorData_Stuck::Draw(
	const UWorld* InWorld,
	UCanvas* InCanvas,
	FCSKitDebug_ScreenWindowText& InWindowText,
	const FCollisionInspectorErrorNode& InErrorNode
	) const
{
	InWindowText.AddText(mHitCollisionInfo);
	DrawDebugLine(InWorld, InErrorNode.GetPos(), GetLastSafePos(), FColor::Orange, false, -1.f, 255, 3.f);
	
	FVector HitImpactPos;HitImpactPos.InitFromString(mHitImpactPosString);
	DrawDebugPoint(InWorld, HitImpactPos, 10.f, FColor::Red, false, -1.f, 0);
	
	FVector CheckPosBegin;CheckPosBegin.InitFromString(mCheckPosBeginString);
	FVector CheckPosEnd;CheckPosEnd.InitFromString(mCheckPosEndString);
	DrawDebugSphere(InWorld, CheckPosBegin, mCheckRadius, 32, FColor::Blue);
	DrawDebugSphere(InWorld, CheckPosEnd, mCheckRadius, 32, FColor::Yellow);
}

/**
 * @brief	狭すぎコリジョン情報表示
 */
void FCollisionInspectorErrorData_TooTight::Draw(
	const UWorld* InWorld,
	UCanvas* InCanvas,
	FCSKitDebug_ScreenWindowText& InWindowText,
	const FCollisionInspectorErrorNode& InErrorNode
	) const
{
	InWindowText.AddText(FString::Printf(TEXT("Length : %.1f"), mLength));
	if (mCollisionHitDataList.Num() < 2)
	{
		return;
	}
	const FCollisionInspectorError_CollisionHitData& HitDataA = mCollisionHitDataList[0];
	const FCollisionInspectorError_CollisionHitData& HitDataB = mCollisionHitDataList[1];
	const FVector HitPosA = HitDataA.GetImpactPoint();
	const FVector HitNormalA = HitDataA.GetImpactNormal();
	const FVector HitPosB = HitDataB.GetImpactPoint();
	const FVector HitNormalB = HitDataB.GetImpactNormal();
	DrawDebugPoint(InWorld, HitPosA, 5.f, FColor::Blue, false, -1.f, 255);
	DrawDebugLine(InWorld, HitPosA, HitPosA+HitNormalA*FVector(50.f,0.f,0.f), FColor::Blue, false, -1.f, 0, 1.f);
	
	DrawDebugPoint(InWorld, HitPosB, 5.f, FColor::Green, false, -1.f, 255);
	DrawDebugLine(InWorld, HitPosB, HitPosB+HitNormalB*FVector(50.f,0.f,0.f), FColor::Green, false, -1.f, 0, 1.f);
	
	DrawDebugLine(InWorld, HitPosA, HitPosB, FColor::Orange, false, -1.f, 0, 2.f);

	const float NormalDot = FVector::DotProduct(HitNormalA, HitNormalB);
	const float NormalDotAngle = FMath::RadiansToDegrees(FMath::Abs(FMath::Acos(NormalDot)));
	InWindowText.AddText(FString::Printf(TEXT("NormalDot : %.2f(%.2f)"), NormalDot, NormalDotAngle));
}

/**
 * @brief	めり込み情報表示
 */
void FCollisionInspectorErrorData_Penetrating::Draw(
	const UWorld* InWorld,
	UCanvas* InCanvas,
	FCSKitDebug_ScreenWindowText& InWindowText,
	const FCollisionInspectorErrorNode& InErrorNode
	) const
{
	InWindowText.AddText(mHitCollisionInfo);
	
	FVector HitImpactPos;HitImpactPos.InitFromString(mHitImpactPosString);
	DrawDebugPoint(InWorld, HitImpactPos, 10.f, FColor::Red, false, -1.f, 0);
	
	FVector CheckPosBegin;CheckPosBegin.InitFromString(mCheckPosBeginString);
	FVector CheckPosEnd;CheckPosEnd.InitFromString(mCheckPosEndString);
	DrawDebugSphere(InWorld, CheckPosBegin, mCheckRadius, 32, FColor::Blue);
	DrawDebugSphere(InWorld, CheckPosEnd, mCheckRadius, 32, FColor::Yellow);
}

/**
 * @brief	地面なし情報表示
 */
void FCollisionInspectorErrorData_NoGround::Draw(const UWorld* InWorld,
	UCanvas* InCanvas,
	FCSKitDebug_ScreenWindowText& InWindowText,
	const FCollisionInspectorErrorNode& InErrorNode) const
{
	InWindowText.AddText(mPosString);
	const FVector Pos = GetPos();
	DrawDebugLine(InWorld, Pos, Pos - FVector(0.f,0.f,400000.f), FColor::Red, false, -1.f, 0, 3.f);
}

/**
 * @brief	
 */
FString FCollisionInspectorErrorNode::sGetErrorTypeString(const ECollisionInspectorErrorType InErrorType)
{
	switch (InErrorType)
	{
	case ECollisionInspectorErrorType::PassThrough:
		return FString(TEXT("コリジョン抜け"));
	case ECollisionInspectorErrorType::Stuck:
		return FString(TEXT("スタック"));
	case ECollisionInspectorErrorType::TooTight:
		return FString(TEXT("狭すぎる隙間"));
	case ECollisionInspectorErrorType::Penetrating:
		return FString(TEXT("めり込み"));
	case ECollisionInspectorErrorType::NoGround:
		return FString(TEXT("地面なし"));
	default:
		ensureMsgf(false, TEXT("case文対応漏れ"));
		break;
	}
	return FString();
}

/**
 * @brief	
 */
bool FCollisionInspectorErrorNode::IsSame(const FCollisionInspectorErrorNode& InNode) const
{
	if (mErrorType != InNode.mErrorType)
	{
		return false;
	}
	const FVector TargetPos = InNode.GetPos();
	const FVector Pos = GetPos();
	const float NearBorderSq = FMath::Square(100.f);
	if (FVector::DistSquared(Pos, TargetPos) > NearBorderSq)
	{
		return false;
	}
	return true;
}

/**
 * @brief	
 */
void FCollisionInspectorErrorNode::Draw(UWorld* InWorld, UCanvas* InCanvas) const
{
	const FVector ErrorPos = GetPos();
		
	const FVector ArrowOffsetV(0.f, 0.f,100.f);
	constexpr float Thickness = 2.f;
	const FColor DrawColor = FColor::Red;
	UCSKitDebug_Draw::OctahedronArrow OctahedronArrow;
	OctahedronArrow.mTargetPos = ErrorPos;
	OctahedronArrow.mBasePos = ErrorPos + ArrowOffsetV;
	OctahedronArrow.Draw(InWorld, DrawColor, 0, Thickness);
		
	FCSKitDebug_ScreenWindowText Window;
	Window.SetWindowName(FString::Printf(TEXT("%s"), *GetErrorTypeString()));
	Window.SetWindowFrameColor(FColor::Red);
	
	//↓Switch_Caseの分岐代わりに内部のif文で
	DrawErrorTypeInfo<FCollisionInspectorErrorData_PassThrough>(InWorld, InCanvas, Window);
	DrawErrorTypeInfo<FCollisionInspectorErrorData_Stuck>(InWorld, InCanvas, Window);
	DrawErrorTypeInfo<FCollisionInspectorErrorData_TooTight>(InWorld, InCanvas, Window);
	DrawErrorTypeInfo<FCollisionInspectorErrorData_Penetrating>(InWorld, InCanvas, Window);
	DrawErrorTypeInfo<FCollisionInspectorErrorData_NoGround>(InWorld, InCanvas, Window);

	Window.Draw(InCanvas, OctahedronArrow.mBasePos, 1000.f);
}

/**
 * @brief	
 */
bool FCollisionInspectorError::IsOwnSameError(const FCollisionInspectorErrorNode& InNode) const
{
	for (const FCollisionInspectorErrorNode& ErrorNode : mNodeList)
	{
		if (ErrorNode.IsSame(InNode))
		{
			return true;
		}
	}
	return false;
}

/**
 * @brief	
 */
void FCollisionInspectorError::Merge(const FCollisionInspectorError& InError)
{
	for (const FCollisionInspectorErrorNode& ErrorNode : InError.mNodeList)
	{
		if (!IsOwnSameError(ErrorNode))
		{
			mNodeList.Add(ErrorNode);
		}
	}
}

/**
 * @brief	
 */
void FCollisionInspectorError::Draw(UWorld* InWorld, UCanvas* InCanvas) const
{
	for (const FCollisionInspectorErrorNode& ErrorNode : mNodeList)
	{
		ErrorNode.Draw(InWorld, InCanvas);
	}
}
#endif