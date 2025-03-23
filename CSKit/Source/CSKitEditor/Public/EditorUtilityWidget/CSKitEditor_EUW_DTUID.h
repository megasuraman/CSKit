// Copyright 2020 megasuraman
/**
 * @file CSKitEditor_EUW_DTUID.h
 * @brief DataTableのRowNameをUniqueID化する
 * @author megasuraman
 * @date 2022/12/27
 */

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget/CSKitEditor_EUW_Base.h"
#include "CSKitEditor_EUW_DTUID.generated.h"

class UDataTable;
/**
 * 
 */
UCLASS()
class CSKITEDITOR_API UCSKitEditor_EUW_DTUID : public UCSKitEditor_EUW_Base
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void	GenerateDataTableUID(const UDataTable* InDataTable);

protected:
	struct FDTUIDInfo
	{
		FString	mName;
		uint32	mId = 0;
		bool	mbDelete = false;
	};
	bool	GetExistDTUID(TArray<FDTUIDInfo>& OutDTUIDList, const FString& InPath) const;
	void	CheckDataTableRow(TArray<FDTUIDInfo>& OutDTUIDList, const UDataTable* InDataTable) const;
	bool	CreateStringReplacepMap(TMap<FString, FString>& OutStringReplaceMap, const TArray<FDTUIDInfo>& InDTUIDList, const FString& InDataTableName) const;
	bool	GetNewFileString(FString& OutFileString, const FString& InBaseFilePath, const TMap<FString, FString>& InStringReplaceMap);
};
