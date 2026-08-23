// Copyright 2020 megasuraman.
#include "VariantProperty/SCSKitDebug_VectorProperty.h"

/**
 * @brief 
 */
void SCSKitDebug_VectorProperty::Construct(const FArguments& InArgs)
{
	mVector = InArgs._Value;
	mOnChangedValue = InArgs._OnChanged;
	mXWidget = CreateFloatWidget(mVector.X);
	mYWidget = CreateFloatWidget(mVector.Y);
	mZWidget = CreateFloatWidget(mVector.Z);
	
	ChildSlot
	[
		SNew(SHorizontalBox)

		// Xの表示と値
		+ SHorizontalBox::Slot()
		.FillWidth(1.0f)
		.VAlign(VAlign_Center)
		.Padding(2.0f)
		[
			SNew(STextBlock)
			.Text(FText::FromString(TEXT("X:")))
		]
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.VAlign(VAlign_Center)
		.Padding(4.0f)
		[
			mXWidget.ToSharedRef()
		]
		
		// Yの表示と値
		+ SHorizontalBox::Slot()
		.FillWidth(1.0f)
		.VAlign(VAlign_Center)
		.Padding(2.0f)
		[
			SNew(STextBlock)
			.Text(FText::FromString(TEXT("Y:")))
		]
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.VAlign(VAlign_Center)
		.Padding(4.0f)
		[
			mYWidget.ToSharedRef()
		]
		
		// Zの表示と値
		+ SHorizontalBox::Slot()
		.FillWidth(1.0f)
		.VAlign(VAlign_Center)
		.Padding(2.0f)
		[
			SNew(STextBlock)
			.Text(FText::FromString(TEXT("Z:")))
		]
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.VAlign(VAlign_Center)
		.Padding(4.0f)
		[
			mZWidget.ToSharedRef()
		]
	];
	
}

/**
 * @brief Float値用のWidget用意
 */
TSharedPtr<SSpinBox<float>> SCSKitDebug_VectorProperty::CreateFloatWidget(const float InValue)
{
	TSharedPtr<SSpinBox<float>> Widget = SNew(SSpinBox<float>)
		.MinDesiredWidth(50.0f)
		.MinSliderValue(-10000.f)
		.MaxSliderValue(10000.f)
		.MinFractionalDigits(1)
		.MaxFractionalDigits(3)
		.Delta(1)
		.Value(InValue)
		.OnValueChanged_Lambda( [this](float NewValue)
		{
			OnChangedValue();
		});
	return Widget;
}

/**
 * @brief 値変化時
 */
void SCSKitDebug_VectorProperty::OnChangedValue()
{
	mVector.X = mXWidget->GetValue();
	mVector.Y = mYWidget->GetValue();
	mVector.Z = mZWidget->GetValue();
	mOnChangedValue.ExecuteIfBound(mVector);
}