// Copyright 2020 megasuraman.
/**
 * @file CSKit_AIFlowDataTable.h
 * @brief AIFlow用DataTable
 * @author megasuraman
 * @date 2025/05/05
 */
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "BehaviorTree/BehaviorTree.h"
#include "CSKit_AIFlowDataTable.generated.h"

// DataTable
USTRUCT(BlueprintType)
struct FCSKit_AIFlowActionTableRow : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY(EditDefaultsOnly, Category = "AIFlow", meta = (DisplayName = "BehaviorTree"))
	TSoftObjectPtr<class UBehaviorTree> mBehaviorTree = nullptr;
};