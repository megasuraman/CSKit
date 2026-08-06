// Copyright 2020 megasuraman.
#include "UMG/CSKit_ListViewItem.h"


void UCSKit_ListViewItem::Init(const int32 InIndex)
{
	mIndex = InIndex;
}
void UCSKit_ListViewItem::RequestAction()
{
	OnActionBP();
}