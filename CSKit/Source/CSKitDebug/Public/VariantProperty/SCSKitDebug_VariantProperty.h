// Copyright 2020 megasuraman.
#pragma once

#include "CoreMinimal.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Input/SSpinBox.h"
#include "Widgets/Input/SComboBox.h"

class SCSKitDebug_VectorProperty;
struct FCSKitDebug_VariantPropertyListItem;

DECLARE_DELEGATE_OneParam(FOnChangedVariantProperty, const TSharedPtr<FCSKitDebug_VariantPropertyListItem>&);

// 複数の型の値を扱えるWidget
class SCSKitDebug_VariantProperty : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SCSKitDebug_VariantProperty){}
	SLATE_ARGUMENT(TSharedPtr<FCSKitDebug_VariantPropertyListItem>, Item)
	SLATE_EVENT(FOnChangedVariantProperty, OnChanged)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);
	
protected:
	TSharedPtr<SWidget> CreateBoolWidget();
	TSharedPtr<SWidget> CreateIntWidget();
	TSharedPtr<SWidget> CreateFloatWidget();
	TSharedPtr<SWidget> CreateVectorWidget();
	TSharedPtr<SWidget> CreateStringWidget();
	TSharedPtr<SWidget> CreateStringListWidget();
	TSharedRef<SWidget> CreateStringListComboBoxWidget(TSharedPtr<FString> InOption);
	FText GetStringListComboBoxSelectedOptionText() const;
	void OnChangedValue() const;

private:
	TSharedPtr<FCSKitDebug_VariantPropertyListItem> mItem;
	TSharedPtr<SCheckBox> mBoolWidget;
	TSharedPtr<SSpinBox<int32>> IntSpinBox;
	TSharedPtr<SSpinBox<float>> mFloatWidget;
	TSharedPtr<SCSKitDebug_VectorProperty> mVectorWidget;
	TSharedPtr<SEditableTextBox> mStringWidget;
	TSharedPtr<SComboBox<TSharedPtr<FString>>> mStringListWidget;
	TArray<TSharedPtr<FString>> mStringListWidgetOptions;
	TSharedPtr<FString> mStringListSelectedOption;
	FOnChangedVariantProperty mOnChangedValue;
};