// Copyright 2022 megasuraman

#pragma once

#include "CoreMinimal.h"
#include "IPropertyTypeCustomization.h"
#include "PropertyHandle.h"

class FCSKitEditor_DataTableRowSelectorCustomization : public IPropertyTypeCustomization
{
public:
	FCSKitEditor_DataTableRowSelectorCustomization();

	static TSharedRef<IPropertyTypeCustomization> MakeInstance() {
		return MakeShareable(new FCSKitEditor_DataTableRowSelectorCustomization);
	}

	virtual void CustomizeHeader(TSharedRef<IPropertyHandle> StructPropertyHandle,
		class FDetailWidgetRow& HeaderRow,
		IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;
	virtual void CustomizeChildren(TSharedRef<class IPropertyHandle> StructPropertyHandle,
		class IDetailChildrenBuilder& StructBuilder,
		IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;


protected:
	using TestSelectTypePtr = TSharedPtr<FString>;
	void OnSelectionChanged(TestSelectTypePtr Type, ESelectInfo::Type SelectionType);
	TSharedRef<SWidget> OnGenerateWidget(TestSelectTypePtr Type);
	FText GetSelectedTypeText() const;
	int32 GetSelectIndex() const;
	void SetupDisplayName(TSharedRef<IPropertyHandle> StructPropertyHandle);
	void SetupRowNameList(TSharedRef<IPropertyHandle> StructPropertyHandle);
	bool SetupRowNameListByStruct(TSharedRef<IPropertyHandle> StructPropertyHandle);
	bool SetupRowNameListByPath(TSharedRef<IPropertyHandle> StructPropertyHandle);

private:
	TSharedPtr<IPropertyHandle> mRowNameHandle;

	TArray< TestSelectTypePtr > mRowNameList;
	TSharedPtr< SComboBox< TestSelectTypePtr > > mComboBox;
	FString mPropertyTitleName;
};