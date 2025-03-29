// Copyright 2020 megasuraman
/**
 * @file CSKitDebug_ScreenWindowGraph.cpp
 * @brief デバッグ情報表示用Window　グラフ表示
 * @author megasuraman
 * @date 2025/03/29
 */
#include "ScreenWindow/CSKitDebug_ScreenWindowGraph.h"

#include "Engine/Canvas.h"
#include "Engine/Engine.h"
#include "CanvasItem.h"
#include "DrawDebugHelpers.h"


/**
 * @brief 
 */
void FCSKitDebug_ScreenWindowGraph::InitWindowExtent()
{
	const float GraphWidth = mMaxValueDrawLen.X + mOffsetDrawLenOverMaxValue.X;
	const float GraphHeight = mMaxValueDrawLen.Y + mOffsetDrawLenOverMaxValue.Y;
	const FVector2D WindowScreenExtent(GraphWidth + mWindowSpaceLen * 2.f, GraphHeight + mWindowSpaceLen * 2.f);
	SetWindowExtent(WindowScreenExtent);
}

/**
 * @brief 
 */
void	FCSKitDebug_ScreenWindowGraph::Update(float InDeltaTime)
{
	if (mLineDataList.Num() == 0)
	{
		return;
	}

	mSec += InDeltaTime;
}

/**
 * @brief 
 */
void	FCSKitDebug_ScreenWindowGraph::AddScore(const FName& InLineName, const float InScore)
{
	for (LineData& Data : mLineDataList)
	{
		if (Data.mName == InLineName)
		{
			const ScoreData& LastData = Data.mScoreDataList.GetLast();
			if (mSec - LastData.mSec < mAddScoreInterval)
			{
				return;
			}

			ScoreData TempData;
			TempData.mScore = InScore;
			TempData.mSec = mSec;
			Data.mScoreDataList.Push(TempData);
			Data.mbHide = false;
			return;
		}
	}

	constexpr uint32 ColorNum = 6;
	const FLinearColor ColorList[ColorNum] = {
		FLinearColor::Red,
		FLinearColor::Green,
		FLinearColor::Yellow,
		FLinearColor(0.9f,0.5f,0.5f,1.f),
		FLinearColor(0.9f,0.5f,0.1f,1.f),
		FLinearColor(0.5f,0.2f,0.5f,1.f)
	};

	LineData TempLineData;
	TempLineData.mName = InLineName;
	TempLineData.mDisplayName = InLineName.ToString();
	TempLineData.mColor = ColorList[mLineDataList.Num() % ColorNum];
	mLineDataList.Add(TempLineData);
	{
		LineData& AddLineData = mLineDataList.Last();
		int32 NeedScoreDataNum = 32;
		if (mAddScoreInterval > 0.f)
		{
			NeedScoreDataNum = mSecMaxLen / mAddScoreInterval;
			++NeedScoreDataNum;
		}
		AddLineData.mScoreDataList.ChangeSize(NeedScoreDataNum);
		ScoreData TempData;
		TempData.mScore = InScore;
		TempData.mSec = mSec;
		AddLineData.mScoreDataList.Push(TempData);
	}
}
/**
 * @brief 
 */
void	FCSKitDebug_ScreenWindowGraph::SetScoreLineColor(const FName& InLineName, const FLinearColor InColor)
{
	for (LineData& Data : mLineDataList)
	{
		if (Data.mName == InLineName)
		{
			Data.mColor = InColor;
			break;
		}
	}
}
/**
 * @brief 
 */
void	FCSKitDebug_ScreenWindowGraph::SetScoreLineDisplayName(const FName& InLineName, const FString& InDisplayName)
{
	for (LineData& Data : mLineDataList)
	{
		if (Data.mName == InLineName)
		{
			Data.mDisplayName = InDisplayName;
			break;
		}
	}
}

/**
 * @brief 
 */
void FCSKitDebug_ScreenWindowGraph::SetScoreLineHide(const FName& InLineName, const bool bInHide)
{
	for (LineData& Data : mLineDataList)
	{
		if (Data.mName == InLineName)
		{
			Data.mbHide = bInHide;
			break;
		}
	}
}

/**
 * @brief ガイド用の線追加
 */
void	FCSKitDebug_ScreenWindowGraph::AddGuideScoreLine(const GuideScoreLineData& InData)
{
	mGuideScoreLineData.Add(InData);
}

/**
 * @brief Windowの下敷き表示後処理
 */
void	FCSKitDebug_ScreenWindowGraph::DrawAfterBackground(UCanvas* InCanvas, const FVector2D& InPos2D) const
{
	FLinearColor AxisColor = { 1.f, 1.f, 1.f, 1.f };
	FLinearColor AxisFontColor = { 1.f, 1.f, 1.f, 1.f };
	const float GraphMaxValueWidth = mMaxValueDrawLen.X;
	const float GraphMaxValueHeight = mMaxValueDrawLen.Y;
	const float GraphWidth = GraphMaxValueWidth + mOffsetDrawLenOverMaxValue.X;
	const float GraphHeight = GraphMaxValueHeight + mOffsetDrawLenOverMaxValue.Y;
	const float WindowSpaceLen = mWindowSpaceLen;
	const FVector2D WindowScreenPos = InPos2D;
	//const FVector2D WindowScreenExtent(GraphWidth + WindowSpaceLen * 2.f, GraphHeight + WindowSpaceLen * 2.f);
	//SetWindowExtent(WindowScreenExtent);

	//const float AxisLineOffset = 20.f;
	const FVector2D ZeroScreenPos = WindowScreenPos + FVector2D(0.f, GraphHeight) + FVector2D(WindowSpaceLen, WindowSpaceLen);
	{//軸線
		constexpr float ArrowOffset = 5.f;
		{//x軸
			const FVector2D EndPos(ZeroScreenPos.X + GraphWidth, ZeroScreenPos.Y);
			DrawDebugCanvas2DLine(InCanvas, FVector2D(ZeroScreenPos.X, ZeroScreenPos.Y), EndPos, AxisColor);
			//矢印
			DrawDebugCanvas2DLine(InCanvas, EndPos, EndPos + FVector2D(-ArrowOffset, ArrowOffset), AxisColor);
			DrawDebugCanvas2DLine(InCanvas, EndPos, EndPos + FVector2D(-ArrowOffset, -ArrowOffset), AxisColor);
			{//軸名
				FCanvasTextItem TextItem(EndPos + FVector2D(0.f, ArrowOffset), FText::FromString(FString("sec")), GetUseFont(), AxisFontColor);
				InCanvas->DrawItem(TextItem);
			}
			//最大値
			constexpr float MaxValueLineLen = 5.f;
			const FVector2D MaxValuePos = ZeroScreenPos + FVector2D(GraphMaxValueWidth, 0.f);
			DrawDebugCanvas2DLine(InCanvas, MaxValuePos - FVector2D(0.f, MaxValueLineLen*0.5f), MaxValuePos - FVector2D(0.f, -MaxValueLineLen * 0.5f), AxisColor);
		}
		{//y軸
			const FVector2D EndPos(ZeroScreenPos.X, ZeroScreenPos.Y - GraphHeight);
			DrawDebugCanvas2DLine(InCanvas, FVector2D(ZeroScreenPos.X, ZeroScreenPos.Y), EndPos, AxisColor);
			//矢印
			DrawDebugCanvas2DLine(InCanvas, EndPos, EndPos + FVector2D(ArrowOffset, ArrowOffset), AxisColor);
			DrawDebugCanvas2DLine(InCanvas, EndPos, EndPos + FVector2D(-ArrowOffset, ArrowOffset), AxisColor);

			FCanvasTextItem TextItem(EndPos + FVector2D(ArrowOffset, -ArrowOffset), FText::FromString(mScoreName), GetUseFont(), AxisFontColor);
			InCanvas->DrawItem(TextItem);

			//最大値
			constexpr float MaxValueLineLen = 5.f;
			const FVector2D MaxValuePos = ZeroScreenPos + FVector2D(0.f, -GraphMaxValueHeight);
			DrawDebugCanvas2DLine(InCanvas, MaxValuePos - FVector2D(MaxValueLineLen*0.5f, 0.f), MaxValuePos - FVector2D(-MaxValueLineLen * 0.5f, 0.f), AxisColor);
		}
	}

	//グラフ
	const float MinSec = mSec - mSecMaxLen;
	for (const LineData& Line : mLineDataList)
	{
		if(Line.mbHide)
		{
			continue;
		}
		const FLinearColor Color = Line.mColor;
		const int32 ScoreDataNum = Line.mScoreDataList.GetListNum();
		FVector2D PrevScoreScreenPos;
		for (int32 i = 0; i < ScoreDataNum; ++i)
		{
			const ScoreData& Score = Line.mScoreDataList.GetOrder(i);
			const float ScoreRatio = Score.mScore / mScoreMaxLen;
			float SecRatio = 1.f;
			if (i + 1 != ScoreDataNum)
			{
				if (MinSec > 0.f)
				{
					SecRatio = (Score.mSec - MinSec) / mSecMaxLen;
				}
				else
				{
					SecRatio = Score.mSec / mSecMaxLen;
				}
			}

			FVector2D ScoreScreenPos = ZeroScreenPos + FVector2D(GraphMaxValueWidth*SecRatio, -GraphMaxValueHeight * ScoreRatio);
			if (i >= 1
				&& ScoreScreenPos.X > ZeroScreenPos.X)
			{
				FVector2D LineStartPos = PrevScoreScreenPos;
				if (LineStartPos.X < ZeroScreenPos.X)
				{
					if (ScoreScreenPos.X == LineStartPos.X
						|| ScoreScreenPos.Y == LineStartPos.Y)
					{
						LineStartPos.X = ZeroScreenPos.X;
					}
					else
					{
						const float Slope = (ScoreScreenPos.Y - LineStartPos.Y) / (ScoreScreenPos.X - LineStartPos.X);
						const float Intercept = PrevScoreScreenPos.Y - Slope * PrevScoreScreenPos.X;
						LineStartPos.Y = Slope * ZeroScreenPos.X + Intercept;
						LineStartPos.X = ZeroScreenPos.X;
					}
				}
				DrawDebugCanvas2DLine(InCanvas, LineStartPos, ScoreScreenPos, Color);
				DrawDebugCanvas2DCircle(InCanvas, ScoreScreenPos, 2.f, 4, Color);
			}

			PrevScoreScreenPos = ScoreScreenPos;
		}

		FCanvasTextItem TextItem(PrevScoreScreenPos, FText::FromString(Line.mDisplayName), GetUseFont(), Color);
		InCanvas->DrawItem(TextItem);
	}

	//ガイド
	for (const GuideScoreLineData& Data : mGuideScoreLineData)
	{
		const FVector2D BeginPos = ZeroScreenPos + FVector2D(0.f, -GraphMaxValueHeight * Data.mScore);
		const FVector2D EndPos = ZeroScreenPos + FVector2D(GraphMaxValueWidth, -GraphMaxValueHeight * Data.mScore);
		DrawDebugCanvas2DLine(InCanvas, BeginPos, EndPos, Data.mColor);

		FLinearColor FontColor = Data.mColor;
		FontColor.A = 1.f;
		FCanvasTextItem TextItem(EndPos, FText::FromName(Data.mName), GetUseFont(), FontColor);
		InCanvas->DrawItem(TextItem);
	}
}