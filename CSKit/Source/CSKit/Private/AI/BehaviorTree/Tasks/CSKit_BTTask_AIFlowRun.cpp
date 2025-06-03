// Copyright 2020 megasuraman.
/**
 * @file CSKit_BTTask_AIFlowRun.cpp
 * @brief AIFlowNode実行
 * @author megasuraman
 * @date 2025/06/03
 */
#include "AI/BehaviorTree/Tasks/CSKit_BTTask_AIFlowRun.h"

#include "CSKit_Config.h"

UCSKit_BTTask_AIFlowRun::UCSKit_BTTask_AIFlowRun(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	NodeName = TEXT("CSKit_AIFlowRun");

	const UCSKit_Config* CSKitConfig = GetDefault<UCSKit_Config>();
	InjectionTag = CSKitConfig->mAIFlowNodeTag;
}

/* ------------------------------------------------------------
   !静的なパラメータ表示？
------------------------------------------------------------ */
FString UCSKit_BTTask_AIFlowRun::GetStaticDescription() const
{
	const FString KeyDesc("invalid");

	return FString::Printf(TEXT("%s: %s"), *Super::GetStaticDescription(), *KeyDesc);
}