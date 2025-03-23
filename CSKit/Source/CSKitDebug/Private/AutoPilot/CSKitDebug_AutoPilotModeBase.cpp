// Copyright 2021 megasuraman
/**
 * @file CSKitDebug_AutoPilotModeBase.cpp
 * @brief 自動入力等を補助する各モード処理のベース
 * @author megasuraman
 * @date 2021/12/28
 */

#include "CSKitDebug_AutoPilotModeBase.h"
#include "AutoPilot/CSKitDebug_AutoPilotComponent.h"

#include "Kismet/GameplayStatics.h"
#include "Engine/Public/DrawDebugHelpers.h"
#include "Engine/Public/CanvasItem.h"
#include "Engine/Canvas.h"
#include "Engine/Engine.h"
#include "GameFramework/PlayerInput.h"

/**
 * @brief	親としてComponentをセット
 */
void	UCSKitDebug_AutoPilotModeBase::SetParent(UCSKitDebug_AutoPilotComponent* InParentComponent)
{
	mAutoPilotComponent = InParentComponent;
	mPlayerController = InParentComponent->GetPlayerController();

	InitializeKeyMap();
	InitializePadDeadZoneMap();
	OnSetParent();
}

UCSKitDebug_AutoPilotComponent* UCSKitDebug_AutoPilotModeBase::GetParent() const
{
	return mAutoPilotComponent.Get();
}

APlayerController* UCSKitDebug_AutoPilotModeBase::GetPlayerController() const
{
	return mPlayerController.Get();
}

/**
 * @brief	パッド入力値取得
 */
float	UCSKitDebug_AutoPilotModeBase::GetDebugDrawPadInfoAxisValue(ECSKitDebug_AutoPilotKey InKey) const
{
	for (const FCSKitDebug_AutoPilotDebugDrawPadInfo& Info : mDebugDrawPadInfoList)
	{
		if (Info.mKey == InKey)
		{
			return Info.mAxisValue;
		}
	}
	return 0.f;
}

/**
 * @brief	Key
 */
FKey	UCSKitDebug_AutoPilotModeBase::GetKey(ECSKitDebug_AutoPilotKey InKey) const
{
	if (InKey == ECSKitDebug_AutoPilotKey::Invalid
		|| InKey == ECSKitDebug_AutoPilotKey::Num)
	{
		return FKey();
	}
	const FKey* Key = mKeyMap.Find(InKey);
	if (Key == nullptr)
	{
		return FKey();
	}

	return *Key;
}

/**
 * @brief	パッド入力状態デバッグ表示
 */
void	UCSKitDebug_AutoPilotModeBase::DebugDrawPad(UCanvas* InCanvas)
{
	constexpr float VirtualGridLen = 15.f;//グリッド上に図形書くイメージで
	const FVector2D ButtonPosGridList[static_cast<uint8>(ECSKitDebug_AutoPilotKey::Num)] =
	{
		FVector2D::ZeroVector,// Invalid,	//0
		FVector2D(4.0f, 7.0f),// LeftStickX,	//1
		FVector2D(4.0f, 7.0f),// LeftStickY,	//2
		FVector2D(8.0f, 7.0f),// RightStickX,//3
		FVector2D(8.0f, 7.0f),// RightStickY,//4
		FVector2D(2.0f, 4.0f),// Up,			//5
		FVector2D(2.0f, 6.0f),// Down,		//6
		FVector2D(1.0f, 5.0f),// Left,		//7
		FVector2D(3.0f, 5.0f),// Right,		//8
		FVector2D(2.0f, 2.75f),// L1,			//9
		FVector2D(2.0f, 1.5f),// L2,			//10
		FVector2D(4.0f, 7.0f),// L3,			//11
		FVector2D(10.0f,2.75f),// R1,			//12
		FVector2D(10.0f,1.5f),// R2,			//13
		FVector2D(8.0f, 7.0f),// R3,			//14
		FVector2D(10.0f,4.0f),// Sankaku,	//15
		FVector2D(9.0f, 5.0f),// Shikaku,	//16
		FVector2D(10.0f,6.0f),// Batsu,		//17
		FVector2D(11.0f,5.0f),// Maru,		//18
		FVector2D(8.5f, 3.75f),// Option,		//19
		//(5.f,3.5f)タッチパッド左
		//(7.f,3.5f)タッチパッド右
	};
	const FVector2D ButtonLenGridList[static_cast<uint8>(ECSKitDebug_AutoPilotKey::Num)] =
	{
		FVector2D::UnitVector,// Invalid,	//0
		FVector2D(2.0f, 2.0f),// LeftStickX,	//1
		FVector2D(2.0f, 2.0f),// LeftStickY,	//2
		FVector2D(2.0f, 2.0f),// RightStickX,//3
		FVector2D(2.0f, 2.0f),// RightStickY,//4
		FVector2D(0.5f, 1.0f),// Up,			//5
		FVector2D(0.5f, 1.0f),// Down,		//6
		FVector2D(1.0f, 0.5f),// Left,		//7
		FVector2D(1.0f, 0.5f),// Right,		//8
		FVector2D(1.5f, 0.5f),// L1,			//9
		FVector2D(1.5f, 1.0f),// L2,			//10
		FVector2D(1.0f, 1.0f),// L3,			//11
		FVector2D(1.5f, 0.5f),// R1,			//12
		FVector2D(1.5f, 1.0f),// R2,			//13
		FVector2D(1.0f, 1.0f),// R3,			//14
		FVector2D(0.5f, 0.5f),// Sankaku,	//15
		FVector2D(0.5f, 0.5f),// Shikaku,	//16
		FVector2D(0.5f, 0.5f),// Batsu,		//17
		FVector2D(0.5f, 0.5f),// Maru,		//18
		FVector2D(0.5f, 0.5f),// Option,		//19
		//(1.0f,1.0f)タッチパッド左
		//(1.0f,1.0f)タッチパッド右
	};

	const FVector2D Extent(VirtualGridLen*12.f, VirtualGridLen*9.f);
	FVector2D ScreenPos(50.f, 50.f);
	if (GEngine
		&& GEngine->GameViewport)
	{
		FVector2D ViewportSize;
		GEngine->GameViewport->GetViewportSize(ViewportSize);
		ScreenPos.X = ViewportSize.X*0.5f - Extent.X*0.5f;
	}
	DebugDrawPadSheet(InCanvas, ScreenPos, Extent);


	for (uint8 i = 1; i < static_cast<uint8>(ECSKitDebug_AutoPilotKey::Num); ++i)
	{
		const ECSKitDebug_AutoPilotKey KeyId = static_cast<ECSKitDebug_AutoPilotKey>(i);

		const FVector2D CenterPos = ScreenPos + ButtonPosGridList[i] * VirtualGridLen;
		const FVector2D ExtentV = ButtonLenGridList[i] * VirtualGridLen * 0.5f;

		//stickは特殊対処
		if (KeyId == ECSKitDebug_AutoPilotKey::LeftStickX)
		{
			FVector2D AxisV(GetDebugDrawPadInfoAxisValue(ECSKitDebug_AutoPilotKey::LeftStickX), GetDebugDrawPadInfoAxisValue(ECSKitDebug_AutoPilotKey::LeftStickY));
			DebugDrawStick(InCanvas, AxisV, CenterPos, ExtentV.X);
		}
		else if (KeyId == ECSKitDebug_AutoPilotKey::RightStickX)
		{
			FVector2D AxisV(GetDebugDrawPadInfoAxisValue(ECSKitDebug_AutoPilotKey::RightStickX), GetDebugDrawPadInfoAxisValue(ECSKitDebug_AutoPilotKey::RightStickY));
			AxisV *= FVector2D(1.f, -1.f);//何故か逆になるので(もしかして順,逆のオプションの影響受けてる？)
			DebugDrawStick(InCanvas, AxisV, CenterPos, ExtentV.X);
		}
		else if (KeyId == ECSKitDebug_AutoPilotKey::LeftStickY
			|| KeyId == ECSKitDebug_AutoPilotKey::RightStickY)
		{	//xに任せるので
			continue;;
		}
		else if (KeyId == ECSKitDebug_AutoPilotKey::L3
			|| KeyId == ECSKitDebug_AutoPilotKey::R3)
		{
			if (GetDebugDrawPadInfoAxisValue(KeyId) > 0.f)
			{
				FLinearColor FrameColor = FLinearColor(1.f, 0.5f, 0.f);
				DrawDebugCanvas2DCircle(InCanvas, CenterPos, ExtentV.X, 10, FrameColor, 2.f);
			}
		}
		else
		{
			DebugDrawButton(InCanvas, KeyId, CenterPos, ExtentV);
		}
	}

	mDebugDrawPadInfoList.Reset();
}

/**
 * @brief	KeyMap用意
 */
void UCSKitDebug_AutoPilotModeBase::InitializeKeyMap()
{
	if (mKeyMap.Num() != 0)
	{
		return;
	}

	mKeyMap.Reserve(static_cast<int32>(ECSKitDebug_AutoPilotKey::Num));

	mKeyMap.Add(ECSKitDebug_AutoPilotKey::LeftStickX, EKeys::Gamepad_LeftX);
	mKeyMap.Add(ECSKitDebug_AutoPilotKey::LeftStickY, EKeys::Gamepad_LeftY);
	mKeyMap.Add(ECSKitDebug_AutoPilotKey::RightStickX, EKeys::Gamepad_RightX);
	mKeyMap.Add(ECSKitDebug_AutoPilotKey::RightStickY, EKeys::Gamepad_RightY);
	mKeyMap.Add(ECSKitDebug_AutoPilotKey::Up, EKeys::Gamepad_DPad_Up);
	mKeyMap.Add(ECSKitDebug_AutoPilotKey::Down, EKeys::Gamepad_DPad_Down);
	mKeyMap.Add(ECSKitDebug_AutoPilotKey::Left, EKeys::Gamepad_DPad_Left);
	mKeyMap.Add(ECSKitDebug_AutoPilotKey::Right, EKeys::Gamepad_DPad_Right);
	mKeyMap.Add(ECSKitDebug_AutoPilotKey::L1, EKeys::Gamepad_LeftShoulder);
	mKeyMap.Add(ECSKitDebug_AutoPilotKey::L2, EKeys::Gamepad_LeftTrigger);
	mKeyMap.Add(ECSKitDebug_AutoPilotKey::L3, EKeys::Gamepad_LeftThumbstick);
	mKeyMap.Add(ECSKitDebug_AutoPilotKey::R1, EKeys::Gamepad_RightShoulder);
	mKeyMap.Add(ECSKitDebug_AutoPilotKey::R2, EKeys::Gamepad_RightTrigger);
	mKeyMap.Add(ECSKitDebug_AutoPilotKey::R3, EKeys::Gamepad_RightThumbstick);
	mKeyMap.Add(ECSKitDebug_AutoPilotKey::Sankaku, EKeys::Gamepad_FaceButton_Top);
	mKeyMap.Add(ECSKitDebug_AutoPilotKey::Shikaku, EKeys::Gamepad_FaceButton_Left);
	mKeyMap.Add(ECSKitDebug_AutoPilotKey::Batsu, EKeys::Gamepad_FaceButton_Bottom);
	mKeyMap.Add(ECSKitDebug_AutoPilotKey::Maru, EKeys::Gamepad_FaceButton_Right);
	//mKeyMap.Add(&EKeys::Gamepad_Special_Right, ECSKitDebug_AutoPilotKey::Option, );
}

/**
 * @brief	DeadZoneMap用意
 */
void UCSKitDebug_AutoPilotModeBase::InitializePadDeadZoneMap()
{
	if (mPadDeadZoneMap.Num() > 0)
	{
		return;
	}
 
	const APlayerController* PlayerController = GetPlayerController();
	if (const UPlayerInput* PlayerInput = PlayerController->PlayerInput)
	{
		for (const FInputAxisConfigEntry& AxisConfigEntry : PlayerInput->AxisConfig)
		{
			const FKey AxisKey = AxisConfigEntry.AxisKeyName;
			if (AxisKey.IsAxis1D())
			//if (AxisKey.IsFloatAxis())
			{
				mPadDeadZoneMap.Add(AxisKey, AxisConfigEntry.AxisProperties.DeadZone);
			}
		}
	}
}

/**
 * @brief	パッド入力状態デバッグ表示(下敷き)
 */
void	UCSKitDebug_AutoPilotModeBase::DebugDrawPadSheet(UCanvas* InCanvas, const FVector2D& InBasePos, const FVector2D& InExtent)
{
	constexpr FLinearColor FrameColor = { 0.5f, 1.f, 0.5f, 1.f };
	constexpr FLinearColor BackColor = { 0.f, 0.f, 0.f, 0.4f };
	constexpr uint32 WindowPointListSize = 4;
	const FVector2D WindowPointList[WindowPointListSize] =
	{
		FVector2D(InBasePos.X, InBasePos.Y),//左上
		FVector2D(InBasePos.X, InBasePos.Y + InExtent.Y),//左下
		FVector2D(InBasePos.X + InExtent.X, InBasePos.Y + InExtent.Y),//右下
		FVector2D(InBasePos.X + InExtent.X, InBasePos.Y)//右上
	};
	for (uint32 i = 0; i < WindowPointListSize; ++i)
	{//枠
		DrawDebugCanvas2DLine(InCanvas, WindowPointList[i], WindowPointList[(i + 1) % WindowPointListSize], FrameColor);
	}
	{//下敷き
		FCanvasTileItem TileItem(InBasePos, GWhiteTexture, FVector2D(InExtent.X, InExtent.Y), BackColor);
		TileItem.BlendMode = SE_BLEND_Translucent;
		InCanvas->DrawItem(TileItem);
	}
}
/**
 * @brief	2D矢印表示
 */
void	UCSKitDebug_AutoPilotModeBase::DebugDrawArrow2D(UCanvas* InCanvas, const FVector2D& InStartPos, const FVector2D& InGoalPos, const FLinearColor& InColor, float InArrowLen)
{
	DrawDebugCanvas2DLine(InCanvas, InStartPos, InGoalPos, InColor);

	FVector2D StartNV = InStartPos - InGoalPos;
	StartNV.Normalize();
	{
		FVector ArrowV(StartNV*InArrowLen, 0.f);
		ArrowV = ArrowV.RotateAngleAxis(45.f, FVector(0.f, 0.f, 1.f));
		FVector2D ArrowGoalPos(InGoalPos + FVector2D(ArrowV));
		DrawDebugCanvas2DLine(InCanvas, InGoalPos, ArrowGoalPos, InColor);
	}
	{
		FVector ArrowV(StartNV*InArrowLen, 0.f);
		ArrowV = ArrowV.RotateAngleAxis(-45.f, FVector(0.f, 0.f, 1.f));
		FVector2D ArrowGoalPos(InGoalPos + FVector2D(ArrowV));
		DrawDebugCanvas2DLine(InCanvas, InGoalPos, ArrowGoalPos, InColor);
	}
}
/**
 * @brief	ボタン表示
 */
void	UCSKitDebug_AutoPilotModeBase::DebugDrawButton(UCanvas* InCanvas, ECSKitDebug_AutoPilotKey InKey, const FVector2D& InPos, const FVector2D& InExtent) const
{
	const FLinearColor FrameColor = FLinearColor::White;
	FLinearColor BackColor = { 0.f, 0.f, 0.f, 0.4f };
	if (GetDebugDrawPadInfoAxisValue(InKey) > 0.f)
	{
		BackColor = FLinearColor(1.f, 0.5f, 0.f);
	}

	constexpr uint32 WindowPointListSize = 4;
	const FVector2D WindowPointList[WindowPointListSize] =
	{
		FVector2D(InPos.X - InExtent.X, InPos.Y - InExtent.Y),//左上
		FVector2D(InPos.X - InExtent.X, InPos.Y + InExtent.Y),//左下
		FVector2D(InPos.X + InExtent.X, InPos.Y + InExtent.Y),//右下
		FVector2D(InPos.X + InExtent.X, InPos.Y - InExtent.Y)//右上
	};
	for (uint32 i = 0; i < WindowPointListSize; ++i)
	{//枠
		DrawDebugCanvas2DLine(InCanvas, WindowPointList[i], WindowPointList[(i + 1) % WindowPointListSize], FrameColor);
	}
	{//下敷き
		FCanvasTileItem TileItem(FVector2D(InPos.X - InExtent.X, InPos.Y - InExtent.Y), GWhiteTexture, FVector2D(InExtent.X, InExtent.Y)*2.f, BackColor);
		TileItem.BlendMode = SE_BLEND_Translucent;
		InCanvas->DrawItem(TileItem);
	}
}
/**
 * @brief	スティック表示
 */
void	UCSKitDebug_AutoPilotModeBase::DebugDrawStick(UCanvas* InCanvas, const FVector2D& InAxisV, const FVector2D& InPos, const float InRadius)
{
	const FLinearColor FrameColor = FLinearColor::White;
	DrawDebugCanvas2DCircle(InCanvas, InPos, InRadius, 10, FrameColor);

	FLinearColor ArrowColor = FLinearColor::White;
	if (InAxisV.SizeSquared() > 0.f)
	{
		ArrowColor = FLinearColor(1.f, 0.5f, 0.f);
	}

	FVector2D ArrowPos = InPos + InAxisV * InRadius * FVector2D(1.f, -1.f);
	DebugDrawArrow2D(InCanvas, InPos, ArrowPos, ArrowColor, 2.5f);
}