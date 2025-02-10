// Copyright 2020 megasuraman
/**
 * @file CSKitEditor_EUW_ViewportHelper.cpp
 * @brief UObjectのProperty値表示
 * @author megasuraman
 * @date 2022/08/07
 */


#include "EditorUtilityWidget/CSKitEditor_EUW_ViewportHelper.h"
#include "Editor.h"
#include "EditorViewportClient.h"
#include "LevelEditor.h"
#include "SLevelViewport.h"
#include "Debug/DebugDrawService.h"
#include "ScreenWindow/CSKitDebug_ScreenWindowText.h"
#include "LevelEditorViewport.h"

/**
 * @brief	カメラを指定座標に向ける
 */
void	UCSKitEditor_EUW_ViewportHelper::LookAt(const FString& InString)
{
	mLastLookAtLocation.InitFromString(InString);
	LookAtPos(mLastLookAtLocation);

	SetRealTimeDraw_LevelEditorViewport(true);
}

/**
 * @brief	表示on/off
 */
void	UCSKitEditor_EUW_ViewportHelper::RequestDraw(bool bInDraw)
{
	SetActiveDraw(bInDraw);
}

/**
 * @brief	描画
 */
void	UCSKitEditor_EUW_ViewportHelper::Draw(UCanvas* InCanvas, APlayerController* InPlayerController)
{
	Super::Draw(InCanvas, InPlayerController);

	if (GCurrentLevelEditingViewportClient)
	{
		const FVector LookAtPos = GCurrentLevelEditingViewportClient->GetLookAtLocation();
		FCSKitDebug_ScreenWindowText DebugInfo;
		DebugInfo.SetWindowName(FString(TEXT("ViewportHelper")));
		DebugInfo.AddText(FString::Printf(TEXT("LootAtPos : %s"), *LookAtPos.ToString()));
		DebugInfo.Draw(InCanvas, LookAtPos);
	}
}