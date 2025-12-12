// Copyright 2020 megasuraman
/**
 * @file CSKitDebug_ShortcutCommand.cpp
 * @brief デバッグ機能をパッドやキーから呼び出す機能
 * @author megasuraman
 * @date 2020/5/25
 */

#include "CSKitDebug_ShortcutCommand.h"
#include "CSKitDebug_Subsystem.h"
#include "CSKitDebug_Config.h"
#include "CSKitDebug_DebugCameraController.h"
#include "DebugMenu/CSKitDebug_DebugMenuManager.h"

#include "GameFramework/PlayerInput.h"
#include "InputCoreTypes.h"
#include "Engine/DebugCameraController.h"
#include "GameFramework/CheatManager.h"

UCSKitDebug_ShortcutCommand::UCSKitDebug_ShortcutCommand()
#if USE_CSKIT_DEBUG
    : mbDebugStopMode(false)
	, mbDebugCameraMode(false)
	, mbDebugStop(false)
	, mbDebugStep(false)
	, mbDebugStepRepeat(false)
	, mbStopMotionBlur(false)
	, mbRequestReleaseDebugStopAfterMenu(false)
#endif//USE_CSKIT_DEBUG
{}

#if USE_CSKIT_DEBUG
/**
 * @brief	Init
 */
void	UCSKitDebug_ShortcutCommand::Init() const
{
	UCSKitDebug_DebugMenuManager* DebugMenuManager = UCSKitDebug_DebugMenuManager::sGet(this);

	const FString BaseDebugMenuPath(TEXT("CSKitDebug/DebugCommand"));
	DebugMenuManager->AddNode_Bool(BaseDebugMenuPath, FString(TEXT("DebugStopOnDebugMenu")), false);
}
/**
 * @brief	Tick
 */
bool	UCSKitDebug_ShortcutCommand::DebugTick(float InDeltaSecond)
{
    APlayerController* PlayerController = FindPlayerController();
    if (PlayerController == nullptr)
    {
        return true;
    }

	const UPlayerInput* PlayerInput = PlayerController->PlayerInput;
	if (PlayerInput == nullptr)
	{
		return true;
	}

	UCSKitDebug_Subsystem* CSKitDebug = Cast<UCSKitDebug_Subsystem>(GetOuter());
	UCSKitDebug_DebugMenuManager* DebugMenuManager = CSKitDebug->GetDebugMenuManager();
	mbRequestDebugStopOnDebugMenu = DebugMenuManager->GetNodeValue_Bool(FString(TEXT("CSKitDebug/DebugCommand/DebugStopOnDebugMenu")));
    if (!DebugMenuManager->IsActive())
	{
		CheckDebugStep(PlayerController, InDeltaSecond);
		CheckDebugCameraMode(PlayerController);
        CheckSecretCommand(PlayerController);
    }

	const UCSKitDebug_Config* CSKitDebugConfig = GetDefault<UCSKitDebug_Config>();
    //デバッグコマンド入力準備有効
    if (CSKitDebugConfig->mDebugCommand_ReadyKey.IsPressed(*PlayerInput))
	{
        //デバッグメニューon/off
        if (CSKitDebugConfig->mDebugCommand_DebugMenuKey.IsJustPressed(*PlayerInput))
        {
            SwitchDebugMenuActive(PlayerController);
            return true;
        }
		//デバッグストップon/off
		else if (CSKitDebugConfig->mDebugCommand_DebugStopKey.IsJustPressed(*PlayerInput))
		{
            mbDebugStopMode = !mbDebugStopMode;
			SetDebugStop(mbDebugStopMode, PlayerController);
			SetStopMotionBlur(mbDebugCameraMode, PlayerController);
			return true;
		}
		//デバッグカメラon/off
		else if (CSKitDebugConfig->mDebugCommand_DebugCameraKey.IsJustPressed(*PlayerInput))
		{
			if (UCheatManager* CheatManager = PlayerController->CheatManager)
			{
				CheatManager->DebugCameraControllerClass = ACSKitDebug_DebugCameraController::StaticClass();
			}
			PlayerController->ConsoleCommand(FString(TEXT("ToggleDebugCamera")));
			return true;
		}
        else
        {//PersonalShortcutCommand
	        for (const auto& MapElement : mPersonalShortcutCommand)
	        {
	        	if (MapElement.Value.IsJustPressed(*PlayerInput))
	        	{
	        		PlayerController->ConsoleCommand(MapElement.Key);
	        		return true;
	        	}
	        }
        }
    }

    return true;
}

/**
 * @brief	Draw
 */
void	UCSKitDebug_ShortcutCommand::DebugDraw(UCanvas* InCanvas)
{
}

/**
 * @brief	個人用ShortcutCommand設定
 */
void UCSKitDebug_ShortcutCommand::AddPersonalShortcutCommand(
	const FString& InConsoleCommand,
	const FCSKitDebugKey& InKey)
{
	mPersonalShortcutCommand.Add(InConsoleCommand, InKey);
}
void UCSKitDebug_ShortcutCommand::AddPersonalShortcutCommand(const TMap<FString, FCSKitDebugKey>& InCommand)
{
	mPersonalShortcutCommand.Append(InCommand);
}

/**
 * @brief	有効なPlayerControllerを探す
 */
APlayerController*	UCSKitDebug_ShortcutCommand::FindPlayerController() const
{
    for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
    {
        if (APlayerController* PlayerController = Iterator->Get())
        {
            if (PlayerController->Player)
            {
                return PlayerController;
            }
        }
    }
    return nullptr;
}

/**
 * @brief	DebugStep処理
 */
void	UCSKitDebug_ShortcutCommand::CheckDebugStep(APlayerController* InPlayerController, float InDeltaSecond)
{
    if (!mbDebugStopMode)
    {
        return;
    }

    if (mbDebugStep)
    {
        if (!mbDebugStop)
		{
			mDebugStepInterval -= InDeltaSecond;
			if (mDebugStepInterval < 0.f)
			{
                SetDebugStop(true, InPlayerController);
                mbDebugStep = false;
			}
        }
    }

	UPlayerInput* PlayerInput = InPlayerController->PlayerInput;
	const UCSKitDebug_Config* CSKitDebugConfig = GetDefault<UCSKitDebug_Config>();

    if (CSKitDebugConfig->mDebugCommand_DebugStopKey.IsPressed(*PlayerInput))
    {
        if (!mbDebugStepRepeat)
		{
			mDebugStepRepeatBeginTimer += InDeltaSecond;
            if (mDebugStepRepeatBeginTimer > mDebugStepRepeatBeginSec)
            {
                mbDebugStepRepeat = true;
                mDebugStepRepeatStopTimer = 0.f;
            }
        }
    }
    else
    {
        mbDebugStepRepeat = false;
    }

    if (mbDebugStepRepeat)
	{
		if (mbDebugStop)
		{
            mDebugStepRepeatStopTimer += InDeltaSecond;
            if (mDebugStepRepeatStopTimer > mDebugStepRepeatStopSec)
			{
                mDebugStepRepeatStopTimer = 0.f;
				mbDebugStep = true;
				mDebugStepInterval = 0.f;
				SetDebugStop(false, InPlayerController);
            }
		}
    }
    else if(CSKitDebugConfig->mDebugCommand_DebugStopKey.IsJustPressed(*PlayerInput))
    {
        if (mbDebugStop)
		{
			mbDebugStep = true;
			mDebugStepInterval = 0.f;
			SetDebugStop(false, InPlayerController);
        }
    }
}

/**
 * @brief	実際にDebugCamera状態になったかのチェックとon/off時処理
 */
void	UCSKitDebug_ShortcutCommand::CheckDebugCameraMode(APlayerController* InPlayerController)
{
    ADebugCameraController* DebugCameraController = Cast<ADebugCameraController>(InPlayerController);
    const bool bDebugCamera = (DebugCameraController != nullptr);
    if (mbDebugCameraMode == bDebugCamera)
    {
        return;
    }

    mbDebugCameraMode = bDebugCamera;
    if (mbDebugCameraMode)
	{
        if (DebugCameraController->IsDisplayEnabled())
        {
            DebugCameraController->ToggleDisplay();
		}
		SetStopMotionBlur(mbDebugStopMode, InPlayerController);
    }
    else
	{
		SetStopMotionBlur(false, InPlayerController);
    }
}

/**
 * @brief	SecretCommandチェック
 */
void UCSKitDebug_ShortcutCommand::CheckSecretCommand(APlayerController* InPlayerController)
{
	UPlayerInput* PlayerInput = InPlayerController->PlayerInput;
    const float NowTime = GetWorld()->TimeSeconds;
    const bool bInputAnyKey = PlayerInput->WasJustPressed(EKeys::Gamepad_FaceButton_Bottom)
		|| PlayerInput->WasJustPressed(EKeys::Gamepad_FaceButton_Right)
		|| PlayerInput->WasJustPressed(EKeys::Gamepad_FaceButton_Left)
		|| PlayerInput->WasJustPressed(EKeys::Gamepad_FaceButton_Top)
		|| PlayerInput->WasJustPressed(EKeys::Gamepad_LeftShoulder)
		|| PlayerInput->WasJustPressed(EKeys::Gamepad_RightShoulder)
		|| PlayerInput->WasJustPressed(EKeys::Gamepad_LeftTrigger)
		|| PlayerInput->WasJustPressed(EKeys::Gamepad_RightTrigger)
		|| PlayerInput->WasJustPressed(EKeys::Gamepad_DPad_Up)
		|| PlayerInput->WasJustPressed(EKeys::Gamepad_DPad_Down)
		|| PlayerInput->WasJustPressed(EKeys::Gamepad_DPad_Right)
		|| PlayerInput->WasJustPressed(EKeys::Gamepad_DPad_Left);

	const UCSKitDebug_Config* CSKitDebugConfig = GetDefault<UCSKitDebug_Config>();
    const TMap<FString, FCSKitDebugSecretCommand>& SecretCommand = CSKitDebugConfig->mDebugSecretCommand;
    for (const auto& Element : SecretCommand)
    {
		bool bPossibleToRunCommand = false;
		bool bNeedDeleteLog = false;
        const TArray<FKey>& KeyList = Element.Value.mKeyList;
        if (FSecretCommandLog* SecretCommandLog = mSecretCommandLog.Find(Element.Key))
		{
			if (PlayerInput->WasJustPressed(KeyList[SecretCommandLog->mNextIndex]))
			{
                SecretCommandLog->mNextIndex += 1;
                SecretCommandLog->mInputTime = NowTime;
				if (SecretCommandLog->mNextIndex >= KeyList.Num())
				{
					bPossibleToRunCommand = true;
				}
            }
            else if(bInputAnyKey
                    || NowTime - SecretCommandLog->mInputTime > 3.f )
            {
                bNeedDeleteLog = true;
            }
        }
        else if (KeyList.Num() > 0)
        {
            if (PlayerInput->WasJustPressed(KeyList[0]))
            {
                FSecretCommandLog& NewSecretCommandLog = mSecretCommandLog.FindOrAdd(Element.Key);
                NewSecretCommandLog.mNextIndex = 1;
                NewSecretCommandLog.mInputTime = GetWorld()->TimeSeconds;
                if (NewSecretCommandLog.mNextIndex >= KeyList.Num())
                {
                    bPossibleToRunCommand = true;
                }
            }
        }

        if (bPossibleToRunCommand)
		{
			InPlayerController->ConsoleCommand(Element.Key);
            bNeedDeleteLog = true;
        }

        if (bNeedDeleteLog)
        {
            mSecretCommandLog.Remove(Element.Key);
        }
    }
}

/**
 * @brief	DebugMenuのon/off切り替え
 */
void	UCSKitDebug_ShortcutCommand::SwitchDebugMenuActive(APlayerController* InPlayerController)
{
    UCSKitDebug_Subsystem* CSKitDebug = Cast<UCSKitDebug_Subsystem>(GetOuter());
    UCSKitDebug_DebugMenuManager* DebugMenuManager = CSKitDebug->GetDebugMenuManager();
    const bool bOldActiveMenu = DebugMenuManager->IsActive();
    // On -> Off
    if (bOldActiveMenu)
    {
        DebugMenuManager->SetActive(false);
        if (mbRequestReleaseDebugStopAfterMenu)
        {
            SetDebugStop(false, InPlayerController);
            SetStopMotionBlur(false, InPlayerController);
        }
        mbRequestReleaseDebugStopAfterMenu = false;
    }
    // Off -> On
    else
    {
        DebugMenuManager->SetActive(true);
        if (mbRequestDebugStopOnDebugMenu
            && !mbDebugStopMode)
        {
            mbRequestReleaseDebugStopAfterMenu = true;
            SetDebugStop(true, InPlayerController);
            SetStopMotionBlur(true, InPlayerController);
        }
    }
}

/**
 * @brief	DebugStopのon/off
 */
void	UCSKitDebug_ShortcutCommand::SetDebugStop(bool bInStop, APlayerController* InPlayerController)
{
    if (mbDebugStop == bInStop)
    {
        return;
    }

	mbDebugStop = bInStop;
    InPlayerController->ConsoleCommand(FString(TEXT("Pause")));
    if (mbDebugStop)
    {
        mDebugStepRepeatBeginTimer = 0.f;
    }
    else
    {

    }
}

/**
 * @brief	MotionBlur停止のon/off
 */
void	UCSKitDebug_ShortcutCommand::SetStopMotionBlur(bool bInStop, APlayerController* InPlayerController)
{
    if (mbStopMotionBlur == bInStop)
    {
        return;
    }

    mbStopMotionBlur = bInStop;
    InPlayerController->ConsoleCommand(FString(TEXT("Show MotionBlur")));
}

#endif//USE_CSKIT_DEBUG