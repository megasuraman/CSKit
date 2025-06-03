// Copyright 2020 megasuraman.
/**
 * @file CSKit_BTTask_AIFlowRun.h
 * @brief AIFlowNode実行
 * @author megasuraman
 * @date 2025/06/03
 */
#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_RunBehaviorDynamic.h"
#include "CSKit_BTTask_AIFlowRun.generated.h"


UCLASS(DisplayName = "CSKit AIFlow Run", HideCategories=("Node"))
class CSKIT_API UCSKit_BTTask_AIFlowRun : public UBTTask_RunBehaviorDynamic
{
	GENERATED_BODY()
	
public:
	UCSKit_BTTask_AIFlowRun(const FObjectInitializer& ObjectInitializer);
	virtual FString GetStaticDescription() const override;

protected:

};
