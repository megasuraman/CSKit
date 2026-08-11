// Copyright 2020 megasuraman.
/**
 * @file CSKit_Voxel.h
 * @brief Voxelクラス
 * @author megasuraman
 * @date 2026/08/11
 */
#pragma once

#include "CoreMinimal.h"
#include "CSKit_BinarySerializable.h"

//VoxelのIndex
union FCSKit_VoxelIndex
{
	int64 mUID = INT64_MAX;
	struct IndexXYZ
	{
		int16 mDummy;
		int16 mX;
		int16 mY;
		int16 mZ;
	};
	IndexXYZ mIndex;

	FCSKit_VoxelIndex()
	{
	}
	FCSKit_VoxelIndex(const int64 InUID)
	{
		mUID = InUID;
	}
	FCSKit_VoxelIndex(const int16 InX, const int16 InY, const int16 InZ)
	{
		mIndex.mX = InX;
		mIndex.mY = InY;
		mIndex.mZ = InZ;
	}
	FCSKit_VoxelIndex(const FString& InString)
	{
		FromString(InString);
	}
	bool IsValid() const
	{
		return mUID != INT64_MAX;
	}
	void ShiftIndex(const int16 InX, const int16 InY, const int16 InZ)
	{
		mIndex.mX += InX;
		mIndex.mY += InY;
		mIndex.mZ += InZ;
	}
	void GetAroundVoxelList(TArray<FCSKit_VoxelIndex>& OutList, const int32 InAroundNum = 1) const;
	int32 Distance(const FCSKit_VoxelIndex& InTargetIndex) const;
	int32 DiffMax(const FCSKit_VoxelIndex& InTargetIndex) const;
	FString ToString() const;
	FString ToStringIndex() const;
	void FromString(const FString& InString);

	FCSKit_VoxelIndex operator+(const FCSKit_VoxelIndex& InTarget) const
	{
		FCSKit_VoxelIndex Value(mUID);
		Value.mIndex.mX += InTarget.mIndex.mX;
		Value.mIndex.mY += InTarget.mIndex.mY;
		Value.mIndex.mZ += InTarget.mIndex.mZ;
		return Value;
	}
	FCSKit_VoxelIndex operator-(const FCSKit_VoxelIndex& InTarget) const
	{
		FCSKit_VoxelIndex Value(mUID);
		Value.mIndex.mX -= InTarget.mIndex.mX;
		Value.mIndex.mY -= InTarget.mIndex.mY;
		Value.mIndex.mZ -= InTarget.mIndex.mZ;
		return Value;
	}
	bool operator==(const FCSKit_VoxelIndex InTarget) const
	{
		return mUID == InTarget.mUID;
	}
	bool operator!=(const FCSKit_VoxelIndex InTarget) const
	{
		return mUID != InTarget.mUID;
	}
	friend uint32 GetTypeHash(const FCSKit_VoxelIndex& Key)
	{
		return GetTypeHash(Key.mUID);
	}
	friend FArchive& operator<<(FArchive& Ar, FCSKit_VoxelIndex& Data)
	{
		Ar << Data.mUID;
		return Ar;
	}
};

//Voxel毎に持つ情報
struct FCSKit_Voxel
{
	FCSKit_VoxelIndex mVoxelIndex;
	uint8 mDataVersion = 0;

	virtual ~FCSKit_Voxel()
	{
	}

	void GetAroundVoxelList(TArray<FCSKit_VoxelIndex>& OutList, const int32 InAroundNum = 1) const;

	virtual uint8 GetLastDataVersion() const
	{
		return 0;
	};
	virtual void Serializable(FArchive& Ar)
	{
	}
	friend FArchive& operator<<(FArchive& Ar, FCSKit_Voxel& Data)
	{
		if (Ar.IsSaving())
		{
			Data.mDataVersion = Data.GetLastDataVersion();
		}
		Ar << Data.mDataVersion;
		Ar << Data.mVoxelIndex;
		Data.Serializable(Ar);
		return Ar;
	}
};

struct CSKIT_API FCSKit_VoxelChunkBase : public FCSKit_BinarySerializable
{
public:
	virtual uint8 GetLastVersion() const override
	{
		return 0;
	};
	virtual FString GetFilePath() const override
	{
		return FString();
	};
	virtual void Serializable(FArchive& Ar) override
	{
		Ar << mTransform;
		Ar << mVoxelLength;
		Ar << mVoxelExtentX;
		Ar << mVoxelExtentY;
		Ar << mVoxelExtentZ;
	}
	virtual FCSKit_Voxel* FindVoxelBase(const FCSKit_VoxelIndex& InIndex)
	{
		return nullptr;
	}
	virtual const FCSKit_Voxel* FindVoxelBase(const FCSKit_VoxelIndex& InIndex) const
	{
		return nullptr;
	}
	virtual void OnPostAddVoxel(const FCSKit_VoxelIndex& InIndex)
	{
	}
	virtual bool IsPossibleToAdd(const FCSKit_VoxelIndex& InIndex){return true;}

	void SetVoxelLength(const float InVoxelLength)
	{
		mVoxelLength = InVoxelLength;
	}
	void SetVoxelExtent(const int32 InX, const int32 InY, const int32 InZ)
	{
		mVoxelExtentX = InX;
		mVoxelExtentY = InY;
		mVoxelExtentZ = InZ;
	}
	void SetTransform(const FTransform& InTransform)
	{
		mTransform = InTransform;
	}
	const FTransform& GetTransform() const
	{
		return mTransform;
	}
	float GetVoxelLength() const
	{
		return mVoxelLength;
	}
	FVector GetVoxelPos(const FCSKit_VoxelIndex& InIndex) const;
	FVector GetVoxelPos(const FString& InIndexString) const;
	FCSKit_VoxelIndex GetVoxelIndex(const FVector& InWorldPos) const;
	FString GetVoxelIndexString(const FVector& InWorldPos) const;
	int32 GetVoxelExtentX() const
	{
		return mVoxelExtentX;
	}
	int32 GetVoxelExtentY() const
	{
		return mVoxelExtentY;
	}
	int32 GetVoxelExtentZ() const
	{
		return mVoxelExtentZ;
	}
	bool IsOwnVoxel(const FCSKit_VoxelIndex& InIndex) const
	{
		return FindVoxelBase(InIndex) != nullptr;
	}
	bool IsEdgeVoxel(const FCSKit_VoxelIndex& InIndex) const;

protected:
	FTransform mTransform = FTransform::Identity;
	float mVoxelLength = 100.f;
	int32 mVoxelExtentX = 3;
	int32 mVoxelExtentY = 3;
	int32 mVoxelExtentZ = 3;
};

template <class T = FCSKit_Voxel>
struct FCSKit_VoxelChunk : public FCSKit_VoxelChunkBase
{
public:
	virtual void Serializable(FArchive& Ar) override
	{
		FCSKit_VoxelChunkBase::Serializable(Ar);
		Ar << mVoxelMap;
	}
	virtual FCSKit_Voxel* FindVoxelBase(const FCSKit_VoxelIndex& InIndex) override
	{
		return static_cast<FCSKit_Voxel*>(FindVoxel(InIndex));
	}
	virtual const FCSKit_Voxel* FindVoxelBase(const FCSKit_VoxelIndex& InIndex) const override
	{
		return static_cast<const FCSKit_Voxel*>(FindVoxel(InIndex));
	}
	void AddVoxel(const T& InVoxel)
	{
		if (!IsPossibleToAdd(InVoxel.mVoxelIndex))
		{
			return;
		}
		if (FindVoxel(InVoxel.mVoxelIndex) != nullptr)
		{
			return;
		}
		mVoxelMap.Add(InVoxel.mVoxelIndex.mUID, InVoxel);
		OnPostAddVoxel(InVoxel.mVoxelIndex);
	}
	T* FindVoxel(const FCSKit_VoxelIndex& InIndex)
	{
		if (T* Voxel = mVoxelMap.Find(InIndex.mUID))
		{
			return Voxel;
		}
		return nullptr;
	}
	const T* FindVoxel(const FCSKit_VoxelIndex& InIndex) const
	{
		if (const T* Voxel = mVoxelMap.Find(InIndex.mUID))
		{
			return Voxel;
		}
		return nullptr;
	}
	TMap<int64, T>& GetVoxelMap()
	{
		return mVoxelMap;
	}
	const TMap<int64, T>& GetVoxelMap() const
	{
		return mVoxelMap;
	}
	virtual void ClearVoxel()
	{
		mVoxelMap.Empty();
	}

protected:
	TMap<int64, T> mVoxelMap;
};