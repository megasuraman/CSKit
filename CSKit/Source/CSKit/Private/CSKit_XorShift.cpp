// Copyright 2020 megasuraman.
/**
 * @file CSKit_XorShift.h
 * @brief XorShiftによる疑似乱数生成
 *			FMath::Rand(),FRandomStreamではキツイ場面があるので用意
 * @author megasuraman
 * @date 2025/05/05
 */
#include "CSKit_XorShift.h"

DEFINE_LOG_CATEGORY_STATIC(LogCSKit_XorShift, Warning, All);

CSKit_XorShift::CSKit_XorShift()
{
}

CSKit_XorShift::CSKit_XorShift(const uint32 InSeed)
{
	UE_LOG(LogCSKit_XorShift, Log, TEXT("XorShift Seed : %d"), InSeed);
	mW = InSeed;
	mX = InSeed << 13;
	mY = (InSeed >> 9) ^ (mX << 6);
	mZ = mY >> 7;
}

CSKit_XorShift::CSKit_XorShift(const uint32 InSeedX, const uint32 InSeedY, const uint32 InSeedZ, const uint32 InSeedW)
	:mX(InSeedX)
	, mY(InSeedY)
	, mZ(InSeedZ)
	, mW(InSeedW)
{
	UE_LOG(LogCSKit_XorShift, Log, TEXT("XorShift Seed : %d %d %d %d"), mX, mY, mZ, mW);
}

CSKit_XorShift::~CSKit_XorShift()
{
}
