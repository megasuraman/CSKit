// Copyright 2020 megasuraman
/**
 * @file CSKitDebug_CollisionInspectorVoxel.h
 * @brief コリジョンチェック用のVoxel情報
 * @author megasuraman
 * @date 2025/12/12
 */

#pragma once

#include "CoreMinimal.h"

//VoxelのIndex
union FCollisionInspectorVoxelIndex
{
	int64 mUID = 0;
	struct IndexXYZ
	{
		int16 mX;
		int16 mY;
		int16 mZ;
	};
	IndexXYZ	mIndex;

	FCollisionInspectorVoxelIndex()
	{}
	FCollisionInspectorVoxelIndex(const int64 InUID){mUID = InUID;}
	FCollisionInspectorVoxelIndex(const int16 InX, const int16 InY, const int16 InZ)
	{
		mIndex.mX = InX;
		mIndex.mY = InY;
		mIndex.mZ = InZ;
	}
	void ShiftIndex(const int16 InX, const int16 InY, const int16 InZ)
	{
		mIndex.mX += InX;
		mIndex.mY += InY;
		mIndex.mZ += InZ;
	}
	bool IsValid() const{return mUID!=0;}
};

//Voxelの状態
enum class ECollisionInspectorVoxelState: uint8
{
	Invalid,
	Safe,
	Hit,
	Outside,
};

//Voxel毎に持つ情報
struct FCollisionInspectorVoxel
{
	FCollisionInspectorVoxelIndex mVoxelIndex;
	ECollisionInspectorVoxelState mState = ECollisionInspectorVoxelState::Invalid;
	
	void GetAroundVoxelList(TArray<FCollisionInspectorVoxelIndex>& OutList, const int32 InAroundNum=1) const;
	FString GetStateString() const{return GetVoxelStateString(mState);}
	FColor GetColor() const{return GetVoxelStateColor(mState);}
	
	static FString GetVoxelStateString(ECollisionInspectorVoxelState InVoxelState);
	static FColor GetVoxelStateColor(const ECollisionInspectorVoxelState InVoxelState);
};
//Voxel全体
struct FCollisionInspectorVoxelChunk
{
public:
	void SetVoxelLength(const float InLength){mVoxelLength = InLength;}
	void SetVoxelExtent(const int32 InX, const int32 InY, const int32 InZ);
	void InitVoxel();
	FVector GetVoxelPos(const FCollisionInspectorVoxelIndex& InIndex) const;
	FCollisionInspectorVoxel* FindVoxel(const FCollisionInspectorVoxelIndex& InIndex);
	const FCollisionInspectorVoxel* FindVoxel(const FCollisionInspectorVoxelIndex& InIndex) const;
	void ClearAllVoxelState();
	void AssignAllInvalidVoxelToOutside();
	void SetTransform(const FTransform& InTransform){mTransform=InTransform;}
	const FTransform& GetTransform() const{return mTransform;}
	float GetVoxelLength() const{return mVoxelLength;}
	const TArray<FCollisionInspectorVoxel>& GetVoxelList() const{return mVoxelList;}
private:
	TArray<FCollisionInspectorVoxel> mVoxelList;
	TMap<int64, FCollisionInspectorVoxel*> mVoxelMap;//mVoxelList依存
	FTransform mTransform;
	float	mVoxelLength = 100.f;
	int32	mVoxelExtentX = 3;
	int32	mVoxelExtentY = 3;
	int32	mVoxelExtentZ = 3;
};