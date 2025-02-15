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
	//TSharedPtr<IPropertyHandle>�̕ێ�(mResultInputHandle�̐ݒ�K�{)
	virtual void SetupPropertyHandle(const TSharedRef<IPropertyHandle>& StructPropertyHandle){};
	//�\����(mDisplayName)�ݒ�
	virtual void SetupDisplayName(const TSharedRef<IPropertyHandle>& StructPropertyHandle){};
	//�R���{�{�b�N�X�ɕ\�����镶����(mStringList)�ݒ�
	virtual void SetupStringList(const TSharedRef<IPropertyHandle>& StructPropertyHandle){}
	//Slate�ݒ�
	virtual void SetupSlate(class IDetailChildrenBuilder& StructBuilder);
	//�R���{�{�b�N�X�̑I��ύX������
	void OnSelectionChanged(TSharedPtr<FString> Type, ESelectInfo::Type SelectionType);
	//Widget����������
	TSharedRef<SWidget> OnGenerateWidget(TSharedPtr<FString> Type);
	//�R���{�{�b�N�X�őI�𒆂̕�����擾
	FText GetSelectedTypeText() const;
	//�R���{�{�b�N�X�őI�𒆂�Index�擾
	int32 GetSelectIndex() const;
	//�o�͐�v���p�e�B�ɓK�؂ȕ�����Z�b�g����Ă邩�`�F�b�N
	void CheckActiveResultInput();

protected:
	TArray< TSharedPtr<FString> > mStringList;//�R���{�{�b�N�X�ɕ\�����镶���񃊃X�g
	TSharedPtr<class SSearchableComboBox> mComboBox;
	TSharedPtr< IPropertyHandle > mResultInputHandle;//�R���{�{�b�N�X�̑I������������̊i�[��
	FString mDisplayName;//�v���p�e�B�̕\����
};
