// Copyright 2020 megasuraman.
/**
 * @file CSKit_Voxel.cpp
 * @brief Voxelクラス
 * @author megasuraman
 * @date 2026/08/11
 */
#include "Voxel/CSKit_Voxel.h"

/**
 * @brief	周囲のVoxelIndexを取得
 */
void FCSKit_VoxelIndex::GetAroundVoxelList(TArray<FCSKit_VoxelIndex>& OutList, const int32 InAroundNum) const
{
	for (int32 x=-InAroundNum; x<=InAroundNum; ++x)
	{
		for (int32 y=-InAroundNum; y<=InAroundNum; ++y)
		{
			for (int32 z=-InAroundNum; z<=InAroundNum; ++z)
			{
				if (x == 0 && y == 0 && z == 0)
				{
					continue;
				}
				OutList.Add(FCSKit_VoxelIndex(mIndex.mX + x, mIndex.mY + y, mIndex.mZ + z));
			}
		}
	}
}

/**
 * @brief	指定Indexとの距離取得(マンハッタン距離)
 */
int32 FCSKit_VoxelIndex::Distance(const FCSKit_VoxelIndex& InTargetIndex) const
{
	int32 Distance = FMath::Abs(mIndex.mX - InTargetIndex.mIndex.mX);
	Distance += FMath::Abs(mIndex.mY - InTargetIndex.mIndex.mY);
	Distance += FMath::Abs(mIndex.mZ - InTargetIndex.mIndex.mZ);
	return Distance;
}

/**
 * @brief	
 */
int32 FCSKit_VoxelIndex::DiffMax(const FCSKit_VoxelIndex& InTargetIndex) const
{
	int32 DiffMax = FMath::Abs(mIndex.mX - InTargetIndex.mIndex.mX);
	DiffMax = FMath::Max(DiffMax, FMath::Abs(mIndex.mY - InTargetIndex.mIndex.mY) );
	DiffMax = FMath::Max(DiffMax, FMath::Abs(mIndex.mZ - InTargetIndex.mIndex.mZ) );
	return DiffMax;
}

/**
 * @brief	
 */
FString FCSKit_VoxelIndex::ToString() const
{
	return FString::Printf(TEXT("[%d, %d, %d](%lld)"), mIndex.mX, mIndex.mY, mIndex.mZ, mUID);
}

/**
 * @brief	
 */
FString FCSKit_VoxelIndex::ToStringIndex() const
{
	return FString::Printf(TEXT("[%d, %d, %d]"), mIndex.mX, mIndex.mY, mIndex.mZ);
}

/**
 * @brief	
 */
void FCSKit_VoxelIndex::FromString(const FString& InString)
{
	if (InString.Contains(TEXT(",")))
	{//[1, 2, 3]
		FString OnlyNumberString = InString;
		OnlyNumberString.RemoveFromStart(TEXT("["));
		OnlyNumberString.RemoveFromEnd(TEXT("]"));
		TArray<FString> NumberArray;
		OnlyNumberString.ParseIntoArray(NumberArray, TEXT(","));
		if (NumberArray.Num() != 3)
		{
			return;
		}
		mIndex.mX = FCString::Atoi(*NumberArray[0]);
		mIndex.mY = FCString::Atoi(*NumberArray[1]);
		mIndex.mZ = FCString::Atoi(*NumberArray[2]);
	}
	else
	{//uint64
		mUID = FCString::Strtoui64(*InString, nullptr, 10);
	}
}



/**
 * @brief	周囲のVoxelIndexを取得
 */
void FCSKit_Voxel::GetAroundVoxelList(TArray<FCSKit_VoxelIndex>& OutList, const int32 InAroundNum) const
{
	mVoxelIndex.GetAroundVoxelList(OutList, InAroundNum);
}
/**
 * @brief	Voxelのワールド座標取得
 */
FVector FCSKit_VoxelChunkBase::GetVoxelPos(const FCSKit_VoxelIndex& InIndex) const
{
	FVector VoxelPos = FVector::ZeroVector;
	VoxelPos.X += static_cast<float>(InIndex.mIndex.mX) * mVoxelLength;
	VoxelPos.Y += static_cast<float>(InIndex.mIndex.mY) * mVoxelLength;
	VoxelPos.Z += static_cast<float>(InIndex.mIndex.mZ) * mVoxelLength;
	
	return mTransform.TransformPosition(VoxelPos);
}

/**
 * @brief	
 */
FVector FCSKit_VoxelChunkBase::GetVoxelPos(const FString& InIndexString) const
{
	const FCSKit_VoxelIndex Index(InIndexString);
	if (!Index.IsValid())
	{
		return FVector::ZeroVector;
	}
	return GetVoxelPos(Index);
}

/**
 * @brief	ワールド座標のVoxelIndex取得
 */
FCSKit_VoxelIndex FCSKit_VoxelChunkBase::GetVoxelIndex(const FVector& InWorldPos) const
{
	const FVector LocalPos = mTransform.InverseTransformPosition(InWorldPos);
	ensure(mVoxelLength > 0.f);
	const float VoxelLengthRcp = 1.f / mVoxelLength;
	const int32 IndexX = FMath::RoundToInt(LocalPos.X * VoxelLengthRcp );
	const int32 IndexY = FMath::RoundToInt(LocalPos.Y * VoxelLengthRcp );
	const int32 IndexZ = FMath::RoundToInt(LocalPos.Z * VoxelLengthRcp );
	return FCSKit_VoxelIndex(IndexX, IndexY, IndexZ);
}
FString FCSKit_VoxelChunkBase::GetVoxelIndexString(const FVector& InWorldPos) const
{
	const FCSKit_VoxelIndex Index = GetVoxelIndex(InWorldPos);
	return Index.ToStringIndex();
}

/**
 * @brief	端っこのVoxelかどうか
 */
bool FCSKit_VoxelChunkBase::IsEdgeVoxel(const FCSKit_VoxelIndex& InIndex) const
{
	if (FMath::Abs(InIndex.mIndex.mX) == mVoxelExtentX
		|| FMath::Abs(InIndex.mIndex.mY) == mVoxelExtentY
		|| FMath::Abs(InIndex.mIndex.mZ) == mVoxelExtentZ)
	{
		return true;
	}
	return false;
}
