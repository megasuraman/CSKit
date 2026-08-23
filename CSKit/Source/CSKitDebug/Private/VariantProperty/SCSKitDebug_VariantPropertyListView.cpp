// Copyright 2020 megasuraman.
#include "VariantProperty/SCSKitDebug_VariantPropertyListView.h"

#include "VariantProperty/SCSKitDebug_VariantProperty.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Views/STableRow.h"

/**
 * @brief 
 */
void SCSKitDebug_VariantPropertyListView::Construct(const FArguments& InArgs)
{
	mOnChangedValue = InArgs._OnChanged;

	ChildSlot
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.FillHeight(1.0f)
		[
			// SListView の構築
			SAssignNew(ListView, SListView<FCSKitDebug_VariantPropertyListItemPtr>)
			.ItemHeight(24.0f)											 // 1行あたりの高さ
			.ListItemsSource(&Items)									   // データ配列の参照を設定
			.OnGenerateRow(this, &SCSKitDebug_VariantPropertyListView::OnGenerateRowForList) // 各行の表示生成処理
			.OnSelectionChanged(this, &SCSKitDebug_VariantPropertyListView::OnSelectionChanged) // 選択変更時イベント
			.SelectionMode(ESelectionMode::Single)						 // 選択モード (Single, Multi, None など)
		]
	];
}

/**
 * @brief ListViewで扱うデータ設定
 */
void SCSKitDebug_VariantPropertyListView::SetItems(const TArray<FCSKitDebug_VariantPropertyListItem> &InItems)
{
	Items.Empty();
	for (const FCSKitDebug_VariantPropertyListItem& Item : InItems)
	{
		Items.Add(MakeShared<FCSKitDebug_VariantPropertyListItem>(Item.Name, Item.Data));
	}
	if (ListView.IsValid())
	{
		ListView->ClearSelection();
		ListView->RequestListRefresh();
	}
}
/**
 * @brief ListViewのRowの値が変更された時
 */
void SCSKitDebug_VariantPropertyListView::OnChangedValueRow(const FCSKitDebug_VariantPropertyListItemPtr &InItem)
{
	if (InItem.IsValid())
	{
		FCSKitDebug_VariantPropertyListItem *Item = InItem.Get();
		//UE_LOG(LogTemp, Log, TEXT("Changed Value : %s"), *Item->Name);
		mOnChangedValue.ExecuteIfBound(Item->Name, Item->Data);
	}
}

/**
 * @brief ListViewの行Widget生成
 *		スクロール時は新たに行が表示される度に呼ばれる
 */
TSharedRef<ITableRow> SCSKitDebug_VariantPropertyListView::OnGenerateRowForList(FCSKitDebug_VariantPropertyListItemPtr InItem, const TSharedRef<STableViewBase>& OwnerTable)
{
	return SNew(STableRow<FCSKitDebug_VariantPropertyListItemPtr>, OwnerTable)
	[
		SNew(SCSKitDebug_VariantProperty)
		.Item(InItem)
		.OnChanged(this, &SCSKitDebug_VariantPropertyListView::OnChangedValueRow)
	];
}

/**
 * @brief ListViewの行選択時
 */
void SCSKitDebug_VariantPropertyListView::OnSelectionChanged(FCSKitDebug_VariantPropertyListItemPtr SelectedItem, ESelectInfo::Type SelectInfo)
{
	if (SelectedItem.IsValid())
	{
		FCSKitDebug_VariantPropertyListItem *Item = SelectedItem.Get();
		//UE_LOG(LogTemp, Log, TEXT("Selected: %s"), *Item->Name);
	}
}