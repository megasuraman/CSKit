// Copyright 2022 megasuraman

#include "DebugMenu/CSKitDebug_DebugMenuNodeList.h"


void CSKitDebug_DebugMenuNodeList::OnBeginAction()
{
	CSKitDebug_DebugMenuNodeBase::OnBeginAction();
	mEditSelectIndex = GetSelectIndex();

	const int32 ListNum = GetNodeData().mList.Num();
	mEditDrawIndexMin = 0;
	mEditDrawIndexMax = ListNum-1;
	if (ListNum >= mEditDrawListNum)
	{
		if (mEditSelectIndex < mEditDrawListInitCursolOffset)
		{
			mEditDrawIndexMin = 0;
			mEditDrawIndexMax = mEditDrawListNum - 1;
		}
		else if(mEditSelectIndex > ListNum - mEditDrawListInitCursolOffset)
		{
			mEditDrawIndexMin = ListNum - mEditDrawListNum;
			mEditDrawIndexMax = ListNum - 1;
		}
		else
		{
			mEditDrawIndexMin = FMath::Max(mEditSelectIndex - mEditDrawListInitCursolOffset, 0);
			mEditDrawIndexMax = FMath::Min(mEditDrawIndexMin + mEditDrawListNum, ListNum - 1);
		}
	}
}

void CSKitDebug_DebugMenuNodeList::OnEndAction(const FCSKitDebug_DebugMenuNodeActionParameter& InParameter)
{
	CSKitDebug_DebugMenuNodeBase::OnEndAction(InParameter);

	SetValueList(mEditSelectIndex);
}

void CSKitDebug_DebugMenuNodeList::OnJustPressedUpKey()
{
	if (!IsEditMode())
	{
		return;
	}
	mEditSelectIndex -= 1;

	const int32 ListNum = GetNodeData().mList.Num();
	if (mEditSelectIndex < 0)
	{
		mEditSelectIndex = ListNum - 1;
		if (ListNum >= mEditDrawListNum)
		{
			mEditDrawIndexMin = mEditSelectIndex - mEditDrawListNum + 1;
			mEditDrawIndexMax = mEditSelectIndex;
		}
	}
	else
	{
		if (ListNum >= mEditDrawListNum
			&& mEditSelectIndex < mEditDrawIndexMin
			&& mEditDrawIndexMin > 0)
		{
			mEditDrawIndexMin -= 1;
			mEditDrawIndexMax -= 1;
		}
	}
}

void CSKitDebug_DebugMenuNodeList::OnJustPressedDownKey()
{
	if (!IsEditMode())
	{
		return;
	}
	mEditSelectIndex += 1;

	const int32 ListNum = GetNodeData().mList.Num();
	if(mEditSelectIndex >= ListNum)
	{
		mEditSelectIndex = 0;
		if (ListNum >= mEditDrawListNum)
		{
			mEditDrawIndexMin = 0;
			mEditDrawIndexMax = mEditSelectIndex + mEditDrawListNum - 1;
		}
	}
	else
	{
		if (ListNum >= mEditDrawListNum
			&& mEditSelectIndex > mEditDrawIndexMax
			&& mEditDrawIndexMax < ListNum-1)
		{
			mEditDrawIndexMin += 1;
			mEditDrawIndexMax += 1;
		}
	}

}

void CSKitDebug_DebugMenuNodeList::OnJustPressedLeftKey()
{
	if (!IsEditMode())
	{
		return;
	}
	mEditSelectIndex -= 5;

	const int32 ListNum = GetNodeData().mList.Num();
	if (mEditSelectIndex < 0)
	{
		mEditSelectIndex = 0;
		if (ListNum >= mEditDrawListNum)
		{
			mEditDrawIndexMin = 0;
			mEditDrawIndexMax = mEditDrawIndexMin + mEditDrawListNum - 1;
		}
	}
	else
	{
		if (ListNum >= mEditDrawListNum
			&& mEditSelectIndex < mEditDrawIndexMin)
		{
			mEditDrawIndexMin = mEditSelectIndex;
			mEditDrawIndexMax = mEditDrawIndexMin + mEditDrawListNum;
		}
	}
}

void CSKitDebug_DebugMenuNodeList::OnJustPressedRightKey()
{
	if (!IsEditMode())
	{
		return;
	}
	mEditSelectIndex += 5;

	const int32 ListNum = GetNodeData().mList.Num();
	if (mEditSelectIndex >= ListNum)
	{
		mEditSelectIndex = ListNum - 1;
		if (ListNum >= mEditDrawListNum)
		{
			mEditDrawIndexMax = ListNum - 1;
			mEditDrawIndexMin = mEditDrawIndexMax - mEditDrawListNum + 1;
		}
	}
	else
	{
		if (ListNum >= mEditDrawListNum
			&& mEditSelectIndex > mEditDrawIndexMax)
		{
			mEditDrawIndexMax = mEditSelectIndex;
			mEditDrawIndexMin = mEditDrawIndexMax - mEditDrawListNum + 1;
		}
	}
}

void CSKitDebug_DebugMenuNodeList::SetInitValue()
{
	if (GetNodeData().mInitValue.IsEmpty())
	{
		return;
	}

	const int32 InitIndex = GetNodeData().mList.Find(GetNodeData().mInitValue);
	if (InitIndex == INDEX_NONE)
	{
		SetValueString(GetNodeData().mInitValue);
		SetValueInt(GetInt());
		return;
	}
	
	SetValueList(InitIndex);
}

void CSKitDebug_DebugMenuNodeList::DrawEditValue(UCanvas* InCanvas, const FVector2D& InValuePos, const FVector2D& InValueExtent) const
{
	const FVector2D SubWindowPos(InValuePos.X+InValueExtent.X+2.f, InValuePos.Y);
	const FVector2D WindowExtent(100.f, InValueExtent.Y);
	const FVector2D StringOffset(2.f, 2.f);
	const FLinearColor WindowBackColor = GetWindowBackColor();
	const FLinearColor WindowFrameColor = GetWindowFrameColor();
	const FLinearColor FontColor = GetFontColor();

	const TArray<FString>& StringList = GetNodeData().mList;
	const int32 StringListNum = StringList.Num();
	FVector2D SelectWindowPos = SubWindowPos;
	FVector2D DrawWindowPos = SubWindowPos;

	if(StringListNum > mEditDrawListNum)
	{//スクロールバー
		const FVector2D ScrollBarFramePos = FVector2D(SubWindowPos.X + WindowExtent.X, SubWindowPos.Y);
		const FVector2D ScrollBarFrameExtent(10.f, WindowExtent.Y * static_cast<float>(mEditDrawListNum));
		{// 下敷き
			FCanvasTileItem Item(ScrollBarFramePos, ScrollBarFrameExtent, WindowBackColor);
			Item.BlendMode = ESimpleElementBlendMode::SE_BLEND_Translucent;
			InCanvas->DrawItem(Item);
		}
		{// 枠
			FCanvasBoxItem Item(ScrollBarFramePos, ScrollBarFrameExtent);
			Item.SetColor(GetWindowFrameColor());
			Item.LineThickness = 1.f;
			InCanvas->DrawItem(Item);
		}
		{//バー
			const float SpaceRatio = static_cast<float>(mEditDrawListNum) / static_cast<float>(StringListNum);

			FVector2D ScrollBarPos = ScrollBarFramePos + FVector2D(2.f,2.f);
			const float ScrollBarPosRatio = mEditDrawIndexMin / static_cast<float>(StringListNum);
			ScrollBarPos.Y += ScrollBarPosRatio * ScrollBarFrameExtent.Y;

			FVector2D ScrollBarExtent = ScrollBarFrameExtent - FVector2D(4.f,4.f);
			ScrollBarExtent.Y *= SpaceRatio;

			FCanvasTileItem Item(ScrollBarPos, ScrollBarExtent, GetWindowFrameColor());
			Item.BlendMode = ESimpleElementBlendMode::SE_BLEND_Opaque;
			InCanvas->DrawItem(Item);
		}
	}

	for(int32 i= mEditDrawIndexMin; i<=mEditDrawIndexMax; ++i)
	{
		// 下敷き
		{
			FCanvasTileItem Item(DrawWindowPos, WindowExtent, WindowBackColor);
			Item.BlendMode = ESimpleElementBlendMode::SE_BLEND_Translucent;
			InCanvas->DrawItem(Item);
		}
		// 枠
		{
			FCanvasBoxItem Item(DrawWindowPos, WindowExtent);
			Item.SetColor(GetWindowFrameColor());
			Item.LineThickness = 1.f;
			InCanvas->DrawItem(Item);
		}

		const FString DrawString = StringList[i];
		FCanvasTextItem Item(DrawWindowPos + StringOffset, FText::FromString(DrawString), GEngine->GetSmallFont(), FontColor);
		Item.Scale = FVector2D(1.f);
		InCanvas->DrawItem(Item);

		if (i == mEditSelectIndex)
		{
			SelectWindowPos = DrawWindowPos;
		}

		DrawWindowPos.Y += WindowExtent.Y;
	}
	{//選択枠
		FCanvasBoxItem Item(SelectWindowPos, WindowExtent);
		Item.SetColor(GetSelectColor());
		Item.LineThickness = 3.f;
		InCanvas->DrawItem(Item);
	}

#if 0
	{//デバッグ表示
		const FString DrawString = FString::Printf(TEXT("mEditSelectIndex(%d) %d-%d"),mEditSelectIndex,mEditDrawIndexMin,mEditDrawIndexMax);
		FCanvasTextItem Item(SubWindowPos + WindowExtent, FText::FromString(DrawString), GEngine->GetSmallFont(), FColor::Red);
		Item.Scale = FVector2D(1.f);
		InCanvas->DrawItem(Item);
	}
#endif
}
