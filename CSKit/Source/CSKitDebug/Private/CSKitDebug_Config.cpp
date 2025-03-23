// Copyright 2020 megasuraman
/**
 * @file CSKitDebug_Config.cpp
 * @brief CSKitDebugConfig
 * @author megasuraman
 * @date 2020/5/26
 */


#include "CSKitDebug_Config.h"
#include "GameFramework/PlayerInput.h"
#include "InputCoreTypes.h"
#include "DebugMenu/CSKitDebug_DebugMenuManager.h"


/**
 * @brief キーかパッドが押されてるかどうか
 */
bool	FCSKitDebugKey::IsPressed(const UPlayerInput& InInput) const
{
	if (InInput.IsPressed(mKeyboard)
		|| InInput.IsPressed(mPad))
	{
		return true;
	}
	return false;
}

/**
 * @brief キーかパッドが押されたかどうか
 */
bool	FCSKitDebugKey::IsJustPressed(const class UPlayerInput& InInput) const
{
	if (InInput.WasJustPressed(mKeyboard)
		|| InInput.WasJustPressed(mPad))
	{
		return true;
	}
	return false;
}

/**
 * @brief キーかパッドが離されたかどうか
 */
bool FCSKitDebugKey::IsJustReleased(const class UPlayerInput& InInput) const
{
	if (InInput.WasJustReleased(mKeyboard)
		|| InInput.WasJustReleased(mPad))
	{
		return true;
	}
	return false;
}

UCSKitDebug_Config::UCSKitDebug_Config()
{
	mDebugCommand_ReadyKey.mKeyboard = EKeys::LeftAlt;
	mDebugCommand_ReadyKey.mPad = EKeys::Gamepad_Special_Right;

	mDebugCommand_DebugMenuKey.mKeyboard = EKeys::One;
	mDebugCommand_DebugMenuKey.mPad = EKeys::Gamepad_DPad_Up;

	mDebugCommand_DebugStopKey.mKeyboard = EKeys::Two;
	mDebugCommand_DebugStopKey.mPad = EKeys::Gamepad_DPad_Right;

	mDebugCommand_DebugCameraKey.mKeyboard = EKeys::Three;
	mDebugCommand_DebugCameraKey.mPad = EKeys::Gamepad_DPad_Down;

	mDebugSelect_SelectKey.mKeyboard = EKeys::LeftMouseButton;
	mDebugSelect_SelectKey.mPad = EKeys::Gamepad_FaceButton_Right;

	mDebugMenuManagerClass = UCSKitDebug_DebugMenuManager::StaticClass();

	mDebugMenu_SelectKey.mKeyboard = EKeys::Enter;
	mDebugMenu_SelectKey.mPad = EKeys::Gamepad_FaceButton_Bottom;
	mDebugMenu_CancelKey.mKeyboard = EKeys::BackSpace;
	mDebugMenu_CancelKey.mPad = EKeys::Gamepad_FaceButton_Right;

	mDebugMenu_UpKey.mKeyboard = EKeys::Up;
	mDebugMenu_UpKey.mPad = EKeys::Gamepad_DPad_Up;
	mDebugMenu_DownKey.mKeyboard = EKeys::Down;
	mDebugMenu_DownKey.mPad = EKeys::Gamepad_DPad_Down;
	mDebugMenu_RightKey.mKeyboard = EKeys::Right;
	mDebugMenu_RightKey.mPad = EKeys::Gamepad_DPad_Right;
	mDebugMenu_LeftKey.mKeyboard = EKeys::Left;
	mDebugMenu_LeftKey.mPad = EKeys::Gamepad_DPad_Left;
}