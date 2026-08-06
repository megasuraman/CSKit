// Copyright 2020 megasuraman.
#include "UMG/CSKit_ListView.h"

#include "UMG/CSKit_ListViewItem.h"

void UCSKit_ListView::CreateItemDefaultBP(const int32 InNum)
{
	UClass* LoadedClass = mItemClass.LoadSynchronous();
	if (LoadedClass == nullptr)
	{
		ensureMsgf(false, TEXT("Invalid Item Class"));
		return;
	}
	for (int32 i = 0; i < InNum; ++i)
	{
		UCSKit_ListViewItem* Item = NewObject<UCSKit_ListViewItem>(this, LoadedClass);
		if (Item == nullptr)
		{
			return;
		}
		Item->Init(i);
		AddItem(Item);
	}
}