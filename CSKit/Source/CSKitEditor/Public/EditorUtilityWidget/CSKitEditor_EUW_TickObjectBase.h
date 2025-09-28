// Copyright 2020 megasuraman
/**
 * @file CSKitEditor_EUW_TickObjectBase.h
 * @brief EditorUtilityWidgetでWidgetとは独立したTickを提供するObject(EUWではTickできないので)
 * @author megasuraman
 * @date 2025/09/29
 */

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "CSKitEditor_EUW_TickObjectBase.generated.h"

DECLARE_DELEGATE_OneParam(FCSKitEditor_EUW_TickEvent, const float);

UCLASS(BlueprintType)
class CSKITEDITOR_API UCSKitEditor_EUW_TickObjectBase : public UObject
{
	GENERATED_BODY()

public:
	void SetActive(const bool bInActive);
    void SetTickDelegate(const FCSKitEditor_EUW_TickEvent& InDelegate);

protected:
	bool DebugTick(float InDeltaSecond) const;
	
protected:
	FDelegateHandle	mDebugTickHandle;
	FCSKitEditor_EUW_TickEvent mTickEvent;
};
