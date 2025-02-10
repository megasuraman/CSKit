// Copyright 2022 megasuraman

#include "DebugMenu/CSKitDebug_DebugMenuNodeFolder.h"
#include "DebugMenu/CSKitDebug_DebugMenuManager.h"


void CSKitDebug_DebugMenuNodeFolder::OnEndAction(const FCSKitDebug_DebugMenuNodeActionParameter& InParameter)
{
	CSKitDebug_DebugMenuNodeBase::OnEndAction(InParameter);
	if (UCSKitDebug_DebugMenuManager* Manager = GetManager())
	{
		Manager->SetMainFolder(GetPath());
	}
}

void CSKitDebug_DebugMenuNodeFolder::OnJustPressedRightKey()
{
	if (UCSKitDebug_DebugMenuManager* Manager = GetManager())
	{
		Manager->SetMainFolder(GetPath());
	}
}

void CSKitDebug_DebugMenuNodeFolder::DrawValue(UCanvas* InCanvas, const FVector2D& InPos, const FLinearColor InColor) const
{
	FCanvasTextItem Item(InPos, FText::FromString(FString(TEXT("フォルダ移動"))), GEngine->GetSmallFont(), InColor);
	Item.Scale = FVector2D(1.f);
	InCanvas->DrawItem(Item);
}
