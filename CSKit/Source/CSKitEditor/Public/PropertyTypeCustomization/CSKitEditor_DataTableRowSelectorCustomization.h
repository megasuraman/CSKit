// Copyright 2020 megasuraman.
/**
 * @file CSKitEditor_ComboBoxCustomization.h
 * @brief FCSKit_DataTableRowSelectorを使ってDataTableのRowNameをComboBoxから選択できるように
 * @author megasuraman
 * @date 2025/05/05
 */
#pragma once

#include "CoreMinimal.h"
#include "CSKitEditor_ComboBoxCustomization.h"
#include "PropertyHandle.h"

class CSKITEDITOR_API FCSKitEditor_DataTableRowSelectorCustomization : public FCSKitEditor_ComboBoxCustomization
{
public:
	FCSKitEditor_DataTableRowSelectorCustomization();

	static TSharedRef<IPropertyTypeCustomization> MakeInstance()
	{
		return MakeShareable(new FCSKitEditor_DataTableRowSelectorCustomization);
	}

protected:
	virtual void SetupPropertyHandle(const TSharedRef<IPropertyHandle>& StructPropertyHandle) override;
	virtual void SetupDisplayName(const TSharedRef<IPropertyHandle>& StructPropertyHandle) override;
	virtual void SetupStringList(const TSharedRef<IPropertyHandle>& StructPropertyHandle) override;
	virtual void SetupSlate(class IDetailChildrenBuilder& StructBuilder) override;

	//DataTableからRowName取得
	bool AppendRowNameListByPath(TArray<FString>& OutList, const TSharedRef<IPropertyHandle>& StructPropertyHandle);
	//UStructを使用してるDataTableからRowName取得
	bool AppendRowNameListByStruct(TArray<FString>& OutList, const TSharedRef<IPropertyHandle>& StructPropertyHandle);
};