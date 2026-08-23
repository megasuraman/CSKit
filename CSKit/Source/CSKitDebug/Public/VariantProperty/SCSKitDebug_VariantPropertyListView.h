// Copyright 2020 megasuraman.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Views/SListView.h"
#include "CSKitDebug_VariantProperty.h"

// リスト要素の情報
struct FCSKitDebug_VariantPropertyListItem
{
	FString Name;
	FCSKit_VariantProperty Data;

	FCSKitDebug_VariantPropertyListItem(){}
	FCSKitDebug_VariantPropertyListItem(const FString& InName, const FCSKit_VariantProperty& InData)
		: Name(InName), Data(InData) {}
};

typedef TSharedPtr<FCSKitDebug_VariantPropertyListItem> FCSKitDebug_VariantPropertyListItemPtr;

DECLARE_DELEGATE_TwoParams(FCSKitDebug_OnChangedValuePropertyListView, const FString&, const FCSKit_VariantProperty&);

//複数の型の値を扱うListView
class SCSKitDebug_VariantPropertyListView : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SCSKitDebug_VariantPropertyListView) {}
	SLATE_EVENT(FCSKitDebug_OnChangedValuePropertyListView, OnChanged)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);
	void SetItems(const TArray<FCSKitDebug_VariantPropertyListItem>& InItems);
	
protected:
	void OnChangedValueRow(const FCSKitDebug_VariantPropertyListItemPtr& InItem);

private:
	//扱うデータ本体
	TArray<FCSKitDebug_VariantPropertyListItemPtr> Items;
	TSharedPtr<SListView<FCSKitDebug_VariantPropertyListItemPtr>> ListView;

	// 3. 各行（Row）の見た目を生成する関数
	TSharedRef<ITableRow> OnGenerateRowForList(FCSKitDebug_VariantPropertyListItemPtr InItem, const TSharedRef<STableViewBase>& OwnerTable);

	// 行が選択された時のコールバック関数の例（任意）
	void OnSelectionChanged(FCSKitDebug_VariantPropertyListItemPtr SelectedItem, ESelectInfo::Type SelectInfo);
private:
	FCSKitDebug_OnChangedValuePropertyListView mOnChangedValue;
};
