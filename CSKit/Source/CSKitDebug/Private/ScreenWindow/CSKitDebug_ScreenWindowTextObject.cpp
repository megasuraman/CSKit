// Copyright 2020 megasuraman
/**
 * @file CSKitDebug_ScreenWindowTextObject.cpp
 * @brief デバッグ情報表示用WindowのBlueprint用
 * @author megasuraman
 * @date 2021/12/27
 */


#include "ScreenWindow/CSKitDebug_ScreenWindowTextObject.h"

#include "Engine/Canvas.h"
#include "Engine/Engine.h"
#include "CanvasItem.h"
#include "Debug/DebugDrawService.h"


UCSKitDebug_ScreenWindowTextObject::UCSKitDebug_ScreenWindowTextObject(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
	,mbUsePreDrawDelegate(false)
{

}

void	UCSKitDebug_ScreenWindowTextObject::BeginDestroy()
{
	Super::BeginDestroy();

	EndDraw();
}

/**
 * @brief 表示開始
 */
void	UCSKitDebug_ScreenWindowTextObject::BeginDraw()
{
	if (!mDebugDrawHandle.IsValid())
	{
		auto DebugDrawDelegate = FDebugDrawDelegate::CreateUObject(this, &UCSKitDebug_ScreenWindowTextObject::DebugDraw);
		if (DebugDrawDelegate.IsBound())
		{
			mDebugDrawHandle = UDebugDrawService::Register(TEXT("GameplayDebug"), DebugDrawDelegate);
		}
	}
}

/**
 * @brief 表示終了
 */
void	UCSKitDebug_ScreenWindowTextObject::EndDraw()
{
	if (mDebugDrawHandle.IsValid())
	{
		UDebugDrawService::Unregister(mDebugDrawHandle);
		mDebugDrawHandle.Reset();
	}
}

/**
 * @brief 表示
 */
void	UCSKitDebug_ScreenWindowTextObject::DebugDraw(UCanvas* InCanvas, APlayerController* InPlayerController)
{
	if (mbUsePreDrawDelegate)
	{
		mPreDrawDelegate.Execute();
	}

	const AActor* TargetActor = mDrawTargetActor.Get();
	if (TargetActor)
	{
		mDebugInfoWindow.Draw(InCanvas, TargetActor->GetActorLocation());
	}
	else
	{
		mDebugInfoWindow.Draw(InCanvas, mDrawPos2D);
	}

	mDebugInfoWindow.ClearString();
}