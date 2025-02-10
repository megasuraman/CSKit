// Copyright 2022 megasuraman

#pragma once

#include "CoreMinimal.h"
#include "DebugMenu/CSKitDebug_DebugMenuTableRow.h"
#include "DebugMenu/CSKitDebug_DebugMenuNodeButton.h"

class CSKITDEBUG_API CSKitDebug_DebugMenuNodeFolder : public CSKitDebug_DebugMenuNodeButton
{
public:
	virtual void OnEndAction(const FCSKitDebug_DebugMenuNodeActionParameter& InParameter) override;
	virtual void OnJustPressedRightKey() override;

protected:
	virtual void DrawValue(UCanvas* InCanvas, const FVector2D& InPos, const FLinearColor InColor) const;

private:
};
