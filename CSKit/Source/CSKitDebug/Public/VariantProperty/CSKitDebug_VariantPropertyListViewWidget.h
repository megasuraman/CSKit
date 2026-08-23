// Copyright 2020 megasuraman.
#pragma once

#include "Components/Widget.h"
#include "CoreMinimal.h"
#include "SCSKitDebug_VariantPropertyListView.h"
#include "CSKitDebug_VariantPropertyListViewWidget.generated.h"

//複数の型を扱うListViewのUMGラッパー
UCLASS()
class UCSKitDebug_VariantPropertyListViewWidget : public UWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "PropertyListView")
	void CreateListByTestValue();
	

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;
	virtual void SynchronizeProperties() override;
	
	void OnChangedListViewValue(const FString& InName, const FCSKit_VariantProperty& InData);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PropertyListView")
	TMap<FString,FCSKit_VariantProperty> mTestValue;
	
private:
	TSharedPtr<SCSKitDebug_VariantPropertyListView> mListView;
};