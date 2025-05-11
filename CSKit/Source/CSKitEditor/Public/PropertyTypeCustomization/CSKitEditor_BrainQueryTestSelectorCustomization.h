// Copyright 2020 megasuraman.
/**
 * @file CSKitEditor_BrainQueryTestSelectorCustomization.h
 * @brief CSKit_BrainQueryTestSelectorのプロパティをDataTableのRowNameのComboBox化する
 * @author megasuraman
 * @date 2025/05/05
 */
#pragma once

#include "CoreMinimal.h"
#include "PropertyHandle.h"
#include "CSKitEditor_ComboBoxCustomization.h"

class FCSKitEditor_BrainQueryTestSelectorCustomization : public FCSKitEditor_ComboBoxCustomization
{
public:
	FCSKitEditor_BrainQueryTestSelectorCustomization();

	static TSharedRef<IPropertyTypeCustomization> MakeInstance()
	{
		return MakeShareable(new FCSKitEditor_BrainQueryTestSelectorCustomization);
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
	bool AppendRowNameListByPath(TArray<FString>& OutList);
	//プロパティのRowName指定変更時
	void OnChangeRowName();

	FName GetRowName() const;

private:
	TSharedPtr< IPropertyHandle > mDataTablePathHandle;
	TSharedPtr< IPropertyHandle > mRowNameHandle;
};
