// Copyright 2020 megasuraman.
#include "VariantProperty/CSKitDebug_VariantPropertyListViewWidget.h"

/**
 * @brief mTestValueを元にListViewの中身を設定
 */
void UCSKitDebug_VariantPropertyListViewWidget::CreateListByTestValue()
{
	TArray<FCSKitDebug_VariantPropertyListItem> ItemList;
	for (const auto& MapElement : mTestValue)
	{
		FCSKitDebug_VariantPropertyListItem Item;
		Item.Name = MapElement.Key;
		Item.Data = MapElement.Value;
		ItemList.Add(Item);
	}
	mListView->SetItems(ItemList);
}

/**
 * @brief SlateでWidgetを生成して見た目を構築
 */
TSharedRef<SWidget> UCSKitDebug_VariantPropertyListViewWidget::RebuildWidget()
{
	mListView = SNew(SCSKitDebug_VariantPropertyListView)
		.OnChanged_UObject(this, &UCSKitDebug_VariantPropertyListViewWidget::OnChangedListViewValue);

	return mListView.ToSharedRef();
}

/**
 * @brief メモリ破棄の管理
 */
void UCSKitDebug_VariantPropertyListViewWidget::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);

	// SlateのWidgetの参照解除
	mListView.Reset();
}

/**
 * @brief プロパティ変更時にSlate側の状態を更新したい処理
 */
void UCSKitDebug_VariantPropertyListViewWidget::SynchronizeProperties()
{
	Super::SynchronizeProperties();
}

/**
 * @brief ListView内の値変更時
 */
void UCSKitDebug_VariantPropertyListViewWidget::OnChangedListViewValue(
	const FString &InName,
	const FCSKit_VariantProperty &InData
	)
{
	UE_LOG(LogTemp, Log, TEXT("ChangedValueListView : %s(%s)"), *InName, *InData.mDataString);
}