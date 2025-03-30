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
#include "CSKitDebug_GhostController.h"
#include "CSKitDebug_GhostPawn.h"
#include "CSKitDebug_ShortcutCommand.h"
#include "Debug/DebugDrawService.h"
#include "DebugMenu/CSKitDebug_DebugMenuManager.h"
#include "Engine/Engine.h"
#include "Engine/Player.h"
#include "GameFramework/PlayerController.h"
#include "ScreenWindow/CSKitDebug_ScreenWindowManager.h"

DEFINE_LOG_CATEGORY(CSKitDebugLog);
DEFINE_LOG_CATEGORY_STATIC(LogCSKitDebug_SubSystem, Warning, All);

FCSKitDebug_SaveData UCSKitDebug_Subsystem::mSaveData;

void UCSKitDebug_Subsystem::sOneShotWarning(const UWorld* InWorld, const bool bInExpression, const FName& InKey,
	const FString& InLog)
{
	if(InWorld == nullptr)
	{
		return;
	}
	if (const UGameInstance* GameInstance = InWorld->GetGameInstance())
	{
		if (UCSKitDebug_Subsystem* CSKitDebugSubsystem = GameInstance->GetSubsystem<UCSKitDebug_Subsystem>())
		{
			CSKitDebugSubsystem->OneShotWarning(bInExpression, InKey, InLog);
		}
	}
}

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

/**
 * @brief プレイ中に一回きりに停止させるWarning
 *		直接ensure使うとEditor起動後に一回しかヒットしなくて、プレイし直す際に困るので
 */
void UCSKitDebug_Subsystem::OneShotWarning(const bool bInExpression, const FName& InKey, const FString& InLog)
{
	if (bInExpression
		|| mOneShotWarningKeyList.Find(InKey) != INDEX_NONE)
	{
		return;
	}

	UE_LOG(LogCSKitDebug_SubSystem, Warning, TEXT("[OneShotWarning] %s"), *InLog);
#if WITH_EDITOR
	UE_DEBUG_BREAK();
#endif
	mOneShotWarningKeyList.Add(InKey);
}

void UCSKitDebug_Subsystem::BeginGhostController(AActor* InTarget)
{
	APlayerController* PlayerController = nullptr;
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		if (APlayerController* CheckPlayerController = Iterator->Get())
		{
			if (CheckPlayerController->Player)
			{
				PlayerController = CheckPlayerController;
				break;
			}
		}
	}

	if(!mOriginalPlayerController.IsValid())
	{
		mOriginalPlayerController = PlayerController;
	}

	if (PlayerController
		&& PlayerController->Player
		&& PlayerController->IsLocalPlayerController())
	{
		if (mGCObject.mGhostController == nullptr)
		{
			// spawn if necessary
			FActorSpawnParameters SpawnInfo;
			SpawnInfo.Instigator = PlayerController->GetInstigator();
			mGCObject.mGhostController = GetWorld()->SpawnActor<ACSKitDebug_GhostController>(ACSKitDebug_GhostController::StaticClass(), SpawnInfo);
			GetWorld()->AddController(mGCObject.mGhostController);
		}

		if (mGCObject.mGhostController)
		{
			if(mGCObject.mGhostPawn == nullptr)
			{
				FActorSpawnParameters SpawnInfo;
				SpawnInfo.Instigator = PlayerController->GetInstigator();
				mGCObject.mGhostPawn = GetWorld()->SpawnActor<ACSKitDebug_GhostPawn>(ACSKitDebug_GhostPawn::StaticClass(), SpawnInfo);
			}
			mGCObject.mGhostController->Possess(mGCObject.mGhostPawn);
			PlayerController->Player->SwitchController(mGCObject.mGhostController);
		}
	}

	if(mGCObject.mGhostPawn)
	{
		const FAttachmentTransformRules Rules(EAttachmentRule::SnapToTarget, true);
		mGCObject.mGhostPawn->AttachToActor(InTarget, Rules);
		mGCObject.mGhostPawn->OnPostAttachedActor(InTarget);
	}
}

void UCSKitDebug_Subsystem::EndGhostController()
{
	APlayerController* OriginalPlayerController = mOriginalPlayerController.Get();
	if(OriginalPlayerController == nullptr
		|| mGCObject.mGhostController == nullptr)
	{
		return;
	}

	mGCObject.mGhostController->Player->SwitchController(OriginalPlayerController);
	mOriginalPlayerController = nullptr;
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