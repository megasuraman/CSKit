// Copyright 2020 megasuraman
/**
 * @file CSKitDebug_AutoPilotComponent.cpp
 * @brief PlayerControllerに対して自動入力等を補助するComponent
 * @author megasuraman
 * @date 2021/12/28
 */


#include "AutoPilot/CSKitDebug_AutoPilotComponent.h"
#include "AutoPilot/CSKitDebug_AutoPilotModeRecord.h"

#include "Kismet/GameplayStatics.h"
#include "Debug/DebugDrawService.h"
#include "InputCoreTypes.h"

// Sets default values for this component's properties
UCSKitDebug_AutoPilotComponent::UCSKitDebug_AutoPilotComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UCSKitDebug_AutoPilotComponent::BeginPlay()
{
	Super::BeginPlay();

	RequestDebugDraw(true);
}
void UCSKitDebug_AutoPilotComponent::BeginDestroy()
{
	Super::BeginDestroy();

	RequestDebugDraw(false);
 	SetFixFrameRate(false);
 	SetIgnoreDefaultInput(false);
}

/**
 * @brief PlayerInputの処理前
 */
void UCSKitDebug_AutoPilotComponent::PreProcessInput(float DeltaTime) const
{
	if (UGameplayStatics::IsGamePaused(GetWorld()))
	{
		return;
	}

 	if (mActiveMode)
 	{
 		mActiveMode->PreProcessInput(DeltaTime);
 	}
}

/**
 * @brief PlayerInputの処理後
 */
void UCSKitDebug_AutoPilotComponent::PostProcessInput(float DeltaTime) const
{
	if (UGameplayStatics::IsGamePaused(GetWorld()))
	{
		return;
	}

 	if (mActiveMode)
 	{
 		mActiveMode->PostProcessInput(DeltaTime);
 	}
}

/**
 * @brief APlayerControllerを取得
 */
APlayerController* UCSKitDebug_AutoPilotComponent::GetPlayerController() const
{
	return Cast<APlayerController>(GetOwner());
}

/**
 * @brief Modeセット
 */
void UCSKitDebug_AutoPilotComponent::SetMode(ECSKitDebug_AutoPilotMode InMode)
{
	if (mMode == InMode)
	{
		return;
	}

	OnEndMode();

	mMode = InMode;
	if (mActiveMode != nullptr)
	{
		mActiveMode->MarkPendingKill();
		mActiveMode = nullptr;
	}


	switch (mMode)
	{
	case ECSKitDebug_AutoPilotMode::Record:
		mActiveMode = NewObject<UCSKitDebug_AutoPilotModeBase>(this, UCSKitDebug_AutoPilotModeRecord::StaticClass());
		break;
	default:
		break;
	}

	if (mActiveMode)
	{
		mActiveMode->SetParent(this);
	}

	OnBeginMode();
}

/**
 * @brief 通常の入力処理の無効化
 */
void UCSKitDebug_AutoPilotComponent::SetIgnoreDefaultInput(const bool InIgnore)
{
	if (mbIgnoreInput != InIgnore)
	{
		mbIgnoreInput = InIgnore;
		if (UGameViewportClient* GameViewportClient = GetWorld()->GetGameViewport())
		{
			GameViewportClient->SetIgnoreInput(mbIgnoreInput);
		}
	}
}

/* ------------------------------------------------------------
   !再生
------------------------------------------------------------ */
/**
 * @brief 
 */
void UCSKitDebug_AutoPilotComponent::RequestPlayInputRecord(const FString& InFileName)
{
	SetMode(ECSKitDebug_AutoPilotMode::Record);
	UCSKitDebug_AutoPilotModeRecord* ModeRecord = Cast<UCSKitDebug_AutoPilotModeRecord>(mActiveMode);
	ModeRecord->RequestPlayInputRecord(InFileName);
}

/**
 * @brief 再生終わったかどうか
 */
bool UCSKitDebug_AutoPilotComponent::IsFinishPlayRecord() const
{
	if (const UCSKitDebug_AutoPilotModeRecord* ModeRecord = Cast<UCSKitDebug_AutoPilotModeRecord>(mActiveMode))
	{
		return ModeRecord->IsFinishPlay();
	}
	return false;
}

/**
 * @brief コマンド収録開始
 */
void UCSKitDebug_AutoPilotComponent::RequestBeginRecord(const FString& InFileName)
{
	SetMode(ECSKitDebug_AutoPilotMode::Record);
	UCSKitDebug_AutoPilotModeRecord* ModeRecord = Cast<UCSKitDebug_AutoPilotModeRecord>(mActiveMode);
	ModeRecord->RequestBeginRecord(InFileName);
}

/**
 * @brief コマンド収録終了
 */
void UCSKitDebug_AutoPilotComponent::RequestEndRecord()
{
	SetMode(ECSKitDebug_AutoPilotMode::Record);
	UCSKitDebug_AutoPilotModeRecord* ModeRecord = Cast<UCSKitDebug_AutoPilotModeRecord>(mActiveMode);
	ModeRecord->RequestEndRecord();
}

/**
 * @brief コマンド収録のIdle状態
 */
void UCSKitDebug_AutoPilotComponent::RequestIdleRecord()
{
	SetMode(ECSKitDebug_AutoPilotMode::Record);
	UCSKitDebug_AutoPilotModeRecord* ModeRecord = Cast<UCSKitDebug_AutoPilotModeRecord>(mActiveMode);
	ModeRecord->RequestIdle();
}

/**
 * @brief	Draw登録のon/off
 */
void	UCSKitDebug_AutoPilotComponent::RequestDebugDraw(const bool bInActive)
{
	if (bInActive)
	{
		if (!mDebugDrawHandle.IsValid())
		{
			auto DebugDrawDelegate = FDebugDrawDelegate::CreateUObject(this, &UCSKitDebug_AutoPilotComponent::DebugDraw);
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
 * @brief	Draw
 */
void	UCSKitDebug_AutoPilotComponent::DebugDraw(UCanvas* InCanvas, APlayerController* InPlayerController) const
{
	if (mActiveMode)
	{
		mActiveMode->DebugDraw(InCanvas);
	}
}

/**
 * @brief	モード開始時
 */
void UCSKitDebug_AutoPilotComponent::OnBeginMode()
{
	switch (mMode)
	{
	case ECSKitDebug_AutoPilotMode::Record:
		SetFixFrameRate(true);
		break;
	case ECSKitDebug_AutoPilotMode::Random:
		break;
	case ECSKitDebug_AutoPilotMode::Command:
		SetIgnoreDefaultInput(true);
		break;
	default:
		break;
	}
}

/**
 * @brief	モード終了時
 */
void UCSKitDebug_AutoPilotComponent::OnEndMode()
{
	switch (mMode)
	{
	case ECSKitDebug_AutoPilotMode::Record:
		SetFixFrameRate(false);
		break;
	case ECSKitDebug_AutoPilotMode::Random:
		break;
	case ECSKitDebug_AutoPilotMode::Command:
		SetIgnoreDefaultInput(false);
		break;
	default:
		break;
	}
}

/**
 * @brief	フレームレート固定
 */
void UCSKitDebug_AutoPilotComponent::SetFixFrameRate(bool InFix)
{
#if 1//4.21で固定方法が変わった？
	if (GEngine)
	{
		GEngine->bUseFixedFrameRate = InFix;
		GEngine->FixedFrameRate = 30.f;
	}
#else
	FApp::SetUseFixedTimeStep(true);
	FApp::SetFixedDeltaTime(0.008888);
#endif
}


#if 0

/**
 * @brief	ランダムインプット開始
 */
void UCSKitDebug_AutoPilotComponent::RequestBeginRandom()
{
	SetMode(ECSKitDebug_AutoPilotMode::Random);
	UAutoPilotModeRandom* ModeRandom = Cast<UAutoPilotModeRandom>(mActiveMode);
	ModeRandom->RequestBegin();
	SetIgnoreDefaultInput(true);

	UDebugCommandManager* DebugCommandManager = GetDebugCommandManager();
	if (DebugCommandManager)
	{
		DebugCommandManager->SetSleep(true);
	}
}

/**
 * @brief	ランダムインプット終了
 */
void UCSKitDebug_AutoPilotComponent::RequestEndRandom()
{
	SetMode(ECSKitDebug_AutoPilotMode::Random);
	UAutoPilotModeRandom* ModeRandom = Cast<UAutoPilotModeRandom>(mActiveMode);
	ModeRandom->RequestEnd();
	SetIgnoreDefaultInput(false);

	UDebugCommandManager* DebugCommandManager = GetDebugCommandManager();
	if (DebugCommandManager)
	{
		DebugCommandManager->SetSleep(false);
	}
}

/**
 * @brief	AutoPlay開始
 */
void UCSKitDebug_AutoPilotComponent::RequestBeginAutoPlay()
{
	_LOG(LogAI, Log, TEXT("BeginAutoPlay"));

	SetMode(ECSKitDebug_AutoPilotMode::AutoPlay);
	UAutoPilotModeAutoPlay* ModeAutoPlay = Cast<UAutoPilotModeAutoPlay>(mActiveMode);
	ModeAutoPlay->RequestBegin();
	SetIgnoreDefaultInput(true);

	UDebugCommandManager* DebugCommandManager = GetDebugCommandManager();
	if (DebugCommandManager)
	{
		DebugCommandManager->SetSleep(true);
	}
}

/**
 * @brief	AutoPlay終了
 */
void UCSKitDebug_AutoPilotComponent::RequestEndAutoPlay()
{
	if (UAutoPilotModeAutoPlay* ModeAutoPlay = Cast<UAutoPilotModeAutoPlay>(mActiveMode))
	{
		ModeAutoPlay->RequestEnd();
	}
	SetIgnoreDefaultInput(false);

	SetMode(ECSKitDebug_AutoPilotMode::Invalid);

	UDebugCommandManager* DebugCommandManager = GetDebugCommandManager();
	if (DebugCommandManager)
	{
		DebugCommandManager->SetSleep(false);
	}
}

/**
 * @brief	SemiAutoPlay開始
 */
void UCSKitDebug_AutoPilotComponent::RequestBeginSemiAutoPlay()
{
	_LOG(LogAI, Log, TEXT("BeginSemiAutoPlay"));

	SetMode(ECSKitDebug_AutoPilotMode::SemiAutoPlay);
	UAutoPilotModeSemiAutoPlay* ModeSemiAutoPlay = Cast<UAutoPilotModeSemiAutoPlay>(mActiveMode);
	ModeSemiAutoPlay->RequestBegin();
}

/**
 * @brief	SemiAutoPlay終了
 */
void UCSKitDebug_AutoPilotComponent::RequestEndSemiAutoPlay()
{
	if (UAutoPilotModeSemiAutoPlay* ModeSemiAutoPlay = Cast<UAutoPilotModeSemiAutoPlay>(mActiveMode))
	{
		ModeSemiAutoPlay->RequestEnd();
	}

	SetMode(ECSKitDebug_AutoPilotMode::Invalid);
}

/**
 * @brief	UI側でキーを押した時
 */
void UCSKitDebug_AutoPilotComponent::OnPushUIKey(const FKey& InKey)
{
	if (mActiveMode)
	{
		mActiveMode->OnPushUIKey(InKey);
	}
}

/**
 * @brief	KeyId
 */
uint32 UCSKitDebug_AutoPilotComponent::GetKeyId(const FKey* InKey) const
{
	const ECSKitDebug_AutoPilotKey* KeyId = mKeyMap.Find(InKey);
	if (KeyId)
	{
		return static_cast<uint32>(*KeyId);
	}
	return 0;
}
uint32 UCSKitDebug_AutoPilotComponent::GetKeyId(const FKey& InKey) const
{
	//mKeyMapでポインタ扱うのやめるべきな気も
	for (const auto& Element : mKeyMap)
	{
		if (*Element.Key == InKey)
		{
			return (uint32)Element.Value;
		}
	}
	return 0;
}
/**
 * @brief	Key
 */
const FKey* UCSKitDebug_AutoPilotComponent::GetKey(ECSKitDebug_AutoPilotKey InKey) const
{
	if (InKey == ECSKitDebug_AutoPilotKey::Invalid
		|| InKey == ECSKitDebug_AutoPilotKey::Num)
	{
		return nullptr;
	}
	const FKey* const* KeyPtr = mKeyMap.FindKey(InKey);
	if (KeyPtr == nullptr)
	{
		return nullptr;
	}

	return *KeyPtr;
}

/**
 * @brief	パッド入力コマンドリクエスト
 */
int32 UCSKitDebug_AutoPilotComponent::RequestCommandInput(ECSKitDebug_AutoPilotKey InKey, float InInputTime, float InAxisValue)
{
	SetMode(ECSKitDebug_AutoPilotMode::Command);
	UAutoPilotModeCommand* ModeCommand = Cast<UAutoPilotModeCommand>(mActiveMode);
	return ModeCommand->RequestCommandInput(InKey, InInputTime, InAxisValue);
}

/**
 * @brief	移動コマンドリクエスト
 */
int32 UCSKitDebug_AutoPilotComponent::RequestCommandMove(const FVector& InGoalPos)
{
	SetMode(ECSKitDebug_AutoPilotMode::Command);
	UAutoPilotModeCommand* ModeCommand = Cast<UAutoPilotModeCommand>(mActiveMode);
	return ModeCommand->RequestCommandMove(InGoalPos);
}
/**
 * @brief	移動終了
 */
int32 UCSKitDebug_AutoPilotComponent::RequestCommandMoveStop()
{
	UAutoPilotModeCommand* ModeCommand = Cast<UAutoPilotModeCommand>(mActiveMode);
	if (ModeCommand)
	{
		return ModeCommand->RequestCommandMoveStop();
	}
	return 0;
}
/**
 * @brief	攻撃コマンドリクエスト
 */
int32 UCSKitDebug_AutoPilotComponent::RequestCommandAttack()
{
	return 0;
}
/**
 * @brief	コマンド終了かどうか
 */
bool UCSKitDebug_AutoPilotComponent::IsFinishCommand(int32 InCommandId)
{
	UAutoPilotModeCommand* ModeCommand = Cast<UAutoPilotModeCommand>(mActiveMode);
	if (ModeCommand)
	{
		return ModeCommand->IsFinishCommand(InCommandId);
	}
	return false;
}
/**
 * @brief	最後にリクエストしたコマンドが終了かどうか
 */
bool UCSKitDebug_AutoPilotComponent::IsFinishLastCommand()
{
	UAutoPilotModeCommand* ModeCommand = Cast<UAutoPilotModeCommand>(mActiveMode);
	if (ModeCommand)
	{
		return ModeCommand->IsFinishLastCommand();
	}
	return false;
}
/**
 * @brief	全コマンド終了かどうか
 */
bool UCSKitDebug_AutoPilotComponent::IsFinishAllCommand()
{
	UAutoPilotModeCommand* ModeCommand = Cast<UAutoPilotModeCommand>(mActiveMode);
	if (ModeCommand)
	{
		return !ModeCommand->IsOwnCommand();
	}
	return true;
}
/**
 * @brief	CommandScriptリクエスト
 */
bool UCSKitDebug_AutoPilotComponent::RequestCommandScript(FString InPath)
{
	SetMode(ECSKitDebug_AutoPilotMode::Command);
	UAutoPilotModeCommand* ModeCommand = Cast<UAutoPilotModeCommand>(mActiveMode);
	ModeCommand->RequestCommandScript(InPath);
	return true;
}
/**
 * @brief	Moveモードの移動先取得
 */
FVector UCSKitDebug_AutoPilotComponent::GetCommandMoveGoalPos() const
{
	UAutoPilotModeCommand* ModeCommand = Cast<UAutoPilotModeCommand>(mActiveMode);
	if (ModeCommand)
	{
		return ModeCommand->GetGoalPos();
	}
	return FVector::ZeroVector;
}

/**
 * @brief	デバッグ選択時のデバッグ情報
 */
void UCSKitDebug_AutoPilotComponent::DebugUpdateSelectInfo(float DeltaTime)
{
	APlayerController* PlayerControler = GetPlayerController();
	ACharacter* Player = Cast<ACharacter>(PlayerControler->GetPawn());
	if (Player == nullptr)
	{
		return;
	}
	if (Player->GetDebugSelectComponent() == nullptr)
	{
		return;
	}
	UDebugSelectComponent& DebugSelectComponent = *Player->GetDebugSelectComponent();

	if (mActiveMode)
	{
		mActiveMode->DebugUpdateSelectInfo(DebugSelectComponent, DeltaTime);
	}
}

/**
 * @brief	デバッグ選択時のデバッグ表示
 */
void UCSKitDebug_AutoPilotComponent::DebugDrawSelected(UCanvas* Canvas, const APlayerController* InPlayerController)
{
	if (mActiveMode)
	{
		mActiveMode->DebugDrawSelected(Canvas, InPlayerController);
	}
}
#endif