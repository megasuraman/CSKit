// Copyright 2020 megasuraman.
/**
 * @file CSKitEditor_BrainQueryTestSelectorCustomization.cpp
 * @brief CSKit_BrainQueryTestSelectorのプロパティをDataTableのRowNameのComboBox化する
 * @author megasuraman
 * @date 2025/05/05
 */
#include "PropertyTypeCustomization/CSKitEditor_BrainQueryTestSelectorCustomization.h"
#include "PropertyEditing.h"
#include "AI/BrainQuery/CSKit_BrainQueryDataTable.h"
#include "AI/BrainQuery/CSKit_BrainQueryTestSelector.h"
#include "SSearchableComboBox.h"

#define LOCTEXT_NAMESPACE "FPropertyCustomizeAssistEditor"

FCSKitEditor_BrainQueryTestSelectorCustomization::FCSKitEditor_BrainQueryTestSelectorCustomization()
{
}

void FCSKitEditor_BrainQueryTestSelectorCustomization::SetupPropertyHandle(const TSharedRef<IPropertyHandle>& StructPropertyHandle)
{
	mResultInputHandle = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FCSKit_BrainQueryTestSelector, mTestName));
	mDataTablePathHandle  = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FCSKit_BrainQueryTestSelector, mEditorDataTablePathList));
	mRowNameHandle = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FCSKit_BrainQueryTestSelector, mEditorDataTableRowName));;
	if (mRowNameHandle.IsValid())
	{
		mRowNameHandle->SetOnPropertyValueChanged(
			FSimpleDelegate::CreateSP(this, &FCSKitEditor_BrainQueryTestSelectorCustomization::OnChangeRowName)
		);
	}
}

void FCSKitEditor_BrainQueryTestSelectorCustomization::SetupDisplayName(const TSharedRef<IPropertyHandle>& StructPropertyHandle)
{
	const TSharedPtr<IPropertyHandle> DisplayNameHandle = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FCSKit_BrainQueryTestSelector, mEditorDisplayName));
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

void FCSKitEditor_BrainQueryTestSelectorCustomization::SetupStringList(const TSharedRef<IPropertyHandle>& StructPropertyHandle)
{
	TArray<FString> RowNameList;
	if (!AppendRowNameListByPath(RowNameList))
	{
		mStringList.Add(MakeShareable(new FString(TEXT("None"))));
		return;
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

void FCSKitEditor_BrainQueryTestSelectorCustomization::SetupSlate(IDetailChildrenBuilder& StructBuilder)
{
	StructBuilder.AddProperty(mRowNameHandle.ToSharedRef());

	FCSKitEditor_ComboBoxCustomization::SetupSlate(StructBuilder);

	if (mDisplayName.IsEmpty())
	{
		mDisplayName = FString(TEXT("DataTableRowName"));
	}
}

//DataTableのフルパスから文字列取得
bool FCSKitEditor_BrainQueryTestSelectorCustomization::AppendRowNameListByPath(TArray<FString>& OutList)
{
	if (!mDataTablePathHandle.IsValid())
	{
		return false;
	}

	const TSharedPtr<IPropertyHandleArray> HandleArray = mDataTablePathHandle->AsArray();
	uint32 NumItem = 0;
	if (HandleArray->GetNumElements(NumItem) != FPropertyAccess::Success)
	{
		return false;
	}

	const FName TargetRowName = GetRowName();
	TArray<FString> RowNameList;
	for (uint32 i = 0; i < NumItem; ++i)
	{
		const TSharedRef<IPropertyHandle> ElementHandle = HandleArray->GetElement(i);
		FString DataTablePath;
		if (ElementHandle->GetValue(DataTablePath) != FPropertyAccess::Success)
		{
			return false;
		}
		if (mDisplayName.IsEmpty())
		{
			FString LeftString;
			DataTablePath.Split(TEXT("."), &LeftString, &mDisplayName);
		}
		//if (const UDataTable* DataTable = LoadObject<UDataTable>(nullptr, (*DataTablePath), nullptr, LOAD_None, nullptr))
		if (const UDataTable* DataTable = LoadObject<UDataTable>(nullptr, (*DataTablePath)))
		{
			const FCSKit_BrainQueryTableRow* BrainQueryTableRow = DataTable->FindRow<FCSKit_BrainQueryTableRow>(TargetRowName,TEXT(""));
			if(BrainQueryTableRow == nullptr)
			{
				continue;
			}
			for (const FCSKit_BrainQueryTest& BrainQueryTest : BrainQueryTableRow->mTest)
			{
				OutList.Add(BrainQueryTest.mTestName.ToString());
			}
		}
	}

	return OutList.Num() > 0;
}

//プロパティのRowName指定変更時
void FCSKitEditor_BrainQueryTestSelectorCustomization::OnChangeRowName()
{
	TArray<FString> RowNameList;
	AppendRowNameListByPath(RowNameList);
	RowNameList.Sort([](const FString& A, const FString& B) {
		return A < B;
		});

	FString SelectedString;
	const TSharedPtr<FString> SelectedType = mComboBox->GetSelectedItem();
	if (SelectedType.IsValid())
	{
		SelectedString = *SelectedType;
	}

	int32 SelectIndex = 0;
	mStringList.Empty();
	mStringList.Add(MakeShareable(new FString(TEXT("None"))));
	for (const FString& RowName : RowNameList)
	{
		if (SelectedString == RowName)
		{
			SelectIndex = mStringList.Num();
		}
		mStringList.Add(MakeShareable(new FString(RowName)));
	}

	mComboBox->SetSelectedItem(mStringList[SelectIndex]);
}

FName FCSKitEditor_BrainQueryTestSelectorCustomization::GetRowName() const
{
	FName RowName;
	if (mRowNameHandle->GetValue(RowName) != FPropertyAccess::Success)
	{
		return RowName;
	}
	return RowName;
}

#undef LOCTEXT_NAMESPACE
