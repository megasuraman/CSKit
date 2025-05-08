// Copyright 2020 megasuraman.
/**
 * @file CSKit_BlockSensor.cpp
 * @brief 移動引っ掛かりチェック
 * @author megasuraman
 * @date 2025/05/06
 */
#include "AI/CSKit_BlockSensor.h"

#include "DrawDebugHelpers.h"

FCSKit_BlockSensor::FCSKit_BlockSensor()
	: mbActive(true)
	, mbBlock(false)
	, mbCompleteLocationLog(false)
{
	mLocationLog.Reserve(10);
}

/* ------------------------------------------------------------
   !引っかかりチェック
------------------------------------------------------------ */
bool FCSKit_BlockSensor::Update(float InDeltaTime, const FVector& InPos)
{
	if (mbActive)
	{
		if (UpdateLocationLog(InDeltaTime, InPos))
		{
			return UpdateBlock();
		}
	}
	return false;
}
/* ------------------------------------------------------------
   !
------------------------------------------------------------ */
void FCSKit_BlockSensor::Reset()
{
	mNextLocationLogIndex = 0;
	mbCompleteLocationLog = false;
	mLocationLogTime = 0.f;
	mbBlock = false;
}

/* ------------------------------------------------------------
   !座標ログの更新
------------------------------------------------------------ */
bool FCSKit_BlockSensor::UpdateLocationLog(float InDeltaTime, const FVector& InPos)
{
	mLocationLogTime += InDeltaTime;
	if (mLocationLogTime > mLocationLogInterval)
	{
		const int32 LocationLogMaxSize = mLocationLog.Max();
		if (LocationLogMaxSize > 0)
		{
			if (mLocationLog.Num() == mNextLocationLogIndex)
			{
				mLocationLog.AddZeroed(1);
			}
			mLocationLog[mNextLocationLogIndex] = InPos;
			mNextLocationLogIndex = (mNextLocationLogIndex + 1) % LocationLogMaxSize;
			if (mNextLocationLogIndex == 0)
			{
				mbCompleteLocationLog = true;
			}
			mLocationLogTime = 0.f;
			return true;
		}
	}
	return false;
}
/* ------------------------------------------------------------
   !引っかかりチェック
------------------------------------------------------------ */
bool FCSKit_BlockSensor::UpdateBlock()
{
	mbBlock = false;
	const int32 LocationLogSize = mLocationLog.Num();
	if (!mbCompleteLocationLog
		|| LocationLogSize <= 0)
	{
		return false;
	}

	FVector AveragePos = FVector::ZeroVector;
	for (const FVector& Pos : mLocationLog)
	{
		AveragePos += Pos;
	}

	AveragePos = AveragePos / static_cast<double>(LocationLogSize);

	const float DistanceBorderSq = FMath::Square(mCheckBlockDistanceBorder);
	for (const FVector& Pos : mLocationLog)
	{
		if (FVector::DistSquared(AveragePos, Pos) > DistanceBorderSq)
		{
			return false;
		}
	}

	mbBlock = true;
	return true;
}

#ifdef USE_CSKIT_DEBUG
/* ------------------------------------------------------------
   !座標ログ表示
------------------------------------------------------------ */
void FCSKit_BlockSensor::DebugDraw(const class UWorld* InWorld) const
{
	FColor LineColor = FColor(157, 204, 224);
	if (mbBlock)
	{
		LineColor = FColor::Red;
	}
	const int32 LocationLogSize = mLocationLog.Num();
	for (int32 i = 0; i < LocationLogSize; ++i)
	{
		if (!mbCompleteLocationLog
			&& mNextLocationLogIndex >= i)
		{
			return;
		}
		DrawDebugLine(InWorld, mLocationLog[i], mLocationLog[i] + FVector(0.f, 0.f, 150.f), LineColor);
	}
}
#endif