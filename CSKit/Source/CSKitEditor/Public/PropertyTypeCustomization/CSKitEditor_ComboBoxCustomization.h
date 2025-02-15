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
	//TSharedPtr<IPropertyHandle>の保持(mResultInputHandleの設定必須)
	virtual void SetupPropertyHandle(const TSharedRef<IPropertyHandle>& StructPropertyHandle){};
	//表示名(mDisplayName)設定
	virtual void SetupDisplayName(const TSharedRef<IPropertyHandle>& StructPropertyHandle){};
	//コンボボックスに表示する文字列(mStringList)設定
	virtual void SetupStringList(const TSharedRef<IPropertyHandle>& StructPropertyHandle){}
	//Slate設定
	virtual void SetupSlate(class IDetailChildrenBuilder& StructBuilder);
	//コンボボックスの選択変更時処理
	void OnSelectionChanged(TSharedPtr<FString> Type, ESelectInfo::Type SelectionType);
	//Widget生成時処理
	TSharedRef<SWidget> OnGenerateWidget(TSharedPtr<FString> Type);
	//コンボボックスで選択中の文字列取得
	FText GetSelectedTypeText() const;
	//コンボボックスで選択中のIndex取得
	int32 GetSelectIndex() const;
	//出力先プロパティに適切な文字列セットされてるかチェック
	void CheckActiveResultInput();

protected:
	TArray< TSharedPtr<FString> > mStringList;//コンボボックスに表示する文字列リスト
	TSharedPtr<class SSearchableComboBox> mComboBox;
	TSharedPtr< IPropertyHandle > mResultInputHandle;//コンボボックスの選択した文字列の格納先
	FString mDisplayName;//プロパティの表示名
};
