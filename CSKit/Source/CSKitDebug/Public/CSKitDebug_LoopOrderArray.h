// Copyright 2020 megasuraman
/**
 * @file CSKitDebug_LoopOrderArray.h
 * @brief TArrayに連番で追加して一定サイズで使い回すためのテンプレート
 * @author megasuraman
 * @date 2022/3/19
 */

#pragma once

#include "CoreMinimal.h"

template<typename InElementType>
class TCSKitDebug_LoopOrderArray
{
public:
	typedef InElementType ElementType;

	TCSKitDebug_LoopOrderArray(int32 InSize)
	{
		mList.Reserve(InSize);
	}

	void	ChangeSize(const int32 InSize)
	{
		mList.Empty();
		mList.Reserve(InSize);
		mNextIndex = 0;
	}

	void	Clear()
	{
		const int32 MaxSize = mList.Max();
		mList.Empty();
		mList.Reserve(MaxSize);
		mNextIndex = 0;
	}

	//要素追加(既に最大数だったら古いのを上書き)
	ElementType&	Push(const ElementType& InElement)
	{
		check(mList.Max() > 0);
		if (mList.Num() < mList.Max())
		{
			mList.AddZeroed();
		}
		ElementType& AddElement = mList[mNextIndex];
		AddElement = InElement;

		mNextIndex = (mNextIndex + 1) % mList.Max();
		return AddElement;
	}

	const TArray<ElementType>& GetList() const { return mList; }
	const int32		GetListNum() const { return mList.Num(); }
	const int32		GetListMaxNum() const { return mList.Max(); }
	const int32		GetNextIndex() const { return mNextIndex; }
	//最後に追加した要素のListIndex取得
	const int32		GetLastListIndex() const
	{
		const int32 ListNum = GetListNum();
		check(ListNum > 0);
		int32 LastIndex = ListNum - 1;
		if (ListNum != mNextIndex)
		{
			LastIndex = (mNextIndex + ListNum - 1) % ListNum;
		}
		return LastIndex;
	}
	const ElementType&	GetLast() const
	{
		return mList[GetLastListIndex()];
	}
	//指定の順番のListIndexを取得
	const int32		GetOrderListIndex(const int32 InOrderIndex) const
	{
		const int32 ListNum = GetListNum();
		check(ListNum > 0);
		int32 ListIndex = InOrderIndex;
		if (ListNum != mNextIndex)
		{
			ListIndex = (InOrderIndex + mNextIndex) % ListNum;
		}
		return ListIndex;
	}
	//指定の順番の要素を取得
	const ElementType&	GetOrder(const int32 InOrderIndex) const
	{
		return mList[GetOrderListIndex(InOrderIndex)];
	}

protected:
	TArray<ElementType>	mList;
	int32	mNextIndex = 0;
};