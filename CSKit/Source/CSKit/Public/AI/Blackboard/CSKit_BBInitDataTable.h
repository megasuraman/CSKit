// Copyright 2020 megasuraman.
/**
 * @file CSKit_BBInitDataTable.h
 * @brief Blackboardへの初期値設定用DataTable
 * @author megasuraman
 * @date 2025/05/18
 */
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "CSKit_BBInitDataTable.generated.h"

class UBlackboardComponent;

USTRUCT(BlueprintType)
struct FCSKit_BBInitDataNodeBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, Category = Blackboard, DisplayName = "キー名")
	FName	mKeyName;
#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, Category = Blackboard, DisplayName = "コメント")
	FString	mComment;
#endif

	FCSKit_BBInitDataNodeBase() {}
};
USTRUCT(BlueprintType)
struct FCSKit_BBInitDataNodeInt : public FCSKit_BBInitDataNodeBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, Category = Blackboard, DisplayName = "初期値")
	int32	mValue = 0;

	FCSKit_BBInitDataNodeInt() {}
};
USTRUCT(BlueprintType)
struct FCSKit_BBInitDataNodeFloat : public FCSKit_BBInitDataNodeBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, Category = Blackboard, DisplayName = "初期値")
	float	mValue = 0.f;

	FCSKit_BBInitDataNodeFloat() {}
};
USTRUCT(BlueprintType)
struct FCSKit_BBInitDataNodeName : public FCSKit_BBInitDataNodeBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, Category = Blackboard, DisplayName = "初期値")
	FName	mValue;

	FCSKit_BBInitDataNodeName() {}
};
USTRUCT(BlueprintType)
struct FCSKit_BBInitDataNodeBool : public FCSKit_BBInitDataNodeBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, Category = Blackboard, DisplayName = "初期値")
	bool	mbValue = false;

	FCSKit_BBInitDataNodeBool() {}
};

USTRUCT(BlueprintType)
struct FCSKit_BBInitValueTableRow : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, Category = Blackboard, meta = (DisplayName = "Int", TitleProperty = "mComment"))
	TArray<FCSKit_BBInitDataNodeInt>	mIntValueList;
	UPROPERTY(EditDefaultsOnly, Category = Blackboard, meta = (DisplayName = "Float", TitleProperty = "mComment"))
	TArray<FCSKit_BBInitDataNodeFloat>	mFloatValueList;
	UPROPERTY(EditDefaultsOnly, Category = Blackboard, meta = (DisplayName = "Name", TitleProperty = "mComment"))
	TArray<FCSKit_BBInitDataNodeName>	mNameValueList;
	UPROPERTY(EditDefaultsOnly, Category = Blackboard, meta = (DisplayName = "Bool", TitleProperty = "mComment"))
	TArray<FCSKit_BBInitDataNodeBool>	mBoolValueList;

	virtual void ApplyBlackboardValue(UBlackboardComponent& InBlackboard) const;
#if USE_CSKIT_DEBUG
	static void DebugCheckInvalidKeyName(const UBlackboardComponent& InBlackboard, const FName& InKeyName);
#endif
};