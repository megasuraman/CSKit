// Copyright 2022 megasuraman

#pragma once

#include "CoreMinimal.h"
#include "IPropertyTypeCustomization.h"
#include "SSearchableComboBox.h"
#include "PropertyHandle.h"


class CSKITEDITOR_API FStringComboBoxCustomizationBase : public IPropertyTypeCustomization
{
public:
	FStringComboBoxCustomizationBase();

	// static TSharedRef<IPropertyTypeCustomization> MakeInstance()
	// {
	// 	return MakeShareable(new FStringComboBoxCustomizationBase);
	// }

	virtual void CustomizeHeader(TSharedRef<IPropertyHandle> StructPropertyHandle,
		class FDetailWidgetRow& HeaderRow,
		IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;
	virtual void CustomizeChildren(TSharedRef<class IPropertyHandle> StructPropertyHandle,
		class IDetailChildrenBuilder& StructBuilder,
		IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;


protected:
	//TSharedPtr<IPropertyHandle>
	virtual void SetupPropertyHandle(const TSharedRef<IPropertyHandle>& StructPropertyHandle){};
	//表示名設定
	virtual void SetupDisplayName(const TSharedRef<IPropertyHandle>& StructPropertyHandle){};
	//リスト設定
	virtual void SetupStringList(const TSharedRef<IPropertyHandle>& StructPropertyHandle){}
	//Slate
	virtual void SetupSlate(class IDetailChildrenBuilder& StructBuilder);
	//選択変更時
	void OnSelectionChanged(TSharedPtr<FString> Type, ESelectInfo::Type SelectionType) const;
	//Widget生成時
	TSharedRef<SWidget> OnGenerateWidget(TSharedPtr<FString> Type);
	//選択中の文字列取得
	FText GetSelectedTypeText() const;
	//選択中のIndex取得
	int32 GetSelectIndex() const;
	//有効な入力値かチェック
	void CheckActiveResultInput();

protected:
	TArray< TSharedPtr<FString> > mStringList;
	TSharedPtr<class SSearchableComboBox> mComboBox;
	TSharedPtr< IPropertyHandle > mResultInputHandle;
	FString mDisplayName;
};
