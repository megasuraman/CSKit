// Copyright 2022 megasuraman

#pragma once

#include "CoreMinimal.h"
#include "CSKitEditor_ComboBoxCustomization.h"
#include "PropertyHandle.h"


class CSKITEDITOR_API FCSKitEditor_DataTableRowSelectorCustomization : public FStringComboBoxCustomizationBase
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