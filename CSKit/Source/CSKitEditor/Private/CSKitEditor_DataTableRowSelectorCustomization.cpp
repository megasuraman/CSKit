// Copyright 2022 megasuraman


#include "CSKitEditor_DataTableRowSelectorCustomization.h"
#include "PropertyEditing.h"
#include "Widgets/Input/SNumericEntryBox.h"
#include "CSKit_DataTableRowSelector.h"

#define LOCTEXT_NAMESPACE "FCSKitEditor"

FCSKitEditor_DataTableRowSelectorCustomization::FCSKitEditor_DataTableRowSelectorCustomization()
{
}

void FCSKitEditor_DataTableRowSelectorCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> StructPropertyHandle,
	class FDetailWidgetRow& HeaderRow,
	IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	mRowNameHandle = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FCSKit_DataTableRowSelector, mRowName));

	SetupDisplayName(StructPropertyHandle);
	SetupRowNameList(StructPropertyHandle);
}

void FCSKitEditor_DataTableRowSelectorCustomization::CustomizeChildren(TSharedRef<class IPropertyHandle> StructPropertyHandle,
	class IDetailChildrenBuilder& StructBuilder,
	IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	if (mRowNameList.Num() <= 0)
	{
		return;
	}

	if (mRowNameHandle.IsValid())
	{
		FName RowName;
		mRowNameHandle->GetValue(RowName);
		if (RowName.IsNone())
		{
			mRowNameHandle->SetValue(*mRowNameList[0]);
		}
		else
		{
			bool bInvalidName = true;
			for (int32 i = 0; i < mRowNameList.Num(); ++i)
			{
				if (*mRowNameList[i] == RowName.ToString())
				{
					bInvalidName = false;
					break;
				}
			}
			if (bInvalidName)
			{
				mRowNameHandle->SetValue(*mRowNameList[0]);
			}
		}
	}

	StructBuilder.AddCustomRow(LOCTEXT("TypeRow", "TypeRow"))
	.NameContent()
	[
		SNew(STextBlock)
		.Text(FText::Format(LOCTEXT("TypeTitle", "{0}"), FText::FromString(mPropertyTitleName)))
	]
	.ValueContent()
	[
		SAssignNew(mComboBox, SComboBox<TestSelectTypePtr>)
		.OptionsSource(&mRowNameList)
		.InitiallySelectedItem(mRowNameList[GetSelectIndex()])
		.OnSelectionChanged(this, &FCSKitEditor_DataTableRowSelectorCustomization::OnSelectionChanged)
		.OnGenerateWidget(this, &FCSKitEditor_DataTableRowSelectorCustomization::OnGenerateWidget)
		[
			SNew(STextBlock)
			.Text(this, &FCSKitEditor_DataTableRowSelectorCustomization::GetSelectedTypeText)
		]
	];
}


void FCSKitEditor_DataTableRowSelectorCustomization::OnSelectionChanged(TestSelectTypePtr Type, ESelectInfo::Type SelectionType)
{
	if (mRowNameHandle.IsValid())
	{
		mRowNameHandle->SetValue(*Type);
	}
}

TSharedRef<SWidget> FCSKitEditor_DataTableRowSelectorCustomization::OnGenerateWidget(TestSelectTypePtr Type)
{
	return SNew(STextBlock).Text(FText::FromString(*Type));
}
FText FCSKitEditor_DataTableRowSelectorCustomization::GetSelectedTypeText() const
{
	TestSelectTypePtr SelectedType = mComboBox->GetSelectedItem();
	return FText::FromString(*SelectedType);
}
int32 FCSKitEditor_DataTableRowSelectorCustomization::GetSelectIndex() const
{
	if (!mRowNameHandle.IsValid())
	{
		return 0;
	}
	FName RowName;
	if (mRowNameHandle->GetValue(RowName) != FPropertyAccess::Success)
	{
		return 0;
	}

	for (int32 i = 0; i < mRowNameList.Num(); ++i)
	{
		if (*mRowNameList[i] == RowName.ToString())
		{
			return i;
		}
	}
	return 0;
}

void FCSKitEditor_DataTableRowSelectorCustomization::SetupDisplayName(TSharedRef<IPropertyHandle> StructPropertyHandle)
{
	const TSharedPtr<IPropertyHandle> DisplayNameHandle = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FCSKit_DataTableRowSelector, mDisplayName));
	if (DisplayNameHandle.IsValid())
	{
		FString DisplayName;
		if (DisplayNameHandle->GetValue(DisplayName) == FPropertyAccess::Success)
		{
			if (!DisplayName.IsEmpty())
			{
				mPropertyTitleName = DisplayName;
			}
		}
	}
}

void FCSKitEditor_DataTableRowSelectorCustomization::SetupRowNameList(TSharedRef<IPropertyHandle> StructPropertyHandle)
{
	if (SetupRowNameListByStruct(StructPropertyHandle))
	{
		return;
	}
	SetupRowNameListByPath(StructPropertyHandle);
}
bool FCSKitEditor_DataTableRowSelectorCustomization::SetupRowNameListByStruct(TSharedRef<IPropertyHandle> StructPropertyHandle)
{
	const TSharedPtr<IPropertyHandle> DataTableStructHandle = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FCSKit_DataTableRowSelector, mDataTableStruct));
	if (!DataTableStructHandle.IsValid())
	{
		return false;
	}
	UObject* TargetObject;
	if (DataTableStructHandle->GetValue(TargetObject) != FPropertyAccess::Success)
	{
		return false;
	}
	const UStruct* DataTableStruct = Cast<UStruct>(TargetObject);
	if (DataTableStruct == nullptr)
	{
		return false;
	}

	if (mPropertyTitleName.IsEmpty())
	{
		mPropertyTitleName = DataTableStruct->GetName();
	}
	for (TObjectIterator<UDataTable> It; It; ++It)
	{
		const UDataTable* DataTableAsset = *It;
		if (DataTableAsset == nullptr)
		{
			continue;
		}
		if (DataTableAsset->RowStruct->IsChildOf(DataTableStruct))
		{
			for (const FName& RowName : DataTableAsset->GetRowNames())
			{
				mRowNameList.Add(MakeShareable(new FString(RowName.ToString())));
			}
		}
	}
	return mRowNameList.Num() > 0;
}

bool FCSKitEditor_DataTableRowSelectorCustomization::SetupRowNameListByPath(TSharedRef<IPropertyHandle> StructPropertyHandle)
{
	const TSharedPtr<IPropertyHandle> DataTablePathHandle = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FCSKit_DataTableRowSelector, mDataTablePath));
	if (!DataTablePathHandle.IsValid())
	{
		return false;
	}
	FString DataTablePath;
	if (DataTablePathHandle->GetValue(DataTablePath) != FPropertyAccess::Success)
	{
		return false;
	}

	if (mPropertyTitleName.IsEmpty())
	{
		FString LeftString;
		DataTablePath.Split(TEXT("."), &LeftString, &mPropertyTitleName);
	}

	if (const UDataTable* DataTable = LoadObject<UDataTable>(nullptr, (*DataTablePath), nullptr, LOAD_None, nullptr))
	{
		for (const FName& RowName : DataTable->GetRowNames())
		{
			mRowNameList.Add(MakeShareable(new FString(RowName.ToString())));
		}
	}
	return mRowNameList.Num() > 0;
}

#undef LOCTEXT_NAMESPACE