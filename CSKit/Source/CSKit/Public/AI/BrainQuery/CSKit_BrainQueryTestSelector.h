// Copyright 2020 megasuraman.
/**
 * @file CSKit_BrainQueryTestSelector.h
 * @brief プロパティからDataTableのRowNameをComboBoxから選択可能な構造体
 * @author megasuraman
 * @date 2025/05/11
 */
#pragma once

#include "CoreMinimal.h"
#include "CSKit_BrainQueryTestSelector.generated.h"

class UDataTable;

USTRUCT(BlueprintType)
struct FCSKit_BrainQueryTestSelector
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "CSKit_BrainQueryTestSelector")
	FName mTestName;

#if WITH_EDITORONLY_DATA
	//RowNameの元になるDataTableをフルパスで指定
	UPROPERTY(EditAnywhere, Category = "CSKit_BrainQueryTestSelector")
	TArray<FString> mEditorDataTablePathList;
	//表示名
	UPROPERTY(EditAnywhere, Category = "CSKit_BrainQueryTestSelector")
	FString mEditorDisplayName;
	//FCSKit_BrainQueryTableRowを得るためのRowName
	UPROPERTY(EditAnywhere, Category = "CSKit_BrainQueryTestSelector", meta = (DisplayName = "RawName"))
	FName mEditorDataTableRowName;
#endif

	FCSKit_BrainQueryTestSelector() {}
	FCSKit_BrainQueryTestSelector(const FString& InDataTablePath)
	{
#if WITH_EDITOR
		mEditorDataTablePathList.Add(InDataTablePath);
#endif
	}
};
