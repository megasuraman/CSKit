// Copyright 2020 megasuraman.

#pragma once

#include "CoreMinimal.h"
#include "Components/ListView.h"
#include "CSKit_ListView.generated.h"

class UCSKit_ListViewItem;
/**
 *  ItemとListを紐づけて操作可能なListView
 *  最低限の使い方
 *	1. UCSKit_ListViewItemを継承して扱いたいパラメータをもたせる
 *	2. UCSKit_ListViewEntryWidgetを継承してList内の表示を用意
 *		OnDataSetBP()でItemに応じて表示を更新
 *		ボタン等で何か処理したい場合はUCSKit_ListViewItem::RequestActionBP()を呼ぶ
 *	3. UCSKit_ListViewを継承してListView用意
 *		mItemClass に 1 のクラスを指定
 *		EntryWidgetClass に 2 のクラスを指定
 *	4. Widgetに3を配置
 *		コンストラクタで UCSKit_ListView::CreateItemDefaultBP() 等でAddItemする
 */
UCLASS(BlueprintType,Blueprintable)
class UCSKit_ListView : public UListView
{
	GENERATED_BODY()
	
public:
	//Widgetのコンストラクタで実行
	UFUNCTION(BlueprintCallable, Category = "CSKit_ListView")
	void CreateItemDefaultBP(const int32 InNum);

protected:
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category = "CSKit_ListView")
	TSoftClassPtr<UCSKit_ListViewItem> mItemClass = nullptr;
};



