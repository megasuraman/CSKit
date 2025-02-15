// Copyright 2022 megasuraman

#include "PropertyTypeCustomization/CSKitEditor_ComboBoxCustomization.h"

#include "PropertyEditing.h"
#include "Widgets/Input/SNumericEntryBox.h"
#include "CSKit_DataTableRowSelector.h"


#define LOCTEXT_NAMESPACE "FPropertyCustomizeAssistEditor"

FStringComboBoxCustomizationBase::FStringComboBoxCustomizationBase()
{
}

void FStringComboBoxCustomizationBase::CustomizeHeader(TSharedRef<IPropertyHandle> StructPropertyHandle,
	class FDetailWidgetRow& HeaderRow,
	IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	SetupPropertyHandle(StructPropertyHandle);
	SetupDisplayName(StructPropertyHandle);
	SetupStringList(StructPropertyHandle);
}

void FStringComboBoxCustomizationBase::CustomizeChildren(TSharedRef<class IPropertyHandle> StructPropertyHandle,
	class IDetailChildrenBuilder& StructBuilder,
	IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	SetupSlate(StructBuilder);
}

//Slate準備
void FStringComboBoxCustomizationBase::SetupSlate(IDetailChildrenBuilder& StructBuilder)
{
	if (mStringList.Num() <= 0)
	{
		return;
	}
	CheckActiveResultInput();

	StructBuilder.AddCustomRow(LOCTEXT("TypeRow", "TypeRow"))
	.NameContent()
	[
		SNew(STextBlock)
		.Text(FText::Format(LOCTEXT("TypeTitle", "{0}"), FText::FromString(mDisplayName)))
	]
	.ValueContent()
	[
		SAssignNew(mComboBox, SSearchableComboBox)
		.OptionsSource(&mStringList)
		.InitiallySelectedItem(mStringList[GetSelectIndex()])
		.OnSelectionChanged(this, &FStringComboBoxCustomizationBase::OnSelectionChanged)
		.OnGenerateWidget(this, &FStringComboBoxCustomizationBase::OnGenerateWidget)
		[
			SNew(STextBlock)
			.Text(this, &FStringComboBoxCustomizationBase::GetSelectedTypeText)
		]
	];
}

//コンボボックスの選択変更時処理
// ReSharper disable once CppMemberFunctionMayBeConst
// ReSharper disable once CppPassValueParameterByConstReference
void FStringComboBoxCustomizationBase::OnSelectionChanged(TSharedPtr<FString> Type, ESelectInfo::Type SelectionType)
{
	if (mResultInputHandle.IsValid()
		&& Type.IsValid())
	{
		mResultInputHandle->SetValue(*Type);
	}
}

//Widget生成時処理
// ReSharper disable once CppMemberFunctionMayBeConst
// ReSharper disable once CppPassValueParameterByConstReference
TSharedRef<SWidget> FStringComboBoxCustomizationBase::OnGenerateWidget(TSharedPtr<FString> Type)
{
	return SNew(STextBlock).Text(FText::FromString(*Type));
}

//コンボボックスで選択中の文字列取得
FText FStringComboBoxCustomizationBase::GetSelectedTypeText() const
{
	const TSharedPtr<FString> SelectedType = mComboBox->GetSelectedItem();
	if(SelectedType.IsValid())
	{
		return FText::FromString(*SelectedType);
	}
	return FText::FromString(TEXT("None"));
}

//コンボボックスで選択中のIndex取得
int32 FStringComboBoxCustomizationBase::GetSelectIndex() const
{
	if (!mResultInputHandle.IsValid())
	{
		return 0;
	}
	FName RowName;
	if (mResultInputHandle->GetValue(RowName) != FPropertyAccess::Success)
	{
		return 0;
	}

	for (int32 i = 0; i < mStringList.Num(); ++i)
	{
		if (*mStringList[i] == RowName.ToString())
		{
			return i;
		}
	}
	return 0;
}

//出力先プロパティに適切な文字列セットされてるかチェック
void FStringComboBoxCustomizationBase::CheckActiveResultInput()
{
	if (!mResultInputHandle.IsValid())
	{
		return;
	}

	FName RowName;
	mResultInputHandle->GetValue(RowName);
	if (RowName.IsNone())
	{
		mResultInputHandle->SetValue(*mStringList[0]);
	}
	else
	{
		bool bInvalidName = true;
		for (int32 i = 0; i < mStringList.Num(); ++i)
		{
			if (*mStringList[i] == RowName.ToString())
			{
				bInvalidName = false;
				break;
			}
		}
		if (bInvalidName)
		{
			mResultInputHandle->SetValue(*mStringList[0]);
		}
	}
}

#undef LOCTEXT_NAMESPACE