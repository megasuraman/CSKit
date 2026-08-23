// Copyright 2020 megasuraman.
#pragma once

#include "CoreMinimal.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Input/SSpinBox.h"
#include "Widgets/Input/SComboBox.h"

DECLARE_DELEGATE_OneParam(FCSKitDebug_OnChangedVectorProperty, const FVector&);

// Vector型の値を扱えるWidget
class SCSKitDebug_VectorProperty : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SCSKitDebug_VectorProperty){}
	SLATE_ARGUMENT(FVector, Value)
	SLATE_EVENT(FCSKitDebug_OnChangedVectorProperty, OnChanged)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);
	const FVector& GetVector() const { return mVector; }
	
protected:
	TSharedPtr<SSpinBox<float>> CreateFloatWidget(const float InValue);
	void OnChangedValue();

private:
	FVector mVector;
	TSharedPtr<SSpinBox<float>> mXWidget;
	TSharedPtr<SSpinBox<float>> mYWidget;
	TSharedPtr<SSpinBox<float>> mZWidget;
	FCSKitDebug_OnChangedVectorProperty mOnChangedValue;
};