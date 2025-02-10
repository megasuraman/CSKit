// Copyright 2022 megasuraman

#pragma once

#include "CoreMinimal.h"
#include "DebugMenu/CSKitDebug_DebugMenuTableRow.h"
#include "DebugMenu/CSKitDebug_DebugMenuNodeBase.h"

class CSKITDEBUG_API CSKitDebug_DebugMenuNodeFloat : public CSKitDebug_DebugMenuNodeBase
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
	virtual void DrawEditValue(UCanvas* InCanvas, const FVector2D& InPos, const FVector2D& InValueExtent) const;

private:
	static const int32 mEditIntegralDigitNum = 7;//®”Œ…”
	static const int32 mEditDecimalNum = 3;//¬”Œ…”
	static const int32 mEditDigitNumberNum = mEditIntegralDigitNum + mEditDecimalNum;//Œ…”
	static const int32 mEditDigitNum = mEditDigitNumberNum + 1;//•„†•ª’Ç‰Á
	TArray<int32> mEditDigitIntList;
	int32 mEditDigitIndex = 0;//mEditDigitIntList‚Ì•ÒWˆÊ’u
};
