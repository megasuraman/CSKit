// Copyright 2022 megasuraman

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "CSKitDebug_DebugMenuTableRow.generated.h"

UENUM(BlueprintType)
enum class ECSKitDebug_DebugMenuValueKind : uint8
{
	Invalid,
	Bool,
	Int,
	Float,
	List,
	Enum,
	Button,
	Folder UMETA(Hidden),
};

USTRUCT(BlueprintType)
struct CSKITDEBUG_API FCSKitDebug_DebugMenuNodeData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "表示名", DisplayPriority = 1))
	FString mDisplayName;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "コメント", DisplayPriority = 1, MultiLine = true))
	FString mComment;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "Enum型名", DisplayPriority = 4, EditCondition = "mKind == ECSKitDebug_DebugMenuValueKind::Enum"))
	FString mEnumName;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "初期値", DisplayPriority = 3))
	FString mInitValue;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "文字列リスト", DisplayPriority = 4, EditCondition = "mKind == ECSKitDebug_DebugMenuValueKind::List"))
	TArray<FString> mList;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "型", DisplayPriority = 2))
	ECSKitDebug_DebugMenuValueKind mKind = ECSKitDebug_DebugMenuValueKind::Invalid;
};

USTRUCT(BlueprintType)
struct CSKITDEBUG_API FCSKitDebug_DebugMenuTableRow : public FTableRowBase
{
	GENERATED_BODY()

// 	UPROPERTY(VisibleAnywhere, meta = (DisplayName = "フォルダパス", DisplayPriority = 1))
// 	FString mDisplayName;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "項目リスト", DisplayPriority = 1, TitleProperty = "mDisplayName"))
	TArray<FCSKitDebug_DebugMenuNodeData> mNodeList;
};
