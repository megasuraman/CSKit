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
	//TSharedPtr<IPropertyHandle>�̕ێ�
	virtual void SetupPropertyHandle(const TSharedRef<IPropertyHandle>& StructPropertyHandle) override;
	//�\����(mDisplayName)�ݒ�
	virtual void SetupDisplayName(const TSharedRef<IPropertyHandle>& StructPropertyHandle) override;
	//�R���{�{�b�N�X�ɕ\�����镶����(mStringList)�ݒ�
	virtual void SetupStringList(const TSharedRef<IPropertyHandle>& StructPropertyHandle) override;
	//Slate�ݒ�
	virtual void SetupSlate(class IDetailChildrenBuilder& StructBuilder) override;

	//DataTable�̃t���p�X���當����擾
	bool AppendRowNameListByPath(TArray<FString>& OutList, const TSharedRef<IPropertyHandle>& StructPropertyHandle);
	//UStruct���當����擾
	bool AppendRowNameListByStruct(TArray<FString>& OutList, const TSharedRef<IPropertyHandle>& StructPropertyHandle);
};