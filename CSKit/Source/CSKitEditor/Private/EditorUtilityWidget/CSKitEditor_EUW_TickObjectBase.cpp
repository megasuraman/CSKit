// Copyright 2020 megasuraman
/**
 * @file CSKitEditor_EUW_TickObjectBase.cpp
 * @brief EditorUtilityWidgetでWidgetとは独立したTickを提供するObject(EUWではTickできないので)
 * @author megasuraman
 * @date 2025/09/29
 */

#include "EditorUtilityWidget/CSKitEditor_EUW_TickObjectBase.h"

/**
 * @brief	
 */
void UCSKitEditor_EUW_TickObjectBase::SetActive(const bool bInActive)
{
	if (bInActive)
	{
		if (!mDebugTickHandle.IsValid())
		{
			mDebugTickHandle = FTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateUObject(this, &UCSKitEditor_EUW_TickObjectBase::DebugTick));
		}
	}
	else
	{
		FTicker::GetCoreTicker().RemoveTicker(mDebugTickHandle);
	}
}

/**
 * @brief	
 */
void UCSKitEditor_EUW_TickObjectBase::SetTickDelegate(const FCSKitEditor_EUW_TickEvent& InDelegate)
{
	mTickEvent = InDelegate;
}

/**
 * @brief	
 */
bool UCSKitEditor_EUW_TickObjectBase::DebugTick(float InDeltaSecond) const
{
	return mTickEvent.ExecuteIfBound(InDeltaSecond);
}
