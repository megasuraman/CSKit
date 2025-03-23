// Copyright 2022 megasuraman

#pragma once

#include "CoreMinimal.h"
#include "DebugMenu/CSKitDebug_DebugMenuNodeBase.h"

class CSKITDEBUG_API CSKitDebug_DebugMenuNodeInt : public CSKitDebug_DebugMenuNodeBase
{
public:
	virtual void OnBeginAction() override;
	virtual void OnEndAction(const FCSKitDebug_DebugMenuNodeActionParameter& InParameter) override;
	virtual void OnJustPressedUpKey() override;
	virtual void OnJustPressedDownKey() override;
	virtual void OnJustPressedLeftKey() override;
	virtual void OnJustPressedRightKey() override;

protected:
	virtual void SetInitValue() override;
	virtual void DrawEditValue(UCanvas* InCanvas, const FVector2D& InPos, const FVector2D& InValueExtent) const override;

private:
	static constexpr int32 mEditDigitNum = 8;
	static constexpr int32 mEditDigitIntListNum = mEditDigitNum + 1;
	TArray<int32> mEditDigitIntList;
	int32 mEditDigitIntIndex = 0;
};
