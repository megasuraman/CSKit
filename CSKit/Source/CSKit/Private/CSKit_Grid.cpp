// Copyright 2020 megasuraman.
/**
 * @file CSKit_Grid.h
 * @brief 簡易的なGrid管理提供
 * @author megasuraman
 * @date 2025/05/05
 */
#include "CSKit_Grid.h"

FCSKit_GridIndex::FCSKit_GridIndex(const int32 InUID)
{
	ensure(InUID != 0);
	mUID = InUID;
}

FCSKit_GridIndex::FCSKit_GridIndex(const int32 InX, const int32 InY)
{
	ensure(InX <= INT16_MAX && InY <= INT16_MAX
		&& InX >= INT16_MIN && InY >= INT16_MIN
		&& InX != 0 && InY != 0);
	mIndex.mX = InX;
	mIndex.mY = InY;
}

void FCSKit_GridIndex::ShiftIndexX(const bool bInPlus)
{
	if (bInPlus)
	{
		++mIndex.mX;
		if (mIndex.mX == 0)
		{
			++mIndex.mX;
		}
	}
	else
	{
		--mIndex.mX;
		if (mIndex.mX == 0)
		{
			--mIndex.mX;
		}
	}
}
void FCSKit_GridIndex::ShiftIndexY(const bool bInPlus)
{
	if (bInPlus)
	{
		++mIndex.mY;
		if (mIndex.mY == 0)
		{
			++mIndex.mY;
		}
	}
	else
	{
		--mIndex.mY;
		if (mIndex.mY == 0)
		{
			--mIndex.mY;
		}
	}
}

void FCSKit_GridBase::SetGridLength(const float InLength)
{
	if(InLength > 0.f)
	{
		mGridLength = InLength;
		mGridLengthRcp = 1.f/mGridLength;
	}
}

FCSKit_GridIndex FCSKit_GridBase::CalcGridIndex(const FVector& InPos) const
{
	int32 GridX = static_cast<int32>(InPos.X * mGridLengthRcp);
	if (InPos.X > 0.f)
	{
		++GridX;
	}
	else
	{
		--GridX;
	}
	int32 GridY = static_cast<int32>(InPos.Y * mGridLengthRcp);
	if (InPos.Y > 0.f)
	{
		++GridY;
	}
	else
	{
		--GridY;
	}
	return FCSKit_GridIndex(GridX, GridY);
}

void FCSKit_GridBase::CollectAroundGridIndexList(TArray<FCSKit_GridIndex>& OutList, const FCSKit_GridIndex& InBaseGridIndex, const float InRadius) const
{
	const int32 OffsetIndex = FMath::TruncToInt(InRadius/mGridLength) + 1;
	CollectAroundGridIndexList(OutList, InBaseGridIndex, OffsetIndex);
}

void FCSKit_GridBase::CollectAroundGridIndexList(TArray<FCSKit_GridIndex>& OutList, const FCSKit_GridIndex& InBaseGridIndex, const int32 InOffsetIndex)
{
	for (int32 ShiftX = -InOffsetIndex; ShiftX <= InOffsetIndex; ++ShiftX)
	{
		for (int32 ShiftY = -InOffsetIndex; ShiftY <= InOffsetIndex; ++ShiftY)
		{
			if (ShiftX == 0
				&& ShiftY == 0)
			{
				OutList.Add(InBaseGridIndex);
			}
			else
			{
				FCSKit_GridIndex ShiftIndex = InBaseGridIndex;
				if (ShiftX != 0)
				{
					ShiftIndex.ShiftIndexX(ShiftX > 0);
				}
				if (ShiftY != 0)
				{
					ShiftIndex.ShiftIndexY(ShiftY > 0);
				}
				OutList.Add(ShiftIndex);
			}
		}
	}
}