// Copyright 2022 megasuraman

#include "DebugMenu/CSKitDebug_DebugMenuNodeFloat.h"

#include "CanvasItem.h"
#include "DrawDebugHelpers.h"
#include "Engine/Canvas.h"


void CSKitDebug_DebugMenuNodeFloat::OnBeginAction()
{
	CSKitDebug_DebugMenuNodeBase::OnBeginAction();

	mEditDigitIntList.Empty();
	mEditDigitIntList.Reserve(mEditDigitNum);
	const FString FloatValueString = GetValueString();
	FString IntegerPartString;
	FString FractionalPartString;
	FloatValueString.Split(FString(TEXT(".")), &IntegerPartString, &FractionalPartString);

	FractionalPartString = FractionalPartString.Left(mEditDecimalNum);

	const int32 IntegerPart = FCString::Atoi(*IntegerPartString);
	const int32 AbsIntegerPart = FMath::Abs(IntegerPart);
	const int32 FractionalPart = FCString::Atoi(*FractionalPartString);;
	{
		int32 CalcDigitValue = FractionalPart;
		for (int32 i = 0; i < mEditDecimalNum; ++i)
		{
			const int32 DigitValue = CalcDigitValue % 10;
			mEditDigitIntList.Add(DigitValue);
			CalcDigitValue /= 10;
		}
	}
	{
		int32 CalcDigitValue = AbsIntegerPart;
		for (int32 i = 0; i < mEditIntegralDigitNum; ++i)
		{
			const int32 DigitValue = CalcDigitValue % 10;
			mEditDigitIntList.Add(DigitValue);
			CalcDigitValue /= 10;
		}
	}

	//符号追加
	if (IntegerPart >= 0)
	{
		mEditDigitIntList.Add(1);
	}
	else
	{
		mEditDigitIntList.Add(-1);
	}
}

void CSKitDebug_DebugMenuNodeFloat::OnEndAction(const FCSKitDebug_DebugMenuNodeActionParameter& InParameter)
{
	FString FloatString;
	for (int32 i = 0; i < mEditDecimalNum; ++i)
	{
		FloatString = FString::Printf(TEXT("%d%s"),mEditDigitIntList[i],*FloatString);
	}

	FloatString = FString::Printf(TEXT(".%s"), *FloatString);

	int32 ActiveNumberDigitIndex = mEditDigitNumberNum-1;
	for (int32 i = mEditDigitNumberNum - 1; i >= mEditDecimalNum; --i)
	{
		if (mEditDigitIntList[i] == 0)
		{
			ActiveNumberDigitIndex = i;
		}
	}
	for (int32 i = mEditDecimalNum; i < ActiveNumberDigitIndex; ++i)
	{
		FloatString = FString::Printf(TEXT("%d%s"), mEditDigitIntList[i], *FloatString);
	}

	if (mEditDigitIntList[mEditDigitNum - 1] < 0)
	{
		FloatString = FString::Printf(TEXT("-%s"), *FloatString);
	}

	SetValueString(FloatString);
	
	CSKitDebug_DebugMenuNodeBase::OnEndAction(InParameter);
}

void CSKitDebug_DebugMenuNodeFloat::OnJustPressedUpKey()
{
	if (!IsEditMode())
	{
		return;
	}
	if (mEditDigitIndex == mEditDigitNum - 1)
	{
		mEditDigitIntList[mEditDigitIndex] *= -1;
	}
	else
	{
		if (mEditDigitIntList[mEditDigitIndex] < 9)
		{
			mEditDigitIntList[mEditDigitIndex] += 1;
		}
		else
		{
			mEditDigitIntList[mEditDigitIndex] = 0;
		}
	}
}

void CSKitDebug_DebugMenuNodeFloat::OnJustPressedDownKey()
{
	if (!IsEditMode())
	{
		return;
	}
	if (mEditDigitIndex == mEditDigitNum - 1)
	{
		mEditDigitIntList[mEditDigitIndex] *= -1;
	}
	else
	{
		if (mEditDigitIntList[mEditDigitIndex] > 0)
		{
			mEditDigitIntList[mEditDigitIndex] -= 1;
		}
		else
		{
			mEditDigitIntList[mEditDigitIndex] = 9;
		}
	}
}

void CSKitDebug_DebugMenuNodeFloat::OnJustPressedLeftKey()
{
	if (!IsEditMode())
	{
		return;
	}
	mEditDigitIndex = FMath::Clamp(mEditDigitIndex + 1, 0, mEditDigitNum-1);
}

void CSKitDebug_DebugMenuNodeFloat::OnJustPressedRightKey()
{
	if (!IsEditMode())
	{
		return;
	}
	mEditDigitIndex = FMath::Clamp(mEditDigitIndex - 1, 0, mEditDigitNum-1);
}

void CSKitDebug_DebugMenuNodeFloat::SetInitValue()
{
	FString IntegerPartString;
	FString FractionalPartString;
	if (!GetNodeData().mInitValue.Split(FString(TEXT(".")), &IntegerPartString, &FractionalPartString))
	{
		return;
	}

	FractionalPartString = FractionalPartString.Left(mEditDecimalNum);

	const FString FloatString = FString::Printf(TEXT("%s.%s"),*IntegerPartString, *FractionalPartString);
	SetValueString(FloatString);
}

void CSKitDebug_DebugMenuNodeFloat::DrawEditValue(UCanvas* InCanvas, const FVector2D& InValuePos, const FVector2D& InValueExtent) const
{
	const FVector2D SubWindowPos(InValuePos.X+InValueExtent.X+2.f, InValuePos.Y);
	const FVector2D WindowExtent(120.f, InValueExtent.Y);
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
		Item.SetColor(WindowFrameColor);
		Item.LineThickness = 3.f;
		InCanvas->DrawItem(Item);
	}

	constexpr float DigitLineLength = 6.f;
	const float EditDigitPosY = SubWindowPos.Y + 15.f;
	for (int32 i = mEditDecimalNum; i < mEditDigitNum; ++i)
	{
		const float EditDigitPosX = SubWindowPos.X + 8.f * static_cast<float>(mEditDigitNum - i);
		{
			FString DrawString = FString::Printf(TEXT("%d"), mEditDigitIntList[i]);
			if (i == mEditDigitNum - 1)
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
		if (i == mEditDigitIndex)
		{
			const FVector2D LinePosBegin(EditDigitPosX + 1.f, EditDigitPosY);
			const FVector2D LinePosEnd(EditDigitPosX + DigitLineLength, EditDigitPosY);
			constexpr FLinearColor LineColor(0.9f, 0.9f, 0.1f, 1.f);
			DrawDebugCanvas2DLine(InCanvas, LinePosBegin, LinePosEnd, LineColor, 2.f);
		}
	}
	{//表示
		const float EditDigitPosX = 8.f + SubWindowPos.X + 8.f * static_cast<float>(mEditDigitNum - mEditDecimalNum);
		FString DrawString = FString::Printf(TEXT("."));
		FCanvasTextItem Item(FVector2D(EditDigitPosX, SubWindowPos.Y), FText::FromString(DrawString), GEngine->GetSmallFont(), FontColor);
		Item.Scale = FVector2D(1.f);
		InCanvas->DrawItem(Item);
	}
	for (int32 i = 0; i < mEditDecimalNum; ++i)
	{
		const float EditDigitPosX = 4.f + SubWindowPos.X + 8.f * static_cast<float>(mEditDigitNum - i);
		{
			FString DrawString = FString::Printf(TEXT("%d"), mEditDigitIntList[i]);
			FCanvasTextItem Item(FVector2D(EditDigitPosX, SubWindowPos.Y), FText::FromString(DrawString), GEngine->GetSmallFont(), FontColor);
			Item.Scale = FVector2D(1.f);
			InCanvas->DrawItem(Item);
		}
		if (i == mEditDigitIndex)
		{
			const FVector2D LinePosBegin(EditDigitPosX + 1.f, EditDigitPosY);
			const FVector2D LinePosEnd(EditDigitPosX + DigitLineLength, EditDigitPosY);
			constexpr FLinearColor LineColor(0.9f, 0.9f, 0.1f, 1.f);
			DrawDebugCanvas2DLine(InCanvas, LinePosBegin, LinePosEnd, LineColor, 2.f);
		}
	}
}
