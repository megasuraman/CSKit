// Copyright 2020 megasuraman
/**
 * @file CSKitDebug_ShortcutCommand.h
 * @brief デバッグ機能をパッドやキーから呼び出す機能
 * @author megasuraman
 * @date 2020/5/25
 */

#pragma once

#include "CoreMinimal.h"
#include "CSKitDebug_ShortcutCommand.generated.h"


class APlayerController;

/**
 * 
 */
UCLASS()
class CSKITDEBUG_API UCSKitDebug_ShortcutCommand : public UObject
{
	GENERATED_BODY()

	UCSKitDebug_ShortcutCommand();

#if USE_CSKIT_DEBUG
public:
	void	Init() const;
	bool	DebugTick(float InDeltaSecond);
	void	DebugDraw(class UCanvas* InCanvas);

protected:
	struct FSecretCommandLog
	{
		int32 mNextIndex = 0;
		float mInputTime = 0.f;
	};

	APlayerController* FindPlayerController() const;

	void	CheckDebugStep(APlayerController* InPlayerController, float InDeltaSecond);
	void	CheckDebugCameraMode(APlayerController* InPlayerController);
	void	CheckSecretCommand(APlayerController* InPlayerController);

	void	SwitchDebugMenuActive(APlayerController* InPlayerController);
	void	SetDebugStop(bool bInStop, APlayerController* InPlayerController);
	void	SetStopMotionBlur(bool bInStop, APlayerController* InPlayerController);

private:
	TMap<FString, FSecretCommandLog> mSecretCommandLog;
	float	mDebugStepRepeatBeginSec = 0.3f;//DebugStepRepeat発動までの押しっぱなし時間
	float	mDebugStepRepeatBeginTimer = 0.f;//DebugStepRepeat発動までの押しっぱなし計測時間
	float	mDebugStepInterval = 0.f;//DebugStepでPause解除後に再度解除するまでの時間
	float	mDebugStepRepeatStopSec = 0.05f;//DebugStepRepeat時の停止時間
	float	mDebugStepRepeatStopTimer = 0.f;//DebugStepRepeat時の停止計測時間
	bool	mbRequestDebugStopOnDebugMenu = false;//DebugMenuと同時にDebugStop
	uint8	mbDebugStopMode : 1;//DebugStopModeかどうか
	uint8	mbDebugCameraMode : 1;//DebugCameraModeかどうか
	uint8	mbDebugStop : 1;//実際にPause中かどうか
	uint8	mbDebugStep : 1;//一瞬DebugStop解除して再度DebugStopしたいかどうか
	uint8	mbDebugStepRepeat : 1;//DebugStepを繰り返したいかどうか
	uint8	mbStopMotionBlur : 1;//DebugStopとDebugCameraを併用時にモーションブラー(カメラブラー)が邪魔なのでon/offように
	uint8	mbRequestReleaseDebugStopAfterMenu : 1;//DebugMenu終了時にDebugStop解除

#endif//USE_CSKIT_DEBUG
};
