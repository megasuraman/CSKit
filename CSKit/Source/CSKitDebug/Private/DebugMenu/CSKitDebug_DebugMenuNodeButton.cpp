// Copyright 2022 megasuraman

#include "DebugMenu/CSKitDebug_DebugMenuNodeButton.h"


void CSKitDebug_DebugMenuNodeButton::OnEndAction(const FCSKitDebug_DebugMenuNodeActionParameter& InParameter)
{
	CSKitDebug_DebugMenuNodeBase::OnEndAction(InParameter);
}

void CSKitDebug_DebugMenuNodeButton::DrawValue(UCanvas* InCanvas, const FVector2D& InPos, const FLinearColor InColor) const
{
	FCanvasTextItem Item(InPos, FText::FromString(FString(TEXT("実行"))), GEngine->GetSmallFont(), InColor);
	Item.Scale = FVector2D(1.f);
	InCanvas->DrawItem(Item);
}
