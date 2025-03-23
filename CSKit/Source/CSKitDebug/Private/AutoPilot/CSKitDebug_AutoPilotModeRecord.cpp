// Copyright 2021 megasuraman
/**
 * @file CSKitDebug_AutoPilotModeRecord.cpp
 * @brief 自動入力 パッド入力を保存,再生するモード
 * @author megasuraman
 * @date 2021/12/28
 */
#include "AutoPilot/CSKitDebug_AutoPilotModeRecord.h"
#include "AutoPilot/CSKitDebug_AutoPilotComponent.h"

#include "Misc/FileHelper.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"

/**
 * @brief	PlayerInputの処理前
 */
void	UCSKitDebug_AutoPilotModeRecord::PreProcessInput(float DeltaTime)
{
 	//ResetDebugDrawPadInfo();
 
 	if (mMode == ECommandMode::PlayInputRecord)
 	{
 		UpdatePlayInputRecord(DeltaTime);
 	}
}

/**
 * @brief	PlayerInputの処理後
 */
void	UCSKitDebug_AutoPilotModeRecord::PostProcessInput(float DeltaTime)
{
	UpdateInputRecord(DeltaTime);
}

/**
 * @brief	
 */
void	UCSKitDebug_AutoPilotModeRecord::DebugDraw(class UCanvas* InCanvas)
{
	DebugDrawPad(InCanvas);
}

/**
 * @brief	保存や再生等のモード切替
 */
void	UCSKitDebug_AutoPilotModeRecord::SetMode(ECommandMode InMode)
{
	if (mMode == InMode)
	{
		return;
	}

	if (mMode == ECommandMode::PlayInputRecord)
	{
		GetParent()->SetIgnoreDefaultInput(false);
	}

	mMode = InMode;
	mPlayFrame = 0;

	if (mMode == ECommandMode::PlayInputRecord)
	{
		GetParent()->SetIgnoreDefaultInput(true);
	}
}

/**
 * @brief	再生
 */
void UCSKitDebug_AutoPilotModeRecord::RequestPlayInputRecord(const FString& InFileName)
{
	SetMode(ECommandMode::PlayInputRecord);
	mFileName = InFileName;
	mPlayFrame = 0;
	mPlayInputRecordState = EPlayInputRecordState::Load;
}
/**
 * @brief	
 */
bool	UCSKitDebug_AutoPilotModeRecord::IsFinishPlay() const
{
	if (mMode == ECommandMode::PlayInputRecord
		&& mPlayInputRecordState == EPlayInputRecordState::Finish
		)
	{
		return true;
	}
	return false;
}
/**
 * @brief	コマンド収録開始リクエスト
 */
void UCSKitDebug_AutoPilotModeRecord::RequestBeginRecord(const FString& InFileName)
{
	SetMode(ECommandMode::BeginRecord);
	mFileName = InFileName;
	mPlayFrame = 0;
}
/**
 * @brief	コマンド収録終了リクエスト
 */
void UCSKitDebug_AutoPilotModeRecord::RequestEndRecord()
{
	SetMode(ECommandMode::EndRecord);
}
/**
 * @brief	停止リクエスト
 */
void UCSKitDebug_AutoPilotModeRecord::RequestIdle()
{
	SetMode(ECommandMode::Idle);
}

/**
 * @brief	出力ファイルパス取得
 */
FString	UCSKitDebug_AutoPilotModeRecord::GetFilePath() const
{
	FString SavedPath = FPaths::ProjectSavedDir();
	SavedPath += "CSKitDebug/AutoPilot/" + mFileName + ".json";
	return SavedPath;
}

/**
 * @brief	記録した入力を再生する
 */
bool UCSKitDebug_AutoPilotModeRecord::UpdatePlayInputRecord(float DeltaTime)
{
	APlayerController* PlayerController = GetPlayerController();
	//PlayerController->FlushPressedKeys();
	PlayerController->InputAxis(EKeys::Gamepad_LeftX, 0.f, DeltaTime, 1, true);
	PlayerController->InputAxis(EKeys::Gamepad_LeftY, 0.f, DeltaTime, 1, true);
	PlayerController->InputAxis(EKeys::Gamepad_RightX, 0.f, DeltaTime, 1, true);
	PlayerController->InputAxis(EKeys::Gamepad_RightY, 0.f, DeltaTime, 1, true);

	switch (mPlayInputRecordState)
	{
	case EPlayInputRecordState::Load:
		if (LoadInputRecordFile(DeltaTime))
		{
			mPlayInputRecordState = EPlayInputRecordState::WaitReady;
		}
		break;
	case EPlayInputRecordState::WaitReady:
		if (!WaitPlayInputRecordFile(DeltaTime))
		{
			mPlayInputRecordState = EPlayInputRecordState::Play;
		}
		break;
	case EPlayInputRecordState::Play:
		if (!PlayInputRecordFile(DeltaTime))
		{
			mPlayInputRecordState = EPlayInputRecordState::Finish;
		}
		break;
	case EPlayInputRecordState::Finish:
		mPlayFrame = 0;
		mWarpInterval = 1.f;
		break;
	default:
		break;
	}

	return true;
}

/**
 * @brief	記録した入力をロード
 */
bool UCSKitDebug_AutoPilotModeRecord::LoadInputRecordFile(float DeltaTime)
{
	mPlayFrame = 0;
	mCommand.mList.Empty();
	mWarpInterval = 1.f;

	const FString SavedPath = GetFilePath();
	FString JsonString;
	if (!FFileHelper::LoadFileToString(JsonString, *SavedPath))
	{
		return false;
	}

	TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(JsonString);
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());

	if (FJsonSerializer::Deserialize(JsonReader, JsonObject)
		&& JsonObject.IsValid()
		)
	{
		mCommand.FromJson(JsonObject);
	}
	else
	{
		SetMode(ECommandMode::Invalid);
		return false;
	}

	if (mCommand.mList.Num() == 0)
	{
		SetMode(ECommandMode::Invalid);
		return false;
	}

	return true;
}

/**
 * @brief	記録した入力を再生するまでの待ち
 */
bool UCSKitDebug_AutoPilotModeRecord::WaitPlayInputRecordFile(float DeltaTime)
{
	APlayerController* PlayerController = GetPlayerController();
	ACharacter* Player = Cast<ACharacter>(PlayerController->GetPawn());
	if (Player)
	{
		const FVector StartPos(mCommand.mStartPosX, mCommand.mStartPosY, mCommand.mStartPosZ);
		const FRotator StartRot(mCommand.mStartRotatorPitch, mCommand.mStartRotatorYaw, mCommand.mStartRotatorRoll);
		//const FRotator CameraRot(mCommand.mStartCameraRotatorPitch, mCommand.mStartCameraRotatorYaw, mCommand.mStartCameraRotatorRoll);
		//const FRotator ControlRot(mCommand.mStartControllerPitch, StartRot.Yaw, CameraRot.Yaw);
		if (FVector::DistSquared(StartPos, Player->GetActorLocation()) > FMath::Square(0.1f)
			|| !StartRot.Equals(Player->GetActorRotation()))
		{
			Player->SetActorLocationAndRotation(StartPos, StartRot, false, nullptr, ETeleportType::TeleportPhysics);
			//PlayerController->SetControlRotation(CameraRot, ControlRot);
			return true;
		}
	}

	if (mWarpInterval > 0.f)
	{
		mWarpInterval -= DeltaTime;
		return true;
	}

	return false;
}

/**
 * @brief	記録した入力を実際に再生
 */
bool UCSKitDebug_AutoPilotModeRecord::PlayInputRecordFile(float DeltaTime)
{
	//UCSKitDebug_AutoPilotComponent* Parent = GetParent();
	APlayerController* PlayerController = GetPlayerController();

	for (const FCommandNode& InCommand : mCommand.mList)
	{
		const ECSKitDebug_AutoPilotKey KeyId = static_cast<ECSKitDebug_AutoPilotKey>(InCommand.mKeyId);
		const FKey Key = GetKey(KeyId);
		if (mPlayFrame >= InCommand.mBeginFrame
			&& mPlayFrame <= InCommand.mEndFrame)
		{
			if (Key.IsAxis1D())
			//if (Key.IsFloatAxis())
			{
				PlayerController->InputAxis(Key, InCommand.mAxisValue, InCommand.mDeltaTime, 1, true);
				AddDebugDrawPadInfo(FCSKitDebug_AutoPilotDebugDrawPadInfo(KeyId, InCommand.mAxisValue));
			}
			else
			{
				if (mPlayFrame == InCommand.mBeginFrame)
				{
					PlayerController->InputKey(Key, EInputEvent::IE_Pressed, 1.f, true);
				}
				else
				{
					PlayerController->InputKey(Key, EInputEvent::IE_Repeat, 1.f, true);
				}
				AddDebugDrawPadInfo(FCSKitDebug_AutoPilotDebugDrawPadInfo(KeyId, 1.f));
			}
		}
		else if (InCommand.mEndFrame + 1 == mPlayFrame)
		{//前フレームで処理してた
			if (Key.IsAxis1D())
			//if (Key.IsFloatAxis())
			{
				//PlayerController->InputAxis(*Key, InCommand.mAxisValue, InCommand.mDeltaTime, 1, true);
			}
			else
			{
				PlayerController->InputKey(Key, EInputEvent::IE_Released, 1.f, true);
			}
		}
	}

	mPlayFrame += 1;

	return (mPlayFrame <= mCommand.mEndFrame);
}

/**
 * @brief	入力の記録
 */
bool UCSKitDebug_AutoPilotModeRecord::UpdateInputRecord(float DeltaTime)
{
	switch (mMode)
	{
	case ECommandMode::BeginRecord:
		BeginInputRecord(DeltaTime);
		break;
	case ECommandMode::Record:
		RecordingInput(DeltaTime);
		break;
	case ECommandMode::EndRecord:
	{
		//const FString SavedPath = GetFilePath();
		//const bool bSave = FFileHelper::SaveStringToFile(mCommand.ToJson(), *SavedPath);
		SetMode(ECommandMode::Invalid);
		break;
	}
	default:
		break;
	}
	return true;
}
/**
 * @brief	入力の記録を開始
 */
bool UCSKitDebug_AutoPilotModeRecord::BeginInputRecord(float DeltaTime)
{
	APlayerController* PlayerController = GetPlayerController();
	mCommand.mList.Empty();
	mBeforeFrameCommandList.Empty();
	ACharacter* Player = Cast<ACharacter>(PlayerController->GetPawn());
	if (Player)
	{
		mCommand.mStartPosX = Player->GetActorLocation().X;
		mCommand.mStartPosY = Player->GetActorLocation().Y;
		mCommand.mStartPosZ = Player->GetActorLocation().Z;
		mCommand.mStartRotatorPitch = Player->GetActorRotation().Pitch;
		mCommand.mStartRotatorYaw = Player->GetActorRotation().Yaw;
		mCommand.mStartRotatorRoll = Player->GetActorRotation().Roll;

		const FRotator CameraRot = PlayerController->GetControlRotation();
		mCommand.mStartCameraRotatorPitch = CameraRot.Pitch;
		mCommand.mStartCameraRotatorYaw = CameraRot.Yaw;
		mCommand.mStartCameraRotatorRoll = CameraRot.Roll;
		mCommand.mStartControllerPitch = PlayerController->GetControlRotation().Pitch;
	}

	mPlayFrame = 0;
	SetMode(ECommandMode::Record);

	return true;
}
/**
 * @brief	入力の記録
 */
bool UCSKitDebug_AutoPilotModeRecord::RecordingInput(float DeltaTime)
{
	//UCSKitDebug_AutoPilotComponent* Parent = GetParent();
	APlayerController* PlayerController = GetPlayerController();
	CommandPtrList ActiveCommandPtrList;

	//bool bAnyInput = false;
	for (uint32 i = 0; i < static_cast<uint32>(ECSKitDebug_AutoPilotKey::Num); ++i)
	{
		const ECSKitDebug_AutoPilotKey KeyId = static_cast<ECSKitDebug_AutoPilotKey>(i);
		const FKey Key = GetKey(KeyId);
		FCommandNode Temp;
		Temp.mBeginFrame = mPlayFrame;
		Temp.mEndFrame = mPlayFrame;
		Temp.mDeltaTime = DeltaTime;
		Temp.mKeyId = i;
		bool bInput = false;
		if (Key.IsAxis1D())
		//if (Key.IsFloatAxis())
		{
			Temp.mAxisValue = PlayerController->GetInputAnalogKeyState(Key);
			const float* PadDeadZonePtr = GetPadDeadZoneMap().Find(Key);
			if (PadDeadZonePtr == nullptr)
			{
				continue;
			}
			const float PadDeadZone = *PadDeadZonePtr;
			if (Temp.mAxisValue > 0.f)
			{
				Temp.mAxisValue = Temp.mAxisValue * (1.0f - PadDeadZone) + PadDeadZone;
			}
			else if (Temp.mAxisValue < 0.f)
			{
				Temp.mAxisValue = Temp.mAxisValue * (1.0f - PadDeadZone) - PadDeadZone;
			}
			if (FMath::Abs(Temp.mAxisValue) > PadDeadZone)
			{
				bInput = true;
				AddDebugDrawPadInfo(FCSKitDebug_AutoPilotDebugDrawPadInfo(KeyId, Temp.mAxisValue));
			}
		}
		else
		{
			if (PlayerController->IsInputKeyDown(Key))
			{
				bInput = true;
				Temp.mInputEventId = EInputEvent::IE_Pressed;
				AddDebugDrawPadInfo(FCSKitDebug_AutoPilotDebugDrawPadInfo(KeyId, 1.f));
			}
		}

		if (bInput)
		{
			//bAnyInput = true;

			int32 InputIndex = INDEX_NONE;
			for (int32 f = 0; f < mBeforeFrameCommandList.Num(); ++f)
			{
				if (mBeforeFrameCommandList[f]->IsSameInput(Temp))
				{
					InputIndex = mBeforeFrameCommandList[f]->mIndex;
					break;
				}
			}
			if (InputIndex == INDEX_NONE)
			{
				mCommand.mList.Add(Temp);
				InputIndex = mCommand.mList.Num() - 1;
			}
			FCommandNode* InputCommand = &mCommand.mList[InputIndex];
			InputCommand->mEndFrame = mPlayFrame;
			InputCommand->mIndex = InputIndex;
			ActiveCommandPtrList.Add(InputCommand);
		}
	}

	++mPlayFrame;

	mBeforeFrameCommandList = ActiveCommandPtrList;

	mCommand.mEndFrame = mPlayFrame;

	//何故かWidgetの切り替えでPlayerInputが更新されなくなるので
	//if (bInputUI
	//	&& bAnyInput)
	//{
	//	PlayerController->PlayerInput->FlushPressedKeys();
	//}

	return true;
}

#if 0
/**
 * @brief	デバッグ選択時のデバッグ情報
 */
void	UCSKitDebug_AutoPilotModeRecord::DebugUpdateSelectInfo(UDebugSelectComponent& InDebugSelect, float DeltaTime)
{
	InDebugSelect.AddDebugInfo(FString::Printf(TEXT("mPlayFrame : %d"), mPlayFrame));
	InDebugSelect.AddDebugInfo(FString::Printf(TEXT("DeltaTime : %.5f"), DeltaTime));

	APlayerController* PlayerController = GetPlayerController();
	APawn* Player = PlayerController->GetPawn();
	InDebugSelect.AddDebugInfo(FString::Printf(TEXT("Pos : %s"), *Player->GetActorLocation().ToString()));

	DebugUpdateSelectInfo_Player(InDebugSelect);
	if (mMode == ECommandMode::PlayInputRecord
		|| mMode == ECommandMode::Record)
	{
		DebugUpdateSelectInfo_PlayKey(InDebugSelect);
	}
	else
	{
		DebugUpdateSelectInfo_InputKey(InDebugSelect);
	}
}
/**
 * @brief	デバッグ選択時のデバッグ表示
 */
void	UCSKitDebug_AutoPilotModeRecord::DebugDrawSelected(UCanvas* Canvas, const APlayerController* InPlayerController)
{
	DebugDrawPad(Canvas, InPlayerController);
}

/**
 * @brief	UI側でキーを押した時
 *			RecordingInput()だと何故か特定のwidgetの特定のボタンでだけPlayerInputが反応しないので専用に用意
 */
void	UCSKitDebug_AutoPilotModeRecord::OnPushUIKey(const FKey& InKey)
{
	if (mMode != ECommandMode::Record
		|| !IsInputUI())
	{
		return;
	}
	
	UCSKitDebug_AutoPilotComponent* Parent = GetParent();
	FCommandNode Temp;
	Temp.mBeginFrame = mPlayFrame;
	Temp.mEndFrame = mPlayFrame + 5;
	Temp.mAxisValue = 1.f;
	Temp.mDeltaTime = 0.03f;
	Temp.mKeyId = Parent->GetKeyId(InKey);
	Temp.mInputEventId = EInputEvent::IE_Pressed;

	mCommand.mList.Add(Temp);
	const int32 InputIndex = mCommand.mList.Num() - 1;
	FCommandNode* InputCommand = &mCommand.mList[InputIndex];
	InputCommand->mIndex = InputIndex;

	AddDebugDrawPadInfo(FCSKitDebug_AutoPilotDebugDrawPadInfo((ECSKitDebug_AutoPilotMode)Temp.mKeyId, 1.f));

	_LOG(LogSystem, Log, TEXT("OnPushUIKey %d %.2f"), Temp.mKeyId, GetParent()->GetWorld()->GetTimeSeconds());
}


/**
 * @brief	Player情報表示
 */
void	UCSKitDebug_AutoPilotModeRecord::DebugUpdateSelectInfo_Player(UDebugSelectComponent& InDebugSelect)
{
	APlayerController* PlayerController = GetPlayerController();
	APlayer* Player = Cast<APlayer>(PlayerController->GetPawn());
	if (Player == nullptr)
	{
		return;
	}

	InDebugSelect.BeginDebugInfoCategory(FString::Printf(TEXT("PlayerState")));
	{
		const UActionStateComponent_Player* ActionStateComponent = Player->GetActionState();
		InDebugSelect.AddDebugInfo(FString::Printf(TEXT("PlayerActionState : %s"), *PlayerStateToString(ActionStateComponent->GetCurrentPlayerStateType()).ToString()));
		InDebugSelect.AddDebugInfo(FString::Printf(TEXT("Guts : %d"), Player->GetEnableWilson()));
		//DebugSelectComponent->AddDebugInfo(FString::Printf(TEXT("PlayerMoveType : %s"), *DebugGetDrawNamePlayerMoveType()));
		//const UStatusComponent_Player* StatusComponent = Player->GetStatusComponent();
	}
	InDebugSelect.EndDebugInfoCategory();
}
/**
 * @brief	入力してるKey情報表示
 */
void	UCSKitDebug_AutoPilotModeRecord::DebugUpdateSelectInfo_InputKey(UDebugSelectComponent& InDebugSelect)
{
	UCSKitDebug_AutoPilotComponent* Parent = GetParent();
	APlayerController* PlayerController = GetPlayerController();
	InDebugSelect.BeginDebugInfoCategory(FString::Printf(TEXT("Input")));
	{
		for (uint32 i = 0; i < static_cast<uint32>(ECSKitDebug_AutoPilotMode::Num); ++i)
		{
			const FKey* Key = Parent->GetKey(static_cast<ECSKitDebug_AutoPilotMode>(i));
			if (Key == nullptr)
			{
				continue;
			}
			//if (Key.IsAxis1D())
			if (Key.IsFloatAxis())
			{
				const float AxisValue = PlayerController->GetInputAnalogKeyState(*Key);
				if (FMath::Abs(AxisValue) > 0.f)
				{
					InDebugSelect.AddDebugInfo(FString::Printf(TEXT("Key : %s(%.2f)"), *AutoPilotUtil::GetAutoPilotKeyName(static_cast<ECSKitDebug_AutoPilotMode>(i)), AxisValue));
				}
			}
			else
			{
				if (PlayerController->IsInputKeyDown(*Key))
				{
					InDebugSelect.AddDebugInfo(FString::Printf(TEXT("Key : %s"), *AutoPilotUtil::GetAutoPilotKeyName(static_cast<ECSKitDebug_AutoPilotMode>(i))));
				}
			}
		}
	}
	InDebugSelect.EndDebugInfoCategory();
}
/**
 * @brief	再生するKey情報表示
 */
void	UCSKitDebug_AutoPilotModeRecord::DebugUpdateSelectInfo_PlayKey(UDebugSelectComponent& InDebugSelect)
{
	UCSKitDebug_AutoPilotComponent* Parent = GetParent();
	InDebugSelect.BeginDebugInfoCategory(FString::Printf(TEXT("Play(%d)"), (uint8)mPlayInputRecordState));

	uint32 PlayFrame = mPlayFrame;
	if (mMode == ECommandMode::Record
		&& mPlayFrame > 0)
	{
		--PlayFrame;
	}

	for (const FCommandNode& InCommand : mCommand.mList)
	{
		if (PlayFrame >= InCommand.mBeginFrame
			&& PlayFrame <= InCommand.mEndFrame)
		{
			const ECSKitDebug_AutoPilotMode AutoPilotKey = static_cast<ECSKitDebug_AutoPilotMode>(InCommand.mKeyId);
			InDebugSelect.BeginDebugInfoCategory(FString::Printf(TEXT("Key : %s"), *AutoPilotUtil::GetAutoPilotKeyName(AutoPilotKey)));
			{
				InDebugSelect.AddDebugInfo(FString::Printf(TEXT("Frame : %d - %d"), InCommand.mBeginFrame, InCommand.mEndFrame));
				InDebugSelect.AddDebugInfo(FString::Printf(TEXT("DeltaTime : %.2f"), InCommand.mDeltaTime));
				const FKey* Key = Parent->GetKey(AutoPilotKey);
				if (Key)
				{
					//if (Key.IsAxis1D())
					if (Key.IsFloatAxis())
					{
						InDebugSelect.AddDebugInfo(FString::Printf(TEXT("AxisValue : %.2f"), InCommand.mAxisValue));
					}
					else
					{
						InDebugSelect.AddDebugInfo(FString::Printf(TEXT("InputEvent : IE_Pressed")));
					}
				}
			}
			InDebugSelect.EndDebugInfoCategory();
		}
	}

	InDebugSelect.EndDebugInfoCategory();
}
#endif