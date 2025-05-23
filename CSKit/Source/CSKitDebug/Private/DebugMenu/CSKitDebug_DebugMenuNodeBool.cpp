// Copyright 2022 megasuraman

#include "DebugMenu/CSKitDebug_DebugMenuNodeBool.h"


void CSKitDebug_DebugMenuNodeBool::OnEndAction(const FCSKitDebug_DebugMenuNodeActionParameter& InParameter)
{
	SetValueBool(!GetBool());
	CSKitDebug_DebugMenuNodeBase::OnEndAction(InParameter);
}

void CSKitDebug_DebugMenuNodeBool::SetInitValue()
{
	if (!GetNodeData().mInitValue.IsEmpty())
	{
		SetValueString(GetNodeData().mInitValue);
		SetValueBool(GetBool());
	}
}
