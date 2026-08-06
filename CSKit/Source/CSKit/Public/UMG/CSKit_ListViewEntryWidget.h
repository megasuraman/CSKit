// Copyright 2020 megasuraman.
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Blueprint/UserWidget.h"
#include "CSKit_ListViewEntryWidget.generated.h"

class UCSKit_ListViewItem;

/**
 *  操作可能なListViewでWidget
 */
UCLASS(abstract)
class UCSKit_ListViewEntryWidget : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()
	
public:
	//Item設定時のBP側の処理(Widget側の更新とか)
	UFUNCTION(BlueprintImplementableEvent, Category = "CSKit_ListView")
	void OnDataSetBP(UCSKit_ListViewItem* InItem);
	//アクション実行
	UFUNCTION(BlueprintCallable, Category = "CSKit_ListView")
	void RequestActionBP();
	UFUNCTION(BlueprintCallable, Category = "CSKit_ListView")
	UCSKit_ListViewItem* GetItemBP() const;

protected:
	//IUserObjectListEntry
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
	//Item設定時の処理
	virtual void OnDataSet(UCSKit_ListViewItem* InItem);
	//管理してるItemのアクション実行
	virtual void RequestAction();
	
private:
	TWeakObjectPtr<UCSKit_ListViewItem> mListItem;
};



