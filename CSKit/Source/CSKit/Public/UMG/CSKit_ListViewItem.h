// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CSKit_ListViewItem.generated.h"

/**
 *  操作可能なListViewのItemObject
 */
UCLASS(abstract, Blueprintable)
class UCSKit_ListViewItem : public UObject
{
	GENERATED_BODY()

public:
	//生成直後に呼び出し必須
	void Init(const int32 InIndex);
	//アクション実行
	virtual void RequestAction();
	
public:
	//アクション実行時のBP側の処理
	UFUNCTION(BlueprintImplementableEvent, Category = "CSKit_ListView")
	void OnActionBP();
	//Index取得
	UFUNCTION(BlueprintCallable, Category = "CSKit_ListView")
	int32 GetIndexBP() const {return mIndex;};
	
private:
	int32 mIndex = INDEX_NONE;
};


