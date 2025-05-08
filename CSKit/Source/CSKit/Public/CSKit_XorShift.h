// Copyright 2020 megasuraman.
/**
 * @file CSKit_XorShift.h
 * @brief XorShiftによる疑似乱数生成
 *			FMath::Rand(),FRandomStreamではキツイ場面があるので用意
 * @author megasuraman
 * @date 2025/05/05
 */
#pragma once

#include "CoreMinimal.h"

class CSKIT_API CSKit_XorShift
{
public:
	CSKit_XorShift();
	CSKit_XorShift(const uint32 InSeed);
	CSKit_XorShift(const uint32 InSeedX, const uint32 InSeedY, const uint32 InSeedZ, const uint32 InSeedW);

	~CSKit_XorShift();


	uint32	Rand()
	{
		mT = mX ^ (mX << 11);
		mX = mY;
		mY = mZ;
		mZ = mW;
		mW = (mW ^ (mW >> 19)) ^ (mT ^ (mT >> 8));
		return mW;
	}

	int32	RandRange(const int32 InMin = 0, const int32 InMax = 100)
	{
		return Rand() % (InMax - InMin + 1) + InMin;
	}

	float	FRandRange(const float InMin = 0.f, const float InMax = 1.f)
	{
		return (static_cast<float>(Rand() % 0xFFFF) / static_cast<float>(0xFFFF)) * (InMax - InMin) + InMin;
	}

	bool	RandBool()
	{
		return (RandRange(0, 1) == 1);
	}

private:
	uint32	mX = 123456789;
	uint32	mY = 362436069;
	uint32	mZ = 521288629;
	uint32	mW = 88675123;
	uint32	mT = 0;
};
