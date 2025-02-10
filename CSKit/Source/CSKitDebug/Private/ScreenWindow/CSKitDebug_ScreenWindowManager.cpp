// Copyright 2020 megasuraman
/**
 * @file CSKitDebug_ScreenWindowManager.cpp
 * @brief DebugInfoWindow管理用のManager
 * @author megasuraman
 * @date 2023/02/04
 */
#include "ScreenWindow/CSKitDebug_ScreenWindowManager.h"

#include "ScreenWindow/CSKitDebug_ScreenWindowText.h"
#include "CSKitDebug_Subsystem.h"


UCSKitDebug_ScreenWindowManager::UCSKitDebug_ScreenWindowManager()
{
#if USE_CSKIT_DEBUG
	mTempWindowDataList.Reserve(32);
#endif//USE_CSKIT_DEBUG
}

/**
 * @brief	Window追加
 */
void	UCSKitDebug_ScreenWindowManager::AddWindowBP(const FName InTag, const FText InMessage, const AActor* InFollowActor, float InDispTime)
{
#if USE_CSKIT_DEBUG
	FCSKitDebug_ScreenWindowOption Option;
	Option.mDispTime = InDispTime;
	OnAddWindow(InTag, InMessage.ToString(), InFollowActor, Option);
#endif//USE_CSKIT_DEBUG
}

#if USE_CSKIT_DEBUG
/**
 * @brief	Get
 */
UCSKitDebug_ScreenWindowManager* UCSKitDebug_ScreenWindowManager::Get(UObject* InOwner)
{
	UGameInstance* GameInstance = InOwner->GetWorld()->GetGameInstance();
	UCSKitDebug_Subsystem* CSKitDebugSubsystem = GameInstance->GetSubsystem<UCSKitDebug_Subsystem>();
	return CSKitDebugSubsystem->GetScreenWindowManager();
}

/**
 * @brief	Init
 */
void	UCSKitDebug_ScreenWindowManager::Init()
{
}

/**
 * @brief	Tick
 */
bool	UCSKitDebug_ScreenWindowManager::DebugTick(float InDeltaSecond)
{
	UpdateLifeTime(InDeltaSecond);
	return true;
}

/**
 * @brief	Draw
 */
void	UCSKitDebug_ScreenWindowManager::DebugDraw(UCanvas* InCanvas)
{
	DrawWindow(InCanvas);
}

/**
 * @brief	Window追加
 */
void	UCSKitDebug_ScreenWindowManager::AddWindow(const FName InTag, const FString& InMessage, const AActor* InFollowActor, const FCSKitDebug_ScreenWindowOption& InOption)
{
	OnAddWindow(InTag, InMessage, InFollowActor, InOption);
}
/**
 * @brief	Window追加
 */
void	UCSKitDebug_ScreenWindowManager::OnAddWindow(const FName InTag, const FString& InMessage, const AActor* InFollowActor, const FCSKitDebug_ScreenWindowOption& InOption)
{
	for (FTempWindowData& Data : mTempWindowDataList)
	{
		if (Data.mTagName == InTag)
		{
			Data.mLifeTime = InOption.mDispTime;
			Data.mWindow.SetWindowName(InTag.ToString());
			Data.mWindow.ClearString();
			Data.mWindow.AddText(InMessage);
			Data.mWindow.SetWindowFrameColor(InOption.mFrameColor);
			Data.mFollowTarget = InFollowActor;
			Data.mbActive = true;
			return;
		}
	}

	if (mTempWindowDataList.Num() >= 256)
	{
		return;
	}

	FTempWindowData Data;
	Data.mTagName = InTag;
	Data.mLifeTime = InOption.mDispTime;
	Data.mWindow.SetWindowName(InTag.ToString());
	Data.mWindow.ClearString();
	Data.mWindow.AddText(InMessage);
	Data.mWindow.SetWindowFrameColor(InOption.mFrameColor);
	Data.mFollowTarget = InFollowActor;
	Data.mbActive = true;
	mTempWindowDataList.Add(Data);
}

/**
 * @brief	寿命更新
 */
void UCSKitDebug_ScreenWindowManager::UpdateLifeTime(const float InDeltaSecond)
{
	for (FTempWindowData& Data : mTempWindowDataList)
	{
		if (Data.mbActive)
		{
			Data.mLifeTime -= InDeltaSecond;
			if (Data.mLifeTime <= 0.f)
			{
				Data.mbActive = false;
			}
		}
	}
}

/**
 * @brief	Window表示
 */
void UCSKitDebug_ScreenWindowManager::DrawWindow(UCanvas* InCanvas)
{
	FVector2D DispPos(30.f, 30.f);
	for (FTempWindowData& Data : mTempWindowDataList)
	{
		if (!Data.mbActive)
		{
			continue;
		}
		if (const AActor* FollowActor = Data.mFollowTarget.Get())
		{
			Data.mWindow.Draw(InCanvas, FollowActor->GetActorLocation());
		}
		else
		{
			const FVector2D WindowExtent = Data.mWindow.Draw(InCanvas, DispPos);
			DispPos.X += WindowExtent.X + 10.f;
		}
	}
}

#endif//USE_CSKIT_DEBUG