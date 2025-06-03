// Copyright 2020 megasuraman.
/**
 * @file CSKitEditor_ComboBoxCustomization.cpp
 * @brief FCSKit_DataTableRowSelectorを使ってDataTableのRowNameをComboBoxから選択できるように
 * @author megasuraman
 * @date 2025/05/05
 */
#include "PropertyTypeCustomization/CSKitEditor_DataTableRowSelectorCustomization.h"

#include "Widgets/Input/SNumericEntryBox.h"
#include "CSKit_DataTableRowSelector.h"
#include "AssetRegistry/AssetRegistryModule.h"

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

	if(IsMultiSelected())
	{
		mStringList.Add(MakeShareable(new FString(TEXT("--Multi--"))));
	}
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
	FCSKitEditor_ComboBoxCustomization::SetupSlate(StructBuilder);
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

#if 0//ロード済みのDataTableしか認識できない
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
#else
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

	// 検索用のフィルタを作成
	FARFilter Filter;
	Filter.ClassNames.Add(UDataTable::StaticClass()->GetFName());
	Filter.bRecursivePaths = true;
	Filter.PackagePaths.Add("/Game"); // ここを "/Game" にすることで、プロジェクト内のアセットに限定

	// 結果格納用
	TArray<FAssetData> OutAssetData;
	AssetRegistry.GetAssets(Filter, OutAssetData);

	// 検出されたDataTableをログ出力
	for (const FAssetData& AssetData : OutAssetData)
	{
		//UE_LOG(LogTemp, Log, TEXT("Found DataTable: %s"), *AssetData.AssetName.ToString());

		// 必要に応じてロードする
		UDataTable* DataTableAsset = Cast<UDataTable>(AssetData.GetAsset());
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
#endif
	return OutList.Num() > 0;
}

#undef LOCTEXT_NAMESPACE