// Copyright 2020 megasuraman
/**
 * @file CSKit_SubLevelPresetTableRow.h
 * @brief チェック対象のレベルを定義するDataTable
 * @author megasuraman
 * @date 2025/09/28
 */
#pragma once

#include "CoreMinimal.h"
#include "CSKit_SubLevelPresetTableRow.generated.h"

USTRUCT(BlueprintType)
struct CSKIT_API FCSKit_SubLevelPresetTableRow : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = SubLevelPreset, Meta = (DisplayName = "LevelList", DisplayPriority = 1))
	TArray<TSoftObjectPtr<class UWorld>> mNeedLevelList;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = SubLevelPreset, Meta = (DisplayName = "OptionLevelList", DisplayPriority = 1))
	TArray<TSoftObjectPtr<class UWorld>> mOptionLevelList;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = SubLevelPreset, Meta = (DisplayName = "EditorSupportLevelList", DisplayPriority = 1))
	TArray<TSoftObjectPtr<class UWorld>> mEditorSupportLevelList;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = SubLevelPreset, Meta = (DisplayName = "AutoRun対象", DisplayPriority = 1))
	bool mbEditorEUWAutoRunTarget = false;;
};