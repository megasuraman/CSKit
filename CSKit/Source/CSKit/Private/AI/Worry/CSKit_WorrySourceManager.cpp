// Copyright 2020 megasuraman.
/**
 * @file CSKit_WorrySourceManager.cpp
 * @brief 全AIのWorrySource情報の取りまとめ
 * @author megasuraman
 * @date 2025/05/05
 */
#include "AI/Worry/CSKit_WorrySourceManager.h"

#include "CSKit_Subsystem.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "ScreenWindow/CSKitDebug_ScreenWindowManager.h"

#if USE_CSKIT_DEBUG
#include "CSKitDebug_Subsystem.h"
#include "ScreenWindow/CSKitDebug_ScreenWindowText.h"
#endif


UCSKit_WorrySourceManager* UCSKit_WorrySourceManager::sGet(const UWorld* InWorld)
{
	if(InWorld
		&& !InWorld->IsNetMode(NM_Client))
	{
		if (const UCSKit_Subsystem* CSKitSubsystem = InWorld->GetSubsystem<UCSKit_Subsystem>())
		{
			return CSKitSubsystem->GetWorrySourceManager();
		}
	}
	return nullptr;
}

void UCSKit_WorrySourceManager::sEntrySource(const UWorld* InWorld, const FName& InName, const FVector& InPos, const AActor* InSourceActor)
{
	if(UCSKit_WorrySourceManager* WorrySourceManager = sGet(InWorld))
	{
		WorrySourceManager->EntryWorrySource(InName, InPos, InSourceActor);
	}
}

UCSKit_WorrySourceManager::UCSKit_WorrySourceManager()
{
}

/* ------------------------------------------------------------
   !更新処理
------------------------------------------------------------ */
void UCSKit_WorrySourceManager::Update(const float InDeltaSec)
{
	if (GetWorld()->IsNetMode(NM_Client))
	{
		return;
	}

	for(auto& MapElement : mGridMap)
	{
		for(FCSKit_WorrySource& WorrySource : MapElement.Value.mWorrySourceList)
		{
			if(!WorrySource.mbActive)
			{
				continue;
			}
			WorrySource.mLifeSec += InDeltaSec;
			if(WorrySource.mLifeSec > WorrySource.mData.mEffectTime)
			{
				WorrySource.mbActive = false;
				continue;
			}
			
			if( WorrySource.mLifeSec < WorrySource.mData.mEffectScaleTime
				&& WorrySource.mData.mEffectScaleTime > 0.f)
			{
				const float ScaleRatio = WorrySource.mLifeSec / WorrySource.mData.mEffectScaleTime;
				WorrySource.mRadius = WorrySource.mData.mEffectRadiusMin + (WorrySource.mData.mEffectRadiusMax - WorrySource.mData.mEffectRadiusMin)*ScaleRatio;
			}
			else
			{
				WorrySource.mRadius = WorrySource.mData.mEffectRadiusMax;
			}
		}
	}
}

/**
 * @brief 
 */
void UCSKit_WorrySourceManager::EntryWorrySource(const FName& InName, const FVector& InPos, const AActor* InSourceActor)
{
	const FCSKit_WorrySourceTableRow* WorrySourceTableRow = FindWorrySourceTableRow(InName);
	if(WorrySourceTableRow == nullptr)
	{
#if USE_CSKIT_DEBUG
		UCSKitDebug_Subsystem::sOneShotWarning(
			GetWorld(),
			false,
			TEXT("UCSKit_WorrySourceManager"),
			FString::Printf(TEXT("UCSKit_WorrySourceManager::EntryWorrySource() NoDataTable : %s"), *InName.ToString())
		);
#endif
		return;
	}
	const FCSKit_WorryGridIndex GridIndex = CalcGridIndex(InPos);
	FCSKit_WorrySourceGridElement& GridElement = mGridMap.FindOrAdd(GridIndex.mUID);
	FCSKit_WorrySource AddWorrySource;
	AddWorrySource.mData = *WorrySourceTableRow;
	AddWorrySource.mPos = InPos;
	AddWorrySource.mRadius = AddWorrySource.mData.mEffectRadiusMin;
	AddWorrySource.mSourceActor = InSourceActor;
	AddWorrySource.mName = InName;
	AddWorrySource.mbActive = true;
	for(FCSKit_WorrySource& WorrySource : GridElement.mWorrySourceList)
	{
		if(!WorrySource.mbActive)
		{
			WorrySource = AddWorrySource;
			return;
		}
	}
	GridElement.mWorrySourceList.Add(AddWorrySource);
}

/**
 * @brief 
 */
FCSKit_WorryGridIndex UCSKit_WorrySourceManager::CalcGridIndex(const FVector& InPos) const
{
	FCSKit_WorryGridIndex GridIndex;
#if USE_CSKIT_DEBUG
	if( FMath::Abs(InPos.X) >= 32768.f*mGridLength
		|| FMath::Abs(InPos.Y) >= 32768.f*mGridLength )
	{
		UCSKitDebug_Subsystem::sOneShotWarning(
			GetWorld(),
			false,
			TEXT("CSKit_WorrySourceManager"),
			FString::Printf(TEXT("CalcGrid RangeOver : %s"), *InPos.ToString())
		);
		return GridIndex;
	}
#endif

	const float RcpVoxelLength = 1.f / mGridLength;
	GridIndex.mIndex.mX = FMath::TruncToInt(InPos.X * RcpVoxelLength);
	GridIndex.mIndex.mX += InPos.X>0.f ? 1 : -1;
	GridIndex.mIndex.mY = FMath::TruncToInt(InPos.Y * RcpVoxelLength);
	GridIndex.mIndex.mY += InPos.Y>0.f ? 1 : -1;
	return GridIndex;
}

/**
 * @brief 
 */
void UCSKit_WorrySourceManager::CollectTouchSource(TArray<FCSKit_WorrySource>& OutList, const FVector& InPos, const float InRadius, const float InHeight) const
{
	const FCSKit_WorryGridIndex BaseGridIndex = CalcGridIndex(InPos);
	const int32 OffsetIndex = FMath::TruncToInt(InRadius/mGridLength) + 1;
	//これだとOffsetIndex大きいほど角部分が無駄に
	for(int32 x=-OffsetIndex; x<=OffsetIndex; ++x)
	{
		int32 CheckIndexX = BaseGridIndex.mIndex.mX + x;
		if( BaseGridIndex.mIndex.mX > 0
			&& CheckIndexX <= 0)
		{
			--CheckIndexX;//0を飛び越すために
		}
		else if( BaseGridIndex.mIndex.mX < 0
			&& CheckIndexX >= 0)
		{
			++CheckIndexX;//0を飛び越すために
		}

		for(int32 y=-OffsetIndex; y<=OffsetIndex; ++y)
		{
			int32 CheckIndexY = BaseGridIndex.mIndex.mY + y;
			if( BaseGridIndex.mIndex.mY > 0
				&& CheckIndexY <= 0)
			{
				--CheckIndexY;//0を飛び越すために
			}
			else if( BaseGridIndex.mIndex.mY < 0
				&& CheckIndexY >= 0)
			{
				++CheckIndexY;//0を飛び越すために
			}
			const FCSKit_WorryGridIndex CheckIndex(CheckIndexX, CheckIndexY);
			const FCSKit_WorrySourceGridElement* GridElement = mGridMap.Find(CheckIndex.mUID);
			if(GridElement == nullptr)
			{
				continue;
			}

			for(const FCSKit_WorrySource& WorrySource : GridElement->mWorrySourceList)
			{
				if(WorrySource.mbActive
					&& FMath::Abs(InPos.Z - WorrySource.mPos.Z) < InHeight
					&& FVector::DistSquaredXY(InPos,WorrySource.mPos) < FMath::Square(InRadius + WorrySource.mRadius))
				{
					OutList.Add(WorrySource);
				}
			}
		}
	}
}

const FCSKit_WorrySourceTableRow* UCSKit_WorrySourceManager::FindWorrySourceTableRow(const FName& InName) const
{
	return nullptr;
}

#if USE_CSKIT_DEBUG
/* ------------------------------------------------------------
   !デバッグ表示
------------------------------------------------------------ */
void UCSKit_WorrySourceManager::DebugDraw(UCanvas* InCanvas) const
{
	if (mbDebugDrawInfo)
	{
		DebugDrawInfo(InCanvas);
		DebugDrawSource(InCanvas);
	}
}
/* ------------------------------------------------------------
   !デバッグ表示
------------------------------------------------------------ */
void UCSKit_WorrySourceManager::DebugDrawInfo(UCanvas* InCanvas) const
{
	UCSKitDebug_ScreenWindowManager* DebugWindowManager = UCSKitDebug_ScreenWindowManager::Get(this);
	if(DebugWindowManager == nullptr)
	{
		return;
	}

	int32 TotalWorrySourceNum = 0;
	int32 TotalActiveWorrySourceNum = 0;
	for(const auto& MapElement : mGridMap)
	{
		TotalWorrySourceNum += MapElement.Value.mWorrySourceList.Num();
		for(const FCSKit_WorrySource& WorrySource : MapElement.Value.mWorrySourceList)
		{
			if(WorrySource.mbActive)
			{
				++TotalActiveWorrySourceNum;
			}
		}
	}

	FString Message;
	Message += FString::Printf(TEXT("TotalNum : %d\n"), TotalWorrySourceNum);
	Message += FString::Printf(TEXT("ActiveNum : %d\n"), TotalActiveWorrySourceNum);
	DebugWindowManager->AddWindow(TEXT("WorrySourceManager"), Message);
}

/* ------------------------------------------------------------
   !心配元デバッグ表示
------------------------------------------------------------ */
void UCSKit_WorrySourceManager::DebugDrawSource(UCanvas* InCanvas) const
{
	for(const auto& MapElement : mGridMap)
	{
		for(const FCSKit_WorrySource& WorrySource : MapElement.Value.mWorrySourceList)
		{
			if(!WorrySource.mbActive)
			{
				continue;
			}
			const FMatrix CircleMatrix(FQuatRotationTranslationMatrix(FQuat(FVector(0.f, 1.f, 0.f), PI*0.5f), WorrySource.mPos));
			DrawDebugCircle(
				GetWorld(),
				CircleMatrix,
				WorrySource.mRadius,
				32,
				FColor::Green,
				false,
				-1.f,
				0,
				3.f
			);
			
			FCSKitDebug_ScreenWindowText ScreenWindowText;
			ScreenWindowText.AddText(FString::Printf(TEXT("Name : %s"), *WorrySource.mName.ToString()));
			ScreenWindowText.AddText(FString::Printf(TEXT("Radius : %.1f"), WorrySource.mRadius));
			ScreenWindowText.AddText(FString::Printf(TEXT("Life : %.1f/%.1f"), WorrySource.mLifeSec, WorrySource.mData.mEffectTime));
			ScreenWindowText.Draw(InCanvas, WorrySource.mPos, 1000.f);
		}
	}
}
#endif
