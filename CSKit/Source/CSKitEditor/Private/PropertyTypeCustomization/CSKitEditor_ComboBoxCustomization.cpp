// Copyright 2022 megasuraman

#include "PropertyTypeCustomization/CSKitEditor_ComboBoxCustomization.h"

#include "PropertyEditing.h"


#define LOCTEXT_NAMESPACE "FPropertyCustomizeAssistEditor"

FCSKitEditor_ComboBoxCustomization::FCSKitEditor_ComboBoxCustomization()
{
}

void FCSKitEditor_ComboBoxCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> StructPropertyHandle,
	class FDetailWidgetRow& HeaderRow,
	IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	SetupPropertyHandle(StructPropertyHandle);
	SetupDisplayName(StructPropertyHandle);
	SetupStringList(StructPropertyHandle);
}

void FCSKitEditor_ComboBoxCustomization::CustomizeChildren(TSharedRef<class IPropertyHandle> StructPropertyHandle,
	class IDetailChildrenBuilder& StructBuilder,
	IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	SetupSlate(StructBuilder);
}

//Slate準備
void FCSKitEditor_ComboBoxCustomization::SetupSlate(IDetailChildrenBuilder& StructBuilder)
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
		.OnSelectionChanged(this, &FCSKitEditor_ComboBoxCustomization::OnSelectionChanged)
		.OnGenerateWidget(this, &FCSKitEditor_ComboBoxCustomization::OnGenerateWidget)
		[
			SNew(STextBlock)
			.Text(this, &FCSKitEditor_ComboBoxCustomization::GetSelectedTypeText)
		]
	];
}

//コンボボックスの選択変更時処理
void FCSKitEditor_ComboBoxCustomization::OnSelectionChanged(TSharedPtr<FString> Type, ESelectInfo::Type SelectionType) const
{
	if (mResultInputHandle.IsValid()
		&& Type.IsValid())
	{
		mResultInputHandle->SetValue(*Type);
	}
}

//Widget生成時処理
TSharedRef<SWidget> FCSKitEditor_ComboBoxCustomization::OnGenerateWidget(TSharedPtr<FString> Type)
{
	return SNew(STextBlock).Text(FText::FromString(*Type));
}

//コンボボックスで選択中の文字列取得
FText FCSKitEditor_ComboBoxCustomization::GetSelectedTypeText() const
{
	const TSharedPtr<FString> SelectedType = mComboBox->GetSelectedItem();
	if(SelectedType.IsValid())
	{
		return FText::FromString(*SelectedType);
	}
	return FText::FromString(TEXT("None"));
}

//コンボボックスで選択中のIndex取得
int32 FCSKitEditor_ComboBoxCustomization::GetSelectIndex() const
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
void FCSKitEditor_ComboBoxCustomization::CheckActiveResultInput()
{
	if (!mResultInputHandle.IsValid())
	{
		return;
	}

	FName RowName;
	mResultInputHandle->GetValue(RowName);
	if (RowName.IsNone())
	{
		if(!IsMultiSelected())
		{
			mResultInputHandle->SetValue(*mStringList[0]);
		}
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
		if (bInvalidName
			&& !IsMultiSelected())
		{
			mResultInputHandle->SetValue(*mStringList[0]);
		}
	}
}

//複数選択されてる
bool FCSKitEditor_ComboBoxCustomization::IsMultiSelected() const
{
	if(mResultInputHandle->GetNumPerObjectValues() >= 2)
	{
		return true;
	}
	return false;
}

#undef LOCTEXT_NAMESPACE