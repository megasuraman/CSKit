// Copyright 2020 megasuraman
/**
 * @file CSKitDebug_CollisionInspectorVoxel.cpp
 * @brief コリジョンチェック用のVoxel情報
 * @author megasuraman
 * @date 2025/12/12
 */

#include "CollisionInspector/CSKitDebug_CollisionInspectorVoxel.h"

/**
 * @brief	周囲のVoxelIndexを取得
 */
void FCollisionInspectorVoxel::GetAroundVoxelList(TArray<FCollisionInspectorVoxelIndex>& OutList, const int32 InAroundNum) const
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
				OutList.Add(FCollisionInspectorVoxelIndex(mVoxelIndex.mIndex.mX + x, mVoxelIndex.mIndex.mY + y, mVoxelIndex.mIndex.mZ + z));
			}
		}
	}
}

/**
 * @brief	
 */
FString FCollisionInspectorVoxel::GetVoxelStateString(ECollisionInspectorVoxelState InVoxelState)
{
	switch (InVoxelState)
	{
	case ECollisionInspectorVoxelState::Safe:
		return FString(TEXT("Safe"));
	case ECollisionInspectorVoxelState::Hit:
		return FString(TEXT("Hit"));
	case ECollisionInspectorVoxelState::Outside:
		return FString(TEXT("Outside"));
	default:
		break;
	}
	return FString();
}

/**
 * @brief	
 */
FColor FCollisionInspectorVoxel::GetVoxelStateColor(const ECollisionInspectorVoxelState InVoxelState)
{
	switch (InVoxelState)
	{
	case ECollisionInspectorVoxelState::Safe:
		return FColor::Green;
	case ECollisionInspectorVoxelState::Hit:
		return FColor::Orange;
	case ECollisionInspectorVoxelState::Outside:
		return FColor::Black;
	default:
		break;
	}
	
	return FColor::White;
}

/**
 * @brief	
 */
void FCollisionInspectorVoxelChunk::SetVoxelExtent(const int32 InX, const int32 InY, const int32 InZ)
{
	ensure(InX >= 0 && InY >= 0 && InZ >= 0);
	mVoxelExtentX = InX;
	mVoxelExtentY = InY;
	mVoxelExtentZ = InZ;
}

/**
 * @brief	Voxel初期化
 */
void FCollisionInspectorVoxelChunk::InitVoxel()
{
	mVoxelList.Empty();
	mVoxelList.Reserve(mVoxelExtentX * mVoxelExtentY * mVoxelExtentZ);
	for (int32 x=-mVoxelExtentX; x<=mVoxelExtentX; ++x)
	{
		for (int32 y=-mVoxelExtentY; y<=mVoxelExtentY; ++y)
		{
			for (int32 z=-mVoxelExtentZ; z<=mVoxelExtentZ; ++z)
			{
				FCollisionInspectorVoxel Voxel;
				Voxel.mVoxelIndex = FCollisionInspectorVoxelIndex(x, y, z);
				mVoxelList.Add(Voxel);
			}
		}
	}
	
	mVoxelMap.Empty();
	for (FCollisionInspectorVoxel& Voxel : mVoxelList)
	{
		mVoxelMap.Add(Voxel.mVoxelIndex.mUID, &Voxel);
	}
}

/**
 * @brief	Voxelのワールド座標取得
 */
FVector FCollisionInspectorVoxelChunk::GetVoxelPos(const FCollisionInspectorVoxelIndex& InIndex) const
{
	FVector VoxelPos = FVector::ZeroVector;
	VoxelPos.X += static_cast<float>(InIndex.mIndex.mX) * mVoxelLength;
	VoxelPos.Y += static_cast<float>(InIndex.mIndex.mY) * mVoxelLength;
	VoxelPos.Z += static_cast<float>(InIndex.mIndex.mZ) * mVoxelLength;
	
	return mTransform.TransformPosition(VoxelPos);
}

/**
 * @brief	指定IndexのVoxel取得
 */
FCollisionInspectorVoxel* FCollisionInspectorVoxelChunk::FindVoxel(const FCollisionInspectorVoxelIndex& InIndex)
{
	if (FCollisionInspectorVoxel** VoxelPtr = mVoxelMap.Find(InIndex.mUID))
	{
		return *VoxelPtr;
	}
	return nullptr;
}
const FCollisionInspectorVoxel* FCollisionInspectorVoxelChunk::FindVoxel(const FCollisionInspectorVoxelIndex& InIndex) const
{
	if (const FCollisionInspectorVoxel* const* VoxelPtr = mVoxelMap.Find(InIndex.mUID))
	{
		return *VoxelPtr;
	}
	return nullptr;
}

/**
 * @brief	Voxel状態リセット
 */
void FCollisionInspectorVoxelChunk::ClearAllVoxelState()
{
	for (FCollisionInspectorVoxel& Voxel : mVoxelList)
	{
		Voxel.mState = ECollisionInspectorVoxelState::Invalid;
	}
}

/**
 * @brief	InvalidなVoxelをOutsideに設定
 */
void FCollisionInspectorVoxelChunk::AssignAllInvalidVoxelToOutside()
{
	for (FCollisionInspectorVoxel& Voxel : mVoxelList)
	{
		if (Voxel.mState == ECollisionInspectorVoxelState::Invalid)
		{
			Voxel.mState = ECollisionInspectorVoxelState::Outside;
		}
	}
}