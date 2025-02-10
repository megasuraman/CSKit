// Copyright 2022 megasuraman

#pragma once

#include "CoreMinimal.h"
#include "DebugMenu/CSKitDebug_DebugMenuTableRow.h"
#include "DebugMenu/CSKitDebug_DebugMenuNodeBase.h"

class CSKITDEBUG_API CSKitDebug_DebugMenuNodeButton : public CSKitDebug_DebugMenuNodeBase
{
public:
	virtual void OnEndAction(const FCSKitDebug_DebugMenuNodeActionParameter& InParameter) override;

protected:
	virtual void DrawValue(UCanvas* InCanvas, const FVector2D& InPos, const FLinearColor InColor) const;

};
