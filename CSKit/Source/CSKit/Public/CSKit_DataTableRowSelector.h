// Copyright 2022 megasuraman

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
	UStruct* mDataTableStruct;
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
};
