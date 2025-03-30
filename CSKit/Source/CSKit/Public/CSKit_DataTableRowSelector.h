// Copyright 2022 megasuraman
/**
 * @file CSKit_DataTableRowSelector.h
 * @brief プロパティでDataTableのRowNameをコンボボックス選択できる構造体
 *			FCSKitEditor_DataTableRowSelectorCustomizationで対応
 * @author megasuraman
 * @date 2020/05/27
 */

#pragma once

#include "CoreMinimal.h"
#include "CSKit_DataTableRowSelector.generated.h"

USTRUCT(BlueprintType)
struct CSKIT_API FCSKit_DataTableRowSelector
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "CSKit_DataTableRowSelector")
	FName mRowName;

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, Category = "CSKit_DataTableRowSelector")
	FString mDataTablePath;
	UPROPERTY(EditAnywhere, Category = "CSKit_DataTableRowSelector")
	UStruct* mDataTableStruct = nullptr;
	UPROPERTY(EditAnywhere, Category = "CSKit_DataTableRowSelector")
	FString mDisplayName;
#endif

	FCSKit_DataTableRowSelector() {}
	FCSKit_DataTableRowSelector(const FString& InDataTablePath)
	{
#if WITH_EDITORONLY_DATA
		mDataTablePath = InDataTablePath;
#endif
	}

#if WITH_EDITOR
	//有効なRowNameがセットされてるかどうか
	bool EditorIsValidRowName() const;
	bool EditorIsValidRowName_Path() const;
	bool EditorIsValidRowName_Struct() const;
#endif
};
