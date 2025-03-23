// Copyright 2020 megasuraman
/**
 * @file CSKitDebug_ScreenWindowBase.cpp
 * @brief デバッグ情報表示用WindowBase
 * @author megasuraman
 * @date 2021/12/27
 */


#include "ScreenWindow/CSKitDebug_ScreenWindowBase.h"


#include "Engine/Canvas.h"
#include "Engine/Engine.h"
#include "CanvasItem.h"
#include "DrawDebugHelpers.h"
#include "RenderUtils.h"


 /**
  * @brief Window表示(2D座標)
  */
FVector2D	FCSKitDebug_ScreenWindowBase::Draw(UCanvas* InCanvas, const FVector2D& InPos2D) const
{
	if (mWindowExtent.IsZero())
	{
		return FVector2D::ZeroVector;
	}
	float WindowNameWidth = 0.f;
	if (mWindowName.Len() > 0)
	{
		WindowNameWidth = DrawWindowName(InCanvas, InPos2D);
	}

	FVector2D WindowExtent = mWindowExtent;
	WindowExtent.X = FMath::Max(WindowExtent.X, WindowNameWidth);
	// 下敷き
	{
		FCanvasTileItem Item(InPos2D, WindowExtent, mWindowBackColor);
		Item.BlendMode = ESimpleElementBlendMode::SE_BLEND_Translucent;
		InCanvas->DrawItem(Item);
	}
	// 枠
	{
		FCanvasBoxItem Item(InPos2D, WindowExtent);
		Item.SetColor(mWindowFrameColor);
		InCanvas->DrawItem(Item);
	}

	DrawAfterBackground(InCanvas, InPos2D);

	return WindowExtent;
}

/**
 * @brief Window表示(Windowサイズ比での2D座標)
 */
FVector2D	FCSKitDebug_ScreenWindowBase::Draw(UCanvas* InCanvas, const float InPosRatioX, const float InPosRatioY) const
{
	const FVector2D ScreenPos(InCanvas->ClipX * InPosRatioX, InCanvas->ClipY * InPosRatioY);
	return Draw(InCanvas, ScreenPos);
}

/**
 * @brief Window表示(3D座標指定)
 */
FVector2D	FCSKitDebug_ScreenWindowBase::Draw(UCanvas* InCanvas, const FVector& InPos, const float InBorderDistance) const
{
	const FVector ProjectPos = InCanvas->Project(InPos);
	if (ProjectPos.X < 0.f
		|| ProjectPos.X > InCanvas->SizeX
		|| ProjectPos.Y < 0.f
		|| ProjectPos.Y > InCanvas->SizeY
		|| ProjectPos.Z <= 0.f)
	{
		return FVector2D::ZeroVector;
	}

	if (InBorderDistance > 0.f)
	{
		const FSceneView* View = InCanvas->SceneView;
		if (View
			&& FVector::DistSquared(View->ViewMatrices.GetViewOrigin(), InPos) > FMath::Square(InBorderDistance))
		{
			return FVector2D::ZeroVector;
		}
	}

	const FVector2D ScreenPos(ProjectPos);
	return Draw(InCanvas, ScreenPos);
}

/**
 * @brief Window名部分表示
 */
float	FCSKitDebug_ScreenWindowBase::DrawWindowName(UCanvas* InCanvas, const FVector2D& InPos2D) const
{
	constexpr float WindowInsideOffset = 6.f;
	constexpr float WindowWidthSpace = 4.f;
	constexpr float WindowHeightSpace = 2.f;
	float BaseWindowWidth = 0.f;
	float BaseWindowHeight = 0.f;
	CalcTextDrawWidthHeight(BaseWindowWidth, BaseWindowHeight, InCanvas, mWindowName);
	BaseWindowWidth += WindowInsideOffset * 2.f + WindowWidthSpace*2.f;
	BaseWindowHeight += WindowHeightSpace * 2.f;

	FVector2D WindowEdgePos = InPos2D;
	WindowEdgePos.Y -= BaseWindowHeight;
	constexpr uint32 WindowPointListSize = 4;
	const FVector2D WindowPointList[WindowPointListSize] = {
		FVector2D(WindowEdgePos.X + WindowInsideOffset, WindowEdgePos.Y),//左上
		FVector2D(WindowEdgePos.X, WindowEdgePos.Y + BaseWindowHeight),//左下
		FVector2D(WindowEdgePos.X + BaseWindowWidth, WindowEdgePos.Y + BaseWindowHeight),//右下
		FVector2D(WindowEdgePos.X + BaseWindowWidth - WindowInsideOffset, WindowEdgePos.Y)//右上
	};
	for (uint32 i = 0; i < WindowPointListSize; ++i)
	{//枠
		DrawDebugCanvas2DLine(InCanvas, WindowPointList[i], WindowPointList[(i + 1) % WindowPointListSize], mWindowFrameColor);
	}
	{//下敷き
		{
			FCanvasTriangleItem TileItem(WindowPointList[0], WindowPointList[1], WindowPointList[2], GWhiteTexture);
			TileItem.SetColor(mWindowBackColor);
			TileItem.BlendMode = SE_BLEND_Translucent;
			InCanvas->DrawItem(TileItem);
		}
		{
			FCanvasTriangleItem TileItem(WindowPointList[2], WindowPointList[3], WindowPointList[0], GWhiteTexture);
			TileItem.SetColor(mWindowBackColor);
			TileItem.BlendMode = SE_BLEND_Translucent;
			InCanvas->DrawItem(TileItem);
		}
	}
	FVector2D TextPos = WindowEdgePos;
	TextPos.X += WindowInsideOffset + WindowWidthSpace;
	TextPos.Y += WindowHeightSpace;
	FCanvasTextItem TextItem(TextPos, FText::FromString(mWindowName), GetUseFont(), mWindowNameColor);
	TextItem.Scale = FVector2D(mFontScale);
	InCanvas->DrawItem(TextItem);

	return BaseWindowWidth;
}
/**
 * @brief	使用フォント取得
 * @param
 * @return
 */
UFont* FCSKitDebug_ScreenWindowBase::GetUseFont()
{
	return GEngine->GetMediumFont();
}
/**
 * @brief	テキスト描画時の幅と高さを取得
 * @param
 * @return
 */
void	FCSKitDebug_ScreenWindowBase::CalcTextDrawWidthHeight(float& OutWidth, float& OutHeight, UCanvas* InCanvas, const FString& InText)
{
	//InCanvas->TextSize(GetUseFont(), InText, OutWidth, OutHeight, mFontScale, mFontScale);
	InCanvas->StrLen(GetUseFont(), InText, OutWidth, OutHeight, true);
	//OutWidth *= 1.15f;//何故かズレる大きめに適当な調整(4.25だと変？)
}