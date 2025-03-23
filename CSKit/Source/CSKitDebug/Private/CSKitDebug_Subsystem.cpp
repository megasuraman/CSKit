// Copyright 2020 megasuraman
/**
 * @file CSKitDebug_Subsystem.cpp
 * @brief CSKitDebug_Subsystem
 * @author megasuraman
 * @date 2020/7/24
 */
#include "CSKitDebug_Subsystem.h"

#include "ActorSelect/CSKitDebug_ActorSelectManager.h"
#include "CSKitDebug_Config.h"
#include "CSKitDebug_ShortcutCommand.h"
#include "Debug/DebugDrawService.h"
#include "DebugMenu/CSKitDebug_DebugMenuManager.h"
#include "Engine/Engine.h"
#include "ScreenWindow/CSKitDebug_ScreenWindowManager.h"

DEFINE_LOG_CATEGORY(CSKitDebugLog);

FCSKitDebug_SaveData UCSKitDebug_Subsystem::mSaveData;

FCSKitDebug_SaveData& UCSKitDebug_Subsystem::sGetSaveData()
{
	mSaveData.Load();
	return mSaveData;
}

/**
 * @brief 
 */
UCSKitDebug_ScreenWindowManager* UCSKitDebug_Subsystem::GetScreenWindowManagerBP() const
{
#if USE_CSKIT_DEBUG
	return mGCObject.mScreenWindowManager;
#else
	return nullptr;
#endif
}

#if USE_CSKIT_DEBUG

/**
 * @brief Initialize
 */
void	UCSKitDebug_Subsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	RequestTick(true);
	RequestDraw(true);

	if (mGCObject.mDebugMenuManager == nullptr)
	{
		const UCSKitDebug_Config* CSKitDebugConfig = GetDefault<UCSKitDebug_Config>();
		mGCObject.mDebugMenuManager = NewObject<UCSKitDebug_DebugMenuManager>(this, CSKitDebugConfig->mDebugMenuManagerClass);
		mGCObject.mDebugMenuManager->Init();
	}
	if (mGCObject.mShortcutCommand == nullptr)
	{
		mGCObject.mShortcutCommand = NewObject<UCSKitDebug_ShortcutCommand>(this);
		mGCObject.mShortcutCommand->Init();
	}
	if (mGCObject.mActorSelectManager == nullptr)
	{
		mGCObject.mActorSelectManager = NewObject<UCSKitDebug_ActorSelectManager>(this);
		mGCObject.mActorSelectManager->Init();
	}
	if (mGCObject.mScreenWindowManager == nullptr)
	{
		mGCObject.mScreenWindowManager = NewObject<UCSKitDebug_ScreenWindowManager>(this);
		mGCObject.mScreenWindowManager->Init();
	}
}
/**
 * @brief Deinitialize
 */
void	UCSKitDebug_Subsystem::Deinitialize()
{
	RequestTick(false);
	RequestDraw(false);
}

/**
 * @brief	Tickのon/off
 */
void	UCSKitDebug_Subsystem::RequestTick(const bool bInActive)
{
	if (bInActive)
	{
		if (!mDebugTickHandle.IsValid())
		{
			mDebugTickHandle = FTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateUObject(this, &UCSKitDebug_Subsystem::DebugTick));
		}
	}
	else
	{
		FTicker::GetCoreTicker().RemoveTicker(mDebugTickHandle);
	}
}

/**
 * @brief	Drawのon/off
 */
void	UCSKitDebug_Subsystem::RequestDraw(const bool bInActive)
{
	if (bInActive)
	{
		if (!mDebugDrawHandle.IsValid())
		{
			auto DebugDrawDelegate = FDebugDrawDelegate::CreateUObject(this, &UCSKitDebug_Subsystem::DebugDraw);
			if (DebugDrawDelegate.IsBound())
			{
				mDebugDrawHandle = UDebugDrawService::Register(TEXT("GameplayDebug"), DebugDrawDelegate);
			}
		}
	}
	else
	{
		if (mDebugDrawHandle.IsValid())
		{
			UDebugDrawService::Unregister(mDebugDrawHandle);
			mDebugDrawHandle.Reset();
		}
	}
}

/**
 * @brief	Tick
 */
bool	UCSKitDebug_Subsystem::DebugTick(float InDeltaSecond) const
{
	const UCSKitDebug_Config* CSKitDebugConfig = GetDefault<UCSKitDebug_Config>();
	if (!CSKitDebugConfig->mbActiveCSKitDebug)
	{
		return true;
	}

	if (mGCObject.mShortcutCommand)
	{
		mGCObject.mShortcutCommand->DebugTick(InDeltaSecond);
	}
	if (mGCObject.mActorSelectManager)
	{
		mGCObject.mActorSelectManager->DebugTick(InDeltaSecond);
	}
	if (mGCObject.mDebugMenuManager)
	{
		mGCObject.mDebugMenuManager->DebugTick(InDeltaSecond);
	}
	if (mGCObject.mScreenWindowManager)
	{
		mGCObject.mScreenWindowManager->DebugTick(InDeltaSecond);
	}

	return true;
}
/**
 * @brief	Draw
 */
void	UCSKitDebug_Subsystem::DebugDraw(UCanvas* InCanvas, APlayerController* InPlayerController) const
{
	if (mGCObject.mShortcutCommand)
	{
		mGCObject.mShortcutCommand->DebugDraw(InCanvas);
	}
	if (mGCObject.mActorSelectManager)
	{
		mGCObject.mActorSelectManager->DebugDraw(InCanvas);
	}
	if (mGCObject.mDebugMenuManager)
	{
		mGCObject.mDebugMenuManager->DebugDraw(InCanvas);
	}
	if (mGCObject.mScreenWindowManager)
	{
		mGCObject.mScreenWindowManager->DebugDraw(InCanvas);
	}
}
#endif//USE_CSKIT_DEBUG