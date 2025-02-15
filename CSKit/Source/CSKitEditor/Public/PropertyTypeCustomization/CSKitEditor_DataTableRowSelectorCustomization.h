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
	//TSharedPtr<IPropertyHandle>の保持
	virtual void SetupPropertyHandle(const TSharedRef<IPropertyHandle>& StructPropertyHandle) override;
	//表示名(mDisplayName)設定
	virtual void SetupDisplayName(const TSharedRef<IPropertyHandle>& StructPropertyHandle) override;
	//コンボボックスに表示する文字列(mStringList)設定
	virtual void SetupStringList(const TSharedRef<IPropertyHandle>& StructPropertyHandle) override;
	//Slate設定
	virtual void SetupSlate(class IDetailChildrenBuilder& StructBuilder) override;

	//DataTableのフルパスから文字列取得
	bool AppendRowNameListByPath(TArray<FString>& OutList, const TSharedRef<IPropertyHandle>& StructPropertyHandle);
	//UStructから文字列取得
	bool AppendRowNameListByStruct(TArray<FString>& OutList, const TSharedRef<IPropertyHandle>& StructPropertyHandle);
};