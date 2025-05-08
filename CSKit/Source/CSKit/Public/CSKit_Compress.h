// Copyright 2020 megasuraman.
/**
 * @file CSKit_Compress.h
 * @brief 簡易的なデータ圧縮関連
 * @author megasuraman
 * @date 2025/05/05
 */
#pragma once

#include "CoreMinimal.h"

//8bit(0～200)で0.0～1.0を表現
struct FCSKit_CompressOneFloat
{
	uint8 mData = 0;

	float Get() const
	{
		return static_cast<float>(mData) * 0.005f;
	}
	void Set(const float InValue)
	{
		mData = static_cast<uint8>(FMath::Clamp(static_cast<int32>(InValue * 200.f),0,200));
	}
	uint8 GetData() const { return mData; }
	void SetData(const uint8 InData){mData = InData;}
	bool IsOne() const{return mData==200;}
	FCSKit_CompressOneFloat(){}
	FCSKit_CompressOneFloat(const uint8 InData)
		:mData(InData)
	{}
	FCSKit_CompressOneFloat(const float InValue)
	{
		Set(InValue);
	}
};

//32bitで0.0～1.0のx,y,zのVectorを表現
union FCSKit_CompressOneVector
{
	int32 mData = 0;
	struct FCompressVector
	{//0.0～1.0を0～1000で表現
		int32 mX : 10;
		int32 mY : 10;
		int32 mZ : 10;
		int32 :2;

		FVector GetVector() const
		{
			return FVector(
				static_cast<float>(mX) * 0.001f,
				static_cast<float>(mY) * 0.001f,
				static_cast<float>(mZ) * 0.001f
			);
		}
		void SetVector(const FVector& InVector)
		{
			mX = FMath::Clamp(static_cast<int32>(InVector.X * 1000.f), 0, 1000);
			mY = FMath::Clamp(static_cast<int32>(InVector.Y * 1000.f), 0, 1000);
			mZ = FMath::Clamp(static_cast<int32>(InVector.Z * 1000.f), 0, 1000);
		}
	};
	FCompressVector	mCompressVector;

	void	Set(const FVector& InVector)
	{
		mCompressVector.SetVector(InVector);
	}
	FVector	Get() const
	{
		return mCompressVector.GetVector();
	}
	int32	GetData() const{return mData;}
	void	SetData(const int32 InData)
	{
		mData = InData;
	}
};

//int32のTArrayをbit配列代わりにする
struct FCSKit_CompressBitArray
{
	static bool	GetBool(const TArray<int32>& InArray, const int32 InIndex)
	{
		const int32 ArrayIndex = InIndex/32;
		if (ArrayIndex >= InArray.Num())
		{
			return false;
		}
		const int32 TargetInt = InArray[ArrayIndex];
		const int32 IndexShift = InIndex - ArrayIndex*32;
		return !!(TargetInt & 1<<IndexShift);
	}
	static void	SetBool(TArray<int32>& OutArray, const int32 InIndex, const bool bInBool)
	{
		const int32 ArrayIndex = InIndex / 32;
		while (ArrayIndex >= OutArray.Num())
		{
			OutArray.Add(0);
		}
		int32& TargetInt = OutArray[ArrayIndex];
		const int32 IndexShift = InIndex - ArrayIndex * 32;
		if(bInBool)
		{
			TargetInt = TargetInt | 1 << IndexShift;
		}
		else
		{
			TargetInt = TargetInt & ~(1 << IndexShift);
		}
	}
};

//2byteのVector(小数点切り捨て & 2^20制限)
struct FCSKit_CompressInt64Vector
{
	int64 mValue = 0;

	union FCalculator
	{
		int64 mBitValue = 0;
		struct FIntVector
		{
			int64 mX : 21;
			int64 mY : 21;
			int64 mZ : 21;
		};
		FIntVector IntVector;
	};

	FCSKit_CompressInt64Vector(){}
	FCSKit_CompressInt64Vector(const FVector& InPos)
	{
		Set(InPos);
	}
	FCSKit_CompressInt64Vector(const int64& InData)
	{
		mValue = InData;
	}

	FVector	Get() const
	{
		FCalculator Calculator;
		Calculator.mBitValue = mValue;
		return FVector(static_cast<double>(Calculator.IntVector.mX),
			static_cast<double>(Calculator.IntVector.mY),
			static_cast<double>(Calculator.IntVector.mZ));
	}
	void Set(const FVector& InPos)
	{
		check( FMath::Abs(InPos.X)<1048576.0
			&& FMath::Abs(InPos.Y)<1048576.0
			&& FMath::Abs(InPos.Z)<1048576.0
			);
		FCalculator Calculator;
		Calculator.IntVector.mX = static_cast<int32>(InPos.X);
		Calculator.IntVector.mY = static_cast<int32>(InPos.Y);
		Calculator.IntVector.mZ = static_cast<int32>(InPos.Z);
		mValue = Calculator.mBitValue;
	}
};