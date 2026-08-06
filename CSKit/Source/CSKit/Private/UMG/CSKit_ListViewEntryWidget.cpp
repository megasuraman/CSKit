// Copyright Epic Games, Inc. All Rights Reserved.

#include "UMG/CSKit_ListViewEntryWidget.h"

#include "UMG/CSKit_ListViewItem.h"

void UCSKit_ListViewEntryWidget::RequestActionBP()
{
	RequestAction();
}
UCSKit_ListViewItem* UCSKit_ListViewEntryWidget::GetItemBP() const
{
	return mListItem.Get();
}

void UCSKit_ListViewEntryWidget::NativeOnListItemObjectSet(UObject *ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);
	if (UCSKit_ListViewItem* ItemBase = Cast<UCSKit_ListViewItem>(ListItemObject))
	{
		OnDataSet( ItemBase);
	}
}
void UCSKit_ListViewEntryWidget::OnDataSet(UCSKit_ListViewItem *InItem)
{
	mListItem = InItem;
	OnDataSetBP(InItem);
}
void UCSKit_ListViewEntryWidget::RequestAction()
{
	if (UCSKit_ListViewItem* ItemBase = mListItem.Get())
	{
		ItemBase->RequestAction();
	}
}