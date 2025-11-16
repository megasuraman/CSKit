// Copyright 2020 megasuraman.
/**
 * @file CSKit_Grid.h
 * @brief 簡易的なGrid管理提供
 * @author megasuraman
 * @date 2025/05/05
 */
#pragma once

#include "CoreMinimal.h"
#include "Engine/Canvas.h"
#include "DrawDebugHelpers.h"
#if USE_CSKIT_DEBUG
#include "ScreenWindow/CSKitDebug_ScreenWindowText.h"
#endif

//Spawn管理用GridのIndex(原点基準でGrid分けするので0はNG)
union CSKIT_API FCSKit_GridIndex
{
	int32 mUID = 0;
	struct IndexXY
	{
		int16 mX;
		int16 mY;
	};
	IndexXY	mIndex;

	FCSKit_GridIndex()
	{}
	FCSKit_GridIndex(const int32 InUID);
	FCSKit_GridIndex(const int32 InX, const int32 InY);

	void ShiftIndexX(const bool bInPlus);
	void ShiftIndexY(const bool bInPlus);
	bool IsValid() const{return mUID!=0;}
	bool operator == (const FCSKit_GridIndex& In) const
	{
		return (mUID == In.mUID);
	}
};


//Gridが持つDataを示すIndex
struct FCSKit_GridNodeIndex
{
	FCSKit_GridIndex mGridIndex;
	int32 mListIndex = -1;

	FCSKit_GridNodeIndex(){}
	FCSKit_GridNodeIndex(const FCSKit_GridIndex& InGridIndex, const int32 InListIndex)
		:mGridIndex(InGridIndex)
		,mListIndex(InListIndex)
	{}
	bool IsValid() const{return mGridIndex.IsValid()&&mListIndex>=0;}
	FString ToString() const {return FString::Printf(TEXT("[%d,%d](%d)"),mGridIndex.mIndex.mX,mGridIndex.mIndex.mY,mListIndex);}
	bool operator == (const FCSKit_GridNodeIndex& In) const
	{
		return (mGridIndex.mUID == In.mGridIndex.mUID
			&& mListIndex == In.mListIndex);
	}
};

struct CSKIT_API FCSKit_GridBase
{
	FCSKit_GridBase(){}
	virtual ~FCSKit_GridBase(){}

	void SetGridLength(const float InLength);
	FCSKit_GridIndex CalcGridIndex(const FVector& InPos) const;
	
	void CollectAroundGridIndexList(TArray<FCSKit_GridIndex>& OutList, const FCSKit_GridIndex& InBaseGridIndex, const float InRadius) const;
	static void CollectAroundGridIndexList(TArray<FCSKit_GridIndex>& OutList, const FCSKit_GridIndex& InBaseGridIndex, const int32 InOffsetIndex);

protected:
	float mGridLength = 2000.f;
	float mGridLengthRcp = 0.0005f;//mGridLengthの逆数
};

template<class T>
struct FCSKit_Grid : public FCSKit_GridBase
{
public:
	FCSKit_Grid(){}
	virtual ~FCSKit_Grid() override {}

	void Clear()
	{
		mGridMap.Empty();
	}
	FCSKit_GridNodeIndex Add(const T& InData)
	{
		const FCSKit_GridIndex GridIndex = CalcGridIndex(InData.GetPos());
		TArray<T>& DataList = mGridMap.FindOrAdd(GridIndex.mUID);
		const int32 DataListIndex = DataList.AddUnique(InData);
		return FCSKit_GridNodeIndex(GridIndex, DataListIndex);
	}
	bool Sub(const T& InData)
	{
		const FCSKit_GridIndex GridIndex = CalcGridIndex(InData.GetPos());
		if (TArray<T>* DataList = mGridMap.Find(GridIndex.mUID))
		{
			const int32 DataListIndex = DataList->Find(InData);
			if (DataListIndex != INDEX_NONE)
			{
				DataList->RemoveAtSwap(DataListIndex);
			}
		}
		return false;
	}
	bool IsOwn(const T& InData) const
	{
		const FCSKit_GridIndex GridIndex = CalcGridIndex(InData.GetPos());
		if (const TArray<T>* DataList = mGridMap.Find(GridIndex.mUID))
		{
			if (DataList->Find(InData) != -INDEX_NONE)
			{
				return true;
			}
		}
		return false;
	}
	void Collect(TArray<T*>& OutList, const FVector& InPos, const float InRadius)
	{
		const float BorderDistanceSq = FMath::Square(InRadius);
		const FCSKit_GridIndex ViewGridIndex = CalcGridIndex(InPos);
		TArray<FCSKit_GridIndex> AroundGridIndexList;
		CollectAroundGridIndexList(AroundGridIndexList, ViewGridIndex, InRadius);
		for (const FCSKit_GridIndex& GridIndex : AroundGridIndexList)
		{
			if ( TArray<T>* DataPtr = mGridMap.Find(GridIndex.mUID))
			{
				TArray<T>& DataList = *DataPtr;
				for (T& Data : DataList)
				{
					if (FVector::DistSquared(Data.GetPos(), InPos) < BorderDistanceSq)
					{
						OutList.AddUnique(&Data);
					}
				}
			}
		}
	}
	void Collect(TArray<const T*>& OutList, const FVector& InPos, const float InRadius) const
	{
		const float BorderDistanceSq = FMath::Square(InRadius);
		const FCSKit_GridIndex ViewGridIndex = CalcGridIndex(InPos);
		TArray<FCSKit_GridIndex> AroundGridIndexList;
		CollectAroundGridIndexList(AroundGridIndexList, ViewGridIndex, InRadius);
		for (const FCSKit_GridIndex& GridIndex : AroundGridIndexList)
		{
			if ( const TArray<T>* DataPtr = mGridMap.Find(GridIndex.mUID))
			{
				const TArray<T>& DataList = *DataPtr;
				for (const T& Data : DataList)
				{
					if (FVector::DistSquared(Data.GetPos(), InPos) < BorderDistanceSq)
					{
						OutList.AddUnique(&Data);
					}
				}
			}
		}
	}
	T* Find(const FCSKit_GridNodeIndex& InGridNodeIndex)
	{
		if(TArray<T>* DataListPtr = mGridMap.Find(InGridNodeIndex.mGridIndex.mUID))
		{
			TArray<T>& DataList = *DataListPtr;
			if(InGridNodeIndex.mListIndex < DataList.Num())
			{
				return &DataList[InGridNodeIndex.mListIndex];
			}
		}
		return nullptr;
	}
	const T* Find(const FCSKit_GridNodeIndex& InGridNodeIndex) const
	{
		if(const TArray<T>* DataListPtr = mGridMap.Find(InGridNodeIndex.mGridIndex.mUID))
		{
			const TArray<T>& DataList = *DataListPtr;
			if(InGridNodeIndex.mListIndex < DataList.Num())
			{
				return &DataList[InGridNodeIndex.mListIndex];
			}
		}
		return nullptr;
	}
	TMap<int32, TArray<T>>& GetGridMap(){return mGridMap;}
	const TMap<int32, TArray<T>>& GetGridMap() const {return mGridMap;}

private:
	TMap<int32, TArray<T>> mGridMap;

#if USE_CSKIT_DEBUG
public:
	int32 DebugCalcTotalDataNum() const
	{
		int32 Count = 0;
		for(const auto& MapElement : mGridMap)
		{
			Count += MapElement.Value.Num();
		}
		return Count;
	}
	void DebugDraw(UCanvas* InCanvas, UWorld* InWorld) const
	{
		FVector ViewPos = FVector::ZeroVector;
		if (const FSceneView* View = InCanvas->SceneView)
		{
			ViewPos = View->ViewMatrices.GetViewOrigin();
		}

		const float GridLengthHalf = mGridLength*0.5f;
		const FCSKit_GridIndex ViewGridIndex = CalcGridIndex(ViewPos);
		TArray<FCSKit_GridIndex> AroundGridIndexList;
		CollectAroundGridIndexList(AroundGridIndexList, ViewGridIndex, 1);
		for (const FCSKit_GridIndex& TargetGridIndex : AroundGridIndexList)
		{
			const int32 IndexX = TargetGridIndex.mIndex.mX;
			const int32 IndexY = TargetGridIndex.mIndex.mY;

			FColor GridColor = FColor::White;
			float MaxPosZ = -100000.f;
			float MinPosZ = 100000.f;
			const FCSKit_GridIndex GridIndex(IndexX, IndexY);
			if (const TArray<T>* DataPtr = mGridMap.Find(GridIndex.mUID))
			{
				GridColor = FColor::Green;
				const TArray<T>& DataList = *DataPtr;
				if (DataList.Num() > 0)
				{
					GridColor = FColor::Green;
				}
				for (const T& Data : DataList)
				{
					Data.DebugDraw(InCanvas, InWorld);
					const FVector Pos = Data.GetPos();
					if(Pos.Z > MaxPosZ)
					{
						MaxPosZ = Pos.Z;
					}
					if(Pos.Z < MinPosZ)
					{
						MinPosZ = Pos.Z;
					}
				}
			}

			FVector GridCenterPos(static_cast<float>(IndexX)*mGridLength, static_cast<float>(IndexY)*mGridLength, ViewPos.Z);
			FVector OffsetGridCenterV = FVector(GridLengthHalf, GridLengthHalf, 0.f);
			if (IndexX > 0)
			{
				OffsetGridCenterV.X *= -1.f;
			}
			if (IndexY > 0)
			{
				OffsetGridCenterV.Y*= -1.f;
			}
			GridCenterPos += OffsetGridCenterV;
			GridCenterPos.Z = (MaxPosZ + MinPosZ)*0.5f;
			const FVector BoxExtentV(GridLengthHalf, GridLengthHalf, FMath::Abs(MaxPosZ - MinPosZ)*0.5f + 100.f);
			DrawDebugBox(InWorld, GridCenterPos, BoxExtentV, GridColor, false, -1.f, 0, 5.f);

			FCSKitDebug_ScreenWindowText ScreenWindow;
			//ScreenWindow.SetWindowName(TEXT("EnemySpawnManager"));
			ScreenWindow.SetWindowFrameColor(GridColor);
			ScreenWindow.AddText(FString::Printf(TEXT("%d, %d"), IndexX, IndexY));
			ScreenWindow.Draw(InCanvas, GridCenterPos);
		}
	}
#endif
};