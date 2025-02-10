// Copyright 2022 megasuraman

#include "DebugMenu/CSKitDebug_DebugMenuNodeInt.h"


void CSKitDebug_DebugMenuNodeInt::OnBeginAction()
{
	CSKitDebug_DebugMenuNodeBase::OnBeginAction();

	mEditDigitIntList.Empty();
	mEditDigitIntList.Reserve(mEditDigitIntListNum);
	const int32 IntValue = GetInt();
	int32 CalcDigitValue = FMath::Abs(IntValue);
	for (int32 i = 0; i < mEditDigitNum; ++i)
	{
		const int32 DigitValue = CalcDigitValue % 10;
		mEditDigitIntList.Add(DigitValue);
		CalcDigitValue /= 10;
	}
	//最後に符号情報
	if (IntValue >= 0)
	{
		mEditDigitIntList.Add(1);
	}
	else
	{
		mEditDigitIntList.Add(-1);
	}
}

void CSKitDebug_DebugMenuNodeInt::OnEndAction(const FCSKitDebug_DebugMenuNodeActionParameter& InParameter)
{
	CSKitDebug_DebugMenuNodeBase::OnEndAction(InParameter);

	int32 IntValue = 0;
	int32 DigitValue = 1;
	for (int32 i = 0; i < mEditDigitNum; ++i)
	{
		IntValue += mEditDigitIntList[i] * DigitValue;
		DigitValue *= 10;
	}
	if (mEditDigitIntList[mEditDigitIntListNum - 1] < 0)
	{
		IntValue *= -1;
	}

	SetValueInt(IntValue);
}

void CSKitDebug_DebugMenuNodeInt::OnJustPressedUpKey()
{
	if (!IsEditMode())
	{
		return;
	}
	if (mEditDigitIntIndex == mEditDigitIntListNum - 1)
	{
		mEditDigitIntList[mEditDigitIntIndex] *= -1;
	}
	else
	{
		if (mEditDigitIntList[mEditDigitIntIndex] < 9)
		{
			mEditDigitIntList[mEditDigitIntIndex] += 1;
		}
		else
		{
			mEditDigitIntList[mEditDigitIntIndex] = 0;
		}
	}
}

void CSKitDebug_DebugMenuNodeInt::OnJustPressedDownKey()
{
	if (!IsEditMode())
	{
		return;
	}
	if (mEditDigitIntIndex == mEditDigitIntListNum - 1)
	{
		mEditDigitIntList[mEditDigitIntIndex] *= -1;
	}
	else
	{
		if (mEditDigitIntList[mEditDigitIntIndex] > 0)
		{
			mEditDigitIntList[mEditDigitIntIndex] -= 1;
		}
		else
		{
			mEditDigitIntList[mEditDigitIntIndex] = 9;
		}
	}
}

void CSKitDebug_DebugMenuNodeInt::OnJustPressedLeftKey()
{
	if (!IsEditMode())
	{
		return;
	}
	mEditDigitIntIndex = FMath::Clamp(mEditDigitIntIndex + 1, 0, mEditDigitIntListNum-1);
}

void CSKitDebug_DebugMenuNodeInt::OnJustPressedRightKey()
{
	if (!IsEditMode())
	{
		return;
	}
	mEditDigitIntIndex = FMath::Clamp(mEditDigitIntIndex - 1, 0, mEditDigitIntListNum-1);
}

void CSKitDebug_DebugMenuNodeInt::SetInitValue()
{
	if (!GetNodeData().mInitValue.IsEmpty())
	{
		SetValueString(GetNodeData().mInitValue);
		SetValueInt(GetInt());
	}
}

void CSKitDebug_DebugMenuNodeInt::DrawEditValue(UCanvas* InCanvas, const FVector2D& InValuePos, const FVector2D& InValueExtent) const
{
	const FVector2D SubWindowPos(InValuePos.X+InValueExtent.X+2.f, InValuePos.Y);
	const FVector2D WindowExtent(100.f, InValueExtent.Y);
	const FLinearColor WindowBackColor = GetWindowBackColor();
	const FLinearColor WindowFrameColor = GetWindowFrameColor();
	const FLinearColor FontColor = GetFontColor();
	// 下敷き
	{
		FCanvasTileItem Item(SubWindowPos, WindowExtent, WindowBackColor);
		Item.BlendMode = ESimpleElementBlendMode::SE_BLEND_Translucent;
		InCanvas->DrawItem(Item);
	}
	// 枠
	{
		FCanvasBoxItem Item(SubWindowPos, WindowExtent);
		Item.SetColor(GetSelectColor());
		Item.LineThickness = 3.f;
		InCanvas->DrawItem(Item);
	}

	const float DigitLineLength = 6.f;
	const float EditDigitPosY = SubWindowPos.Y + 15.f;
	for (int32 i = 0; i < mEditDigitIntListNum; ++i)
	{
		const float EditDigitPosX = SubWindowPos.X + 8.f * static_cast<float>(mEditDigitIntListNum - i);
		{
			FString DrawString = FString::Printf(TEXT("%d"), mEditDigitIntList[i]);
			if (i == mEditDigitIntListNum - 1)
			{
				if (mEditDigitIntList[i] >= 0)
				{
					DrawString = FString(TEXT("+"));
				}
				else
				{
					DrawString = FString(TEXT("-"));
				}
			}
			FCanvasTextItem Item(FVector2D(EditDigitPosX, SubWindowPos.Y), FText::FromString(DrawString), GEngine->GetSmallFont(), FontColor);
			Item.Scale = FVector2D(1.f);
			InCanvas->DrawItem(Item);
		}
		if (i == mEditDigitIntIndex)
		{
			const FVector2D LinePosBegin(EditDigitPosX + 1.f, EditDigitPosY);
			const FVector2D LinePosEnd(EditDigitPosX + DigitLineLength, EditDigitPosY);
			const FLinearColor LineColor(0.9f, 0.9f, 0.1f, 1.f);
			DrawDebugCanvas2DLine(InCanvas, LinePosBegin, LinePosEnd, LineColor, 2.f);
		}
	}
}
