// Copyright 2021 megasuraman
/**
 * @file CSKitDebug_AutoPilotModeRecord.h
 * @brief 自動入力 パッド入力を保存,再生するモード
 * @author megasuraman
 * @date 2021/12/28
 */
#pragma once

#include "CoreMinimal.h"
#include "AutoPilot/CSKitDebug_AutoPilotModeBase.h"
#include "Serialization/JsonSerializerMacros.h"
#include "InputCore/Classes/InputCoreTypes.h"
#include "CSKitDebug_AutoPilotModeRecord.generated.h"

class UCanvas;

UCLASS()
class CSKITDEBUG_API UCSKitDebug_AutoPilotModeRecord : public UCSKitDebug_AutoPilotModeBase
{
	GENERATED_BODY()

	/* ------------------------------------------------------------
		!自動操作モード
	------------------------------------------------------------ */
	enum class ECommandMode : uint8
	{
		Invalid,
		Idle,
		PlayInputRecord,
		BeginRecord,
		Record,
		EndRecord,
	};
	/* ------------------------------------------------------------
		!入力記録モード時の状態
	------------------------------------------------------------ */
	enum class EPlayInputRecordState : uint8
	{
		Invalid,
		Load,
		WaitReady,
		Play,
		Finish,
	};
	/* ------------------------------------------------------------
	   !自動操作時のコマンド単体
	------------------------------------------------------------ */
	struct FCommandNode : public FJsonSerializable
	{
		BEGIN_JSON_SERIALIZER
			JSON_SERIALIZE("mBeginFrame", mBeginFrame);
		JSON_SERIALIZE("mEndFrame", mEndFrame);
		JSON_SERIALIZE("mAxisValue", mAxisValue);
		JSON_SERIALIZE("mDeltaTime", mDeltaTime);
		JSON_SERIALIZE("mKeyId", mKeyId);
		JSON_SERIALIZE("mInputEventId", mInputEventId);
		JSON_SERIALIZE("mIndex", mIndex);
		END_JSON_SERIALIZER

			uint32	mBeginFrame = 0;
		uint32	mEndFrame = 0;
		float	mAxisValue = 0.f;
		float	mDeltaTime = 0.f;
		uint32	mKeyId = 0;
		uint32	mInputEventId = 0;
		int32	mIndex = INDEX_NONE;//CommandPtrListからmListのIndexを取得するために。。。

		bool	IsSameInput(const FCommandNode& InCommand) const
		{
			return (mKeyId == InCommand.mKeyId
				&& mInputEventId == InCommand.mInputEventId
				&& mAxisValue == InCommand.mAxisValue);
		}
	};
	/* ------------------------------------------------------------
	   !自動操作時のコマンドリスト
	------------------------------------------------------------ */
	struct FCommandList : public FJsonSerializable
	{
		BEGIN_JSON_SERIALIZER
			JSON_SERIALIZE_ARRAY_SERIALIZABLE("mCommandList", mList, FCommandNode);
		JSON_SERIALIZE("mStartPosX", mStartPosX);
		JSON_SERIALIZE("mStartPosY", mStartPosY);
		JSON_SERIALIZE("mStartPosZ", mStartPosZ);
		JSON_SERIALIZE("mStartRotatorPitch", mStartRotatorPitch);
		JSON_SERIALIZE("mStartRotatorYaw", mStartRotatorYaw);
		JSON_SERIALIZE("mStartRotatorRoll", mStartRotatorRoll);
		JSON_SERIALIZE("mStartCameraRotatorPitch", mStartCameraRotatorPitch);
		JSON_SERIALIZE("mStartCameraRotatorYaw", mStartCameraRotatorYaw);
		JSON_SERIALIZE("mStartCameraRotatorRoll", mStartCameraRotatorRoll);
		JSON_SERIALIZE("mStartControllerPitch", mStartControllerPitch);
		JSON_SERIALIZE("mEndFrame", mEndFrame);
		END_JSON_SERIALIZER

			TArray<FCommandNode>	mList;
		float		mStartPosX = 0.f;
		float		mStartPosY = 0.f;
		float		mStartPosZ = 0.f;
		float		mStartRotatorPitch = 0.f;
		float		mStartRotatorYaw = 0.f;
		float		mStartRotatorRoll = 0.f;
		float		mStartCameraRotatorPitch = 0.f;
		float		mStartCameraRotatorYaw = 0.f;
		float		mStartCameraRotatorRoll = 0.f;
		float		mStartControllerPitch = 0.f;
		uint32		mEndFrame = 0;
	};

public:
	virtual void	PreProcessInput(float DeltaTime) override;
	virtual void	PostProcessInput(float DeltaTime) override;
	virtual void	DebugDraw(class UCanvas* InCanvas) override;

	void	SetMode(ECommandMode InMode);
	void	RequestPlayInputRecord(const FString& InFileName);
	bool	IsFinihPlay() const;
	void	RequestBeginRecord(const FString& InFileName);
	void	RequestEndRecord();
	void	RequestIdle();

	FString	GetFilePath() const;

	bool	UpdatePlayInputRecord(float DeltaTime);
	bool	LoadInputRecordFile(float DeltaTime);
	bool	WaitPlayInputRecordFile(float DeltaTime);
	bool	PlayInputRecordFile(float DeltaTime);

	bool	UpdateInputRecord(float DeltaTime);
	bool	BeginInputRecord(float DeltaTime);
	bool	RecordingInput(float DeltaTime);

private:
	typedef	TArray<FCommandNode*>	CommandPtrList;
	CommandPtrList	mBeforeFrameCommandList;
	FString	mFileName;
	uint32	mPlayFrame = 0;
	float	mWarpInterval = 0.f;
	ECommandMode	mMode = ECommandMode::Invalid;
	FCommandList	mCommand;
	EPlayInputRecordState	mPlayInputRecordState = EPlayInputRecordState::Invalid;

#if 0

public:	
	virtual	void	DebugUpdateSelectInfo(UDebugSelectComponent& InDebugSelect, float DeltaTime) override;
	virtual void	DebugDrawSelected(UCanvas* Canvas, const APlayerController* InPlayerController) override;
	virtual void	OnPushUIKey(const FKey& InKey) override;

protected:

	void	DebugUpdateSelectInfo_Player(UDebugSelectComponent& InDebugSelect);
	void	DebugUpdateSelectInfo_InputKey(UDebugSelectComponent& InDebugSelect);
	void	DebugUpdateSelectInfo_PlayKey(UDebugSelectComponent& InDebugSelect);

private:
#endif
};