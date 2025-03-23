// Copyright 2021 megasuraman
/**
 * @file CSKitDebug_AutoPilotModeBase.h
 * @brief �������͓���⏕����e���[�h�����̃x�[�X
 * @author megasuraman
 * @date 2021/12/28
 */

#pragma once

#include "CoreMinimal.h"
#include "InputCore/Classes/InputCoreTypes.h"
#include "CSKitDebug_AutoPilotModeBase.generated.h"

class UCanvas;
class UCSKitDebug_AutoPilotComponent;
class APlayerController;

enum class ECSKitDebug_AutoPilotKey : uint8 {
	Invalid,
	LeftStickX,
	LeftStickY,
	RightStickX,
	RightStickY,
	Up,
	Down,
	Left,
	Right,
	L1,
	L2,
	L3,
	R1,
	R2,
	R3,
	Sankaku,
	Shikaku,
	Batsu,
	Maru,
	Option,
	Num,
};

struct FCSKitDebug_AutoPilotDebugDrawPadInfo
{
	ECSKitDebug_AutoPilotKey	mKey;
	float	mAxisValue = 0.f;
	FCSKitDebug_AutoPilotDebugDrawPadInfo(ECSKitDebug_AutoPilotKey InKey, float InValue)
		:mKey(InKey)
		, mAxisValue(InValue)
	{}
};

UCLASS()
class CSKITDEBUG_API UCSKitDebug_AutoPilotModeBase : public UObject
{
	GENERATED_BODY()

public:
	void	SetParent(UCSKitDebug_AutoPilotComponent* InParentComponent);

	virtual void	PreProcessInput(float DeltaTime) {}
	virtual void	PostProcessInput(float DeltaTime) {}

	virtual void	DebugDraw(class UCanvas* InCanvas) {}

protected:
	virtual void	OnSetParent() {}
	UCSKitDebug_AutoPilotComponent* GetParent() const;
	APlayerController* GetPlayerController() const;
	float	GetDebugDrawPadInfoAxisValue(ECSKitDebug_AutoPilotKey InKey) const;
	const TMap<FKey, float>& GetPadDeadZoneMap() const { return mPadDeadZoneMap; }
	FKey GetKey(ECSKitDebug_AutoPilotKey InKey) const;

	void	AddDebugDrawPadInfo(const FCSKitDebug_AutoPilotDebugDrawPadInfo& InInfo)
	{
		mDebugDrawPadInfoList.Add(InInfo);
	}

	void	DebugDrawPad(UCanvas* InCanvas);

private:
	void	InitializeKeyMap();
	void	InitializePadDeadZoneMap();

	static void	DebugDrawPadSheet(UCanvas* InCanvas, const FVector2D& InBasePos, const FVector2D& InExtent);
	static void	DebugDrawArrow2D(UCanvas* InCanvas, const FVector2D& InStartPos, const FVector2D& InGoalPos, const FLinearColor& InColor, float InArrowLen);
	void	DebugDrawButton(UCanvas* InCanvas, ECSKitDebug_AutoPilotKey InKey, const FVector2D& InPos, const FVector2D& InExtent) const;
	static void	DebugDrawStick(UCanvas* InCanvas, const FVector2D& InAxisV, const FVector2D& InPos, const float InRadius);


private:
	TMap<ECSKitDebug_AutoPilotKey, FKey>	mKeyMap;
	TMap<FKey, float>	mPadDeadZoneMap;
	TArray<FCSKitDebug_AutoPilotDebugDrawPadInfo> mDebugDrawPadInfoList;
	TWeakObjectPtr<UCSKitDebug_AutoPilotComponent> mAutoPilotComponent;
	TWeakObjectPtr<APlayerController> mPlayerController;
};