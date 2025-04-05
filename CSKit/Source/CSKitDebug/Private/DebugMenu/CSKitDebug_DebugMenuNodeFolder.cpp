// Copyright 2022 megasuraman

#include "DebugMenu/CSKitDebug_DebugMenuNodeFolder.h"

#include "CanvasItem.h"
#include "DebugMenu/CSKitDebug_DebugMenuManager.h"
#include "Engine/Canvas.h"


void CSKitDebug_DebugMenuNodeFolder::OnEndAction(const FCSKitDebug_DebugMenuNodeActionParameter& InParameter)
{
	if (UCSKitDebug_DebugMenuManager* Manager = GetManager())
	{
		Manager->SetMainFolder(GetPath());
	}
	CSKitDebug_DebugMenuNodeBase::OnEndAction(InParameter);
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
