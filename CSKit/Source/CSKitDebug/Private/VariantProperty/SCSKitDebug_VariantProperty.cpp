// Copyright 2020 megasuraman.
#include "VariantProperty/SCSKitDebug_VariantProperty.h"

#include "VariantProperty/SCSKitDebug_VariantPropertyListView.h"
#include "VariantProperty/SCSKitDebug_VectorProperty.h"
#include "Widgets/Input/SCheckBox.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Text/STextBlock.h"

/**
 * @brief 
 */
void SCSKitDebug_VariantProperty::Construct(const FArguments& InArgs)
{
	if (!InArgs._Item.IsValid())
	{
		return;
	}
	
	mItem = InArgs._Item;
	mOnChangedValue = InArgs._OnChanged;
	
	TSharedPtr<SWidget> ValueWidget;
	switch (mItem->Data.mType)
	{
	case ECSKit_VariantPropertyType::Bool:
		ValueWidget = CreateBoolWidget();
		break;
	case ECSKit_VariantPropertyType::Int:
		ValueWidget = CreateIntWidget();
		break;
	case ECSKit_VariantPropertyType::Float:
		ValueWidget = CreateFloatWidget();
		break;
	case ECSKit_VariantPropertyType::Vector:
		ValueWidget = CreateVectorWidget();
		break;
	case ECSKit_VariantPropertyType::String:
		ValueWidget = CreateStringWidget();
		break;
	case ECSKit_VariantPropertyType::StringList:
		ValueWidget = CreateStringListWidget();
		break;
	default:
		break;
	}

	if (ValueWidget == nullptr)
	{
		return;
	}
	
	ChildSlot
	[
		SNew(SHorizontalBox)

		// 表示名 スロット
		+ SHorizontalBox::Slot()
		.FillWidth(1.0f)
		.VAlign(VAlign_Center)
		.Padding(4.0f)
		[
			SNew(STextBlock)
			.Text(FText::FromString(mItem->Name))
		]

		// 値 スロット
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.VAlign(VAlign_Center)
		.Padding(4.0f)
		[
			ValueWidget.ToSharedRef()
		]
	];
	
}

/**
 * @brief Bool値用のWidget用意
 */
TSharedPtr<SWidget> SCSKitDebug_VariantProperty::CreateBoolWidget()
{
	mBoolWidget = SNew(SCheckBox)
		.IsChecked(mItem->Data.GetBool() ? ECheckBoxState::Checked : ECheckBoxState::Unchecked)
		.OnCheckStateChanged_Lambda( [this](ECheckBoxState NewState)
		{
			mItem->Data.Set(NewState == ECheckBoxState::Checked);
			OnChangedValue();
		});
	return mBoolWidget;
}

/**
 * @brief Int値用のWidget用意
 */
TSharedPtr<SWidget> SCSKitDebug_VariantProperty::CreateIntWidget()
{
	IntSpinBox = SNew(SSpinBox<int32>)
		.MinDesiredWidth(100.0f)
		.MinSliderValue(-10000)
		.MaxSliderValue(10000)
		.Delta(1)
		.Value(mItem->Data.GetInt())
		.OnValueChanged_Lambda( [this](int32 NewValue)
		{
			mItem->Data.Set(NewValue);
			OnChangedValue();
		});
	return IntSpinBox;
}

/**
 * @brief Float値用のWidget用意
 */
TSharedPtr<SWidget> SCSKitDebug_VariantProperty::CreateFloatWidget()
{
	mFloatWidget = SNew(SSpinBox<float>)
		.MinDesiredWidth(100.0f)
		.MinSliderValue(-10000.f)
		.MaxSliderValue(10000.f)
		.MinFractionalDigits(1)
		.MaxFractionalDigits(3)
		.Delta(1)
		.Value(mItem->Data.GetFloat())
		.OnValueChanged_Lambda( [this](float NewValue)
		{
			mItem->Data.Set(NewValue);
			OnChangedValue();
		});
	return mFloatWidget;
}

TSharedPtr<SWidget> SCSKitDebug_VariantProperty::CreateVectorWidget()
{
	mVectorWidget = SNew(SCSKitDebug_VectorProperty)
		.Value(mItem->Data.GetVector())
		.OnChanged_Lambda( [this](const FVector& NewValue)
		{
			mItem->Data.Set(NewValue);
			OnChangedValue();
		});
	return mVectorWidget;
}

/**
 * @brief String値用のWidget用意
 */
TSharedPtr<SWidget> SCSKitDebug_VariantProperty::CreateStringWidget()
{
	mStringWidget = SNew(SEditableTextBox)
		.MinDesiredWidth(100.0f)
		.Text(FText::FromString(mItem->Data.GetString()))
		.OnTextCommitted_Lambda( [this](const FText& NewText, ETextCommit::Type CommitInfo)
		{
			mItem->Data.Set(NewText.ToString());
			OnChangedValue();
		});
	return mStringWidget;
}

/**
 * @brief StringList値用のWidget用意
 */
TSharedPtr<SWidget> SCSKitDebug_VariantProperty::CreateStringListWidget()
{
	TArray<FString> List;
	int32 SelectIndex = 0;
	mItem->Data.GetStringList(List, SelectIndex);
	mStringListWidgetOptions.Empty();
	for (const FString& String : List)
	{
		mStringListWidgetOptions.Add(MakeShared<FString>(String));
	}
	
	mStringListSelectedOption = mStringListWidgetOptions[SelectIndex];
	
	mStringListWidget = SNew(SComboBox<TSharedPtr<FString>>)
		.OptionsSource(&mStringListWidgetOptions)
		.OnGenerateWidget(this, &SCSKitDebug_VariantProperty::CreateStringListComboBoxWidget)
		.InitiallySelectedItem(mStringListSelectedOption)
		.OnSelectionChanged_Lambda( [this](TSharedPtr<FString> NewSelection, ESelectInfo::Type SelectInfo)
		{
			mStringListSelectedOption = NewSelection;
			mItem->Data.SetStringListSelectIndex(mStringListWidgetOptions.Find( NewSelection));
			OnChangedValue();
		})
		[
			SNew(STextBlock)
			.Text(this, &SCSKitDebug_VariantProperty::GetStringListComboBoxSelectedOptionText)
		];
	return mStringListWidget;
}
/**
 * @brief ComboBoxの選択肢用Widget生成
 */
TSharedRef<SWidget>
SCSKitDebug_VariantProperty::CreateStringListComboBoxWidget(TSharedPtr<FString> InOption)
{
	return SNew(STextBlock)
			.Text(FText::FromString(*InOption));
}
/**
 * @brief ComboBoxの選択中Text
 */
FText SCSKitDebug_VariantProperty::GetStringListComboBoxSelectedOptionText() const
{
	if (mStringListSelectedOption.IsValid())
	{
		return FText::FromString(*mStringListSelectedOption);
	}
	return FText::FromString(TEXT("..."));
}

/**
 * @brief 値変化時
 */
void SCSKitDebug_VariantProperty::OnChangedValue() const
{
	mOnChangedValue.ExecuteIfBound(mItem);
}