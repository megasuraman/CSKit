// Copyright 2022 megasuraman

#pragma once

#include "CoreMinimal.h"
#include "DebugMenu/CSKitDebug_DebugMenuTableRow.h"
#include "DebugMenu/CSKitDebug_DebugMenuNodeBase.h"

class CSKITDEBUG_API CSKitDebug_DebugMenuNodeList : public CSKitDebug_DebugMenuNodeBase
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
	static const int32 mEditDrawListNum = 9;//表示するリスト項目数
	static const int32 mEditDrawListInitCursolOffset = mEditDrawListNum/2;//選択カーソルの初期オフセット
	int32 mEditSelectIndex = 0;
	int32 mEditDrawIndexMin = 0;
	int32 mEditDrawIndexMax = 0;
};
