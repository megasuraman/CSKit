// Copyright 2022 megasuraman

#include "PropertyTypeCustomization/CSKitEditor_DataTableRowSelectorCustomization.h"

#include "PropertyEditing.h"
#include "Widgets/Input/SNumericEntryBox.h"
#include "CSKit_DataTableRowSelector.h"

#define LOCTEXT_NAMESPACE "FPropertyCustomizeAssistEditor"

FCSKitEditor_DataTableRowSelectorCustomization::FCSKitEditor_DataTableRowSelectorCustomization()
{
}

void FCSKitEditor_DataTableRowSelectorCustomization::SetupPropertyHandle(const TSharedRef<IPropertyHandle>& StructPropertyHandle)
{
	mResultInputHandle = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FCSKit_DataTableRowSelector, mRowName));
}

void FCSKitEditor_DataTableRowSelectorCustomization::SetupDisplayName(const TSharedRef<IPropertyHandle>& StructPropertyHandle)
{
	const TSharedPtr<IPropertyHandle> DisplayNameHandle = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FCSKit_DataTableRowSelector, mDisplayName));
	if (DisplayNameHandle.IsValid())
	{
		FString DisplayName;
		if (DisplayNameHandle->GetValue(DisplayName) == FPropertyAccess::Success)
		{
			if (!DisplayName.IsEmpty())
			{
				mDisplayName = DisplayName;
			}
		}
	}
}

void FCSKitEditor_DataTableRowSelectorCustomization::SetupStringList(const TSharedRef<IPropertyHandle>& StructPropertyHandle)
{
	TArray<FString> RowNameList;
	if (!AppendRowNameListByPath(RowNameList, StructPropertyHandle))
	{
		if (!AppendRowNameListByStruct(RowNameList, StructPropertyHandle))
		{
			mStringList.Add(MakeShareable(new FString(TEXT("None"))));
			return;
		}
	}

	RowNameList.Sort([](const FString& A, const FString& B) {
		return A < B;
		});

	mStringList.Add(MakeShareable(new FString(TEXT("None"))));
	for (const FString& RowName : RowNameList)
	{
		mStringList.Add(MakeShareable(new FString(RowName)));
	}
}

void FCSKitEditor_DataTableRowSelectorCustomization::SetupSlate(IDetailChildrenBuilder& StructBuilder)
{
	if (mDisplayName.IsEmpty())
	{
		mDisplayName = FString(TEXT("DataTableRowName"));
	}
	FStringComboBoxCustomizationBase::SetupSlate(StructBuilder);
}

bool FCSKitEditor_DataTableRowSelectorCustomization::AppendRowNameListByPath(TArray<FString>& OutList, const TSharedRef<IPropertyHandle>& StructPropertyHandle)
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

	if (mDisplayName.IsEmpty())
	{
		FString LeftString;
		DataTablePath.Split(TEXT("."), &LeftString, &mDisplayName);
	}

	if (const UDataTable* DataTable = LoadObject<UDataTable>(nullptr, (*DataTablePath), nullptr, LOAD_None, nullptr))
	{
		for (const FName& RowName : DataTable->GetRowNames())
		{
			OutList.Add(RowName.ToString());
		}
	}
	return OutList.Num() > 0;
}

bool FCSKitEditor_DataTableRowSelectorCustomization::AppendRowNameListByStruct(TArray<FString>& OutList, const TSharedRef<IPropertyHandle>& StructPropertyHandle)
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

	if (mDisplayName.IsEmpty())
	{
		mDisplayName = DataTableStruct->GetName();
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
				OutList.Add(RowName.ToString());
			}
		}
	}
	return OutList.Num() > 0;
}

#undef LOCTEXT_NAMESPACE