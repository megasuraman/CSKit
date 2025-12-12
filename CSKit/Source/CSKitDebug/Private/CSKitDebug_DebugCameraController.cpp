// Copyright 2020 megasuraman.
/**
 * @file CSKitDebug_DebugCameraController.cpp
 * @brief 操作方法や機能を変更したDebugCamera
 * @author megasuraman
 * @date 2025/05/05
 */
#include "CSKitDebug_DebugCameraController.h"

#include "CSKitDebug_Draw.h"
#include "CSKitDebug_Utility.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/SpectatorPawn.h"
#include "GameFramework/PlayerInput.h"
#include "DebugRenderSceneProxy.h"
#include "DrawDebugHelpers.h"
#include "NavigationSystem.h"
#include "AI/Navigation/NavigationTypes.h"
#include "Debug/DebugDrawService.h"
#include "HAL/PlatformApplicationMisc.h"
#include "Kismet/GameplayStatics.h"
#include "ScreenWindow/CSKitDebug_ScreenWindowText.h"

/**
 * @brief
 */
ASpectatorPawn* ACSKitDebug_DebugCameraController::SpawnSpectatorPawn()
{
	//本当は専用のSpectatorPawn用意すべきだけど、基底クラスの処理にフラグ操作を挟んで代用
	//return Super::SpawnSpectatorPawn();

	ASpectatorPawn* SpawnedSpectator = nullptr;

	// Only spawned for the local player
	if (GetSpectatorPawn() == nullptr && IsLocalController())
	{
		// ReSharper disable once CppTooWideScope
		AGameStateBase const* const GameState = GetWorld()->GetGameState();
		if (GameState)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			SpawnParams.ObjectFlags |= RF_Transient;	// We never want to save spectator pawns into a map

			SpawnedSpectator = GetWorld()->SpawnActor<ASpectatorPawn>((*GameState->SpectatorClass ? *GameState->SpectatorClass : ASpectatorPawn::StaticClass()), GetSpawnLocation(), GetControlRotation(), SpawnParams);
			if (SpawnedSpectator)
			{
				SpawnedSpectator->bAddDefaultMovementBindings = false;//デフォルトアサインいらない
				SpawnedSpectator->PossessedBy(this);
#if ENGINE_MAJOR_VERSION == 4
				SpawnedSpectator->PawnClientRestart();
#else
				SpawnedSpectator->DispatchRestart(true);
#endif
				if (SpawnedSpectator->PrimaryActorTick.bStartWithTickEnabled)
				{
					SpawnedSpectator->SetActorTickEnabled(true);
				}

				UE_LOG(LogPlayerController, Verbose, TEXT("Spawned spectator %s [server:%d]"), *GetNameSafe(SpawnedSpectator), GetNetMode() < NM_Client);
			}
			else
			{
				UE_LOG(LogPlayerController, Warning, TEXT("Failed to spawn spectator with class %s"), GameState->SpectatorClass ? *GameState->SpectatorClass->GetName() : TEXT("NULL"));
			}
		}
		else
		{
			// This normally happens on clients if the Player is replicated but the GameState has not yet.
			UE_LOG(LogPlayerController, Verbose, TEXT("NULL GameState when trying to spawn spectator!"));
		}
	}

	return SpawnedSpectator != nullptr ? SpawnedSpectator : APlayerController::SpawnSpectatorPawn();
}
/**
 * @brief
 */
void ACSKitDebug_DebugCameraController::OnActivate(APlayerController* OriginalPC)
{
	Super::OnActivate(OriginalPC);

	DebugRequestDraw(true);
	TestPathFindingState = ETestPathFindingState::Standby;
}
/**
 * @brief
 */
void ACSKitDebug_DebugCameraController::OnDeactivate(APlayerController* RestoredPC)
{
	Super::OnDeactivate(RestoredPC);

	DebugRequestDraw(false);
}
/**
 * @brief
 */
void ACSKitDebug_DebugCameraController::SetupInputComponent()
{
	//Super::SetupInputComponent();通常のアサインは全部無視
	APlayerController::SetupInputComponent();
	
	static bool bBindingsAdded = false;
	if (!bBindingsAdded)
	{
		bBindingsAdded = true;

		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("CSKit_DebugCamera_MoveForward", EKeys::W, 1.f));
		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("CSKit_DebugCamera_MoveForward", EKeys::S, -1.f));
		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("CSKit_DebugCamera_MoveForward", EKeys::Gamepad_LeftY, 1.f));
		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("CSKit_DebugCamera_MoveRight", EKeys::A, -1.f));
		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("CSKit_DebugCamera_MoveRight", EKeys::D, 1.f));
		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("CSKit_DebugCamera_MoveRight", EKeys::Gamepad_LeftX, 1.f));
		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("CSKit_DebugCamera_MoveUp", EKeys::Gamepad_RightTrigger, 1.f));
		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("CSKit_DebugCamera_MoveUp", EKeys::Gamepad_LeftTrigger, -1.f));

		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("CSKit_DebugCamera_TurnRate", EKeys::Gamepad_RightX, 1.f));
		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("CSKit_DebugCamera_LookUpRate", EKeys::Gamepad_RightY, 1.f));

		UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("CSKit_DebugCamera_SpeedLevelUp", EKeys::Gamepad_RightShoulder));
		UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("CSKit_DebugCamera_SpeedLevelDown", EKeys::Gamepad_LeftShoulder));
		UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("CSKit_DebugCamera_WarpPlayer", EKeys::Gamepad_FaceButton_Top));
		UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("CSKit_DebugCamera_DrawViewPointInfo", EKeys::Gamepad_FaceButton_Left));
		UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("CSKit_DebugCamera_StepTestPathFindingState", EKeys::Gamepad_FaceButton_Right));
	}

	InputComponent->BindAxis("CSKit_DebugCamera_MoveForward", this, &ACSKitDebug_DebugCameraController::MoveForward);
	InputComponent->BindAxis("CSKit_DebugCamera_MoveRight", this, &ACSKitDebug_DebugCameraController::MoveRight);
	InputComponent->BindAxis("CSKit_DebugCamera_TurnRate", this, &ACSKitDebug_DebugCameraController::TurnAtRate);
	InputComponent->BindAxis("CSKit_DebugCamera_LookUpRate", this, &ACSKitDebug_DebugCameraController::LookUpAtRate);
	InputComponent->BindAxis("CSKit_DebugCamera_MoveUp", this, &ACSKitDebug_DebugCameraController::MoveUp_World);
	InputComponent->BindAction("CSKit_DebugCamera_SpeedLevelUp", IE_Pressed, this, &ACSKitDebug_DebugCameraController::SpeedLevelUp);
	InputComponent->BindAction("CSKit_DebugCamera_SpeedLevelDown", IE_Pressed, this, &ACSKitDebug_DebugCameraController::SpeedLevelDown);
	InputComponent->BindAction("CSKit_DebugCamera_WarpPlayer", IE_Pressed, this, &ACSKitDebug_DebugCameraController::WarpPlayer);
	InputComponent->BindAction("CSKit_DebugCamera_DrawViewPointInfo", IE_Pressed, this, &ACSKitDebug_DebugCameraController::SwitchDrawViewPointInfo);
	InputComponent->BindAction("CSKit_DebugCamera_StepTestPathFindingState", IE_Pressed, this, &ACSKitDebug_DebugCameraController::StepTestPathFindingState);

}

/**
 * @brief 前後移動
 */
// ReSharper disable once CppMemberFunctionMayBeConst
void ACSKitDebug_DebugCameraController::MoveForward(float Val)
{
	if(ASpectatorPawn* OwnerPawn = GetSpectatorPawn())
	{
		OwnerPawn->MoveForward(Val);
	}
}
/**
 * @brief 左右移動
 */
// ReSharper disable once CppMemberFunctionMayBeConst
void ACSKitDebug_DebugCameraController::MoveRight(float Val)
{
	if (ASpectatorPawn* OwnerPawn = GetSpectatorPawn())
	{
		OwnerPawn->MoveRight(Val);
	}
}
/**
 * @brief Z軸方向の移動
 */
// ReSharper disable once CppMemberFunctionMayBeConst
void ACSKitDebug_DebugCameraController::MoveUp_World(float Val)
{
	if (ASpectatorPawn* OwnerPawn = GetSpectatorPawn())
	{
		OwnerPawn->MoveUp_World(Val);
	}
}
/**
 * @brief カメラ向きの左右操作
 */
// ReSharper disable once CppMemberFunctionMayBeConst
void ACSKitDebug_DebugCameraController::TurnAtRate(float Rate)
{
	if (ASpectatorPawn* OwnerPawn = GetSpectatorPawn())
	{
		OwnerPawn->TurnAtRate(Rate);
	}
}
/**
 * @brief カメラ向きの上下操作
 */
// ReSharper disable once CppMemberFunctionMayBeConst
void ACSKitDebug_DebugCameraController::LookUpAtRate(float Rate)
{
	if (ASpectatorPawn* OwnerPawn = GetSpectatorPawn())
	{
		OwnerPawn->LookUpAtRate(Rate);
	}
}
/**
 * @brief カメラ移動速度アップ
 */
void ACSKitDebug_DebugCameraController::SpeedLevelUp()
{
	SpeedScale *= 2.f;
	ApplySpeedScale();
}
/**
 * @brief カメラ移動速度ダウン
 */
void ACSKitDebug_DebugCameraController::SpeedLevelDown()
{
	SpeedScale *= 0.5f;
	SpeedScale = FMath::Max(SpeedScale, 0.25f); 
	ApplySpeedScale();
}
/**
 * @brief 注視点の先にプレイヤーをワープ
 */
// ReSharper disable once CppMemberFunctionMayBeConst
void ACSKitDebug_DebugCameraController::WarpPlayer()
{
	FHitResult HitResult;
	if(!LineTraceChannelToViewPoint(HitResult, ECC_Pawn))
	{
		return;
	}

	FVector CamLoc;
	FRotator CamRot;
	GetPlayerViewPoint(CamLoc, CamRot);

	const FVector WarpPos = HitResult.ImpactPoint + FVector(0.f,0.f,100.f);
	FRotator WarpRot = CamRot;
	WarpRot.Pitch = 0.f;

	if (APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0))
	{
		PlayerPawn->SetActorLocationAndRotation(WarpPos, WarpRot.Quaternion());
#if PLATFORM_WINDOWS
		FPlatformApplicationMisc::ClipboardCopy(*WarpPos.ToString());
#endif
	}
}
/**
 * @brief 注視点情報表示切り替え
 */
void ACSKitDebug_DebugCameraController::SwitchDrawViewPointInfo()
{
	bDrawViewPointInfo = !bDrawViewPointInfo;
}
/**
 * @brief 経路探索テスト
 */
void ACSKitDebug_DebugCameraController::StepTestPathFindingState()
{
	switch(TestPathFindingState)
	{
	case ETestPathFindingState::Standby:
	{
		TestPathFindingState = ETestPathFindingState::FindBeginPos;
		break;
	}
	case ETestPathFindingState::FindBeginPos:
	{
		FHitResult HitResult;
		if(LineTraceChannelToViewPoint(HitResult, ECC_Pawn))
		{
			TestPathFindingBeginPos = HitResult.ImpactPoint;
			TestPathFindingState = ETestPathFindingState::FindEndPos;
		}
		break;
	}
	case ETestPathFindingState::FindEndPos:
	{
		FHitResult HitResult;
		if (LineTraceChannelToViewPoint(HitResult, ECC_Pawn))
		{
			TestPathFindingEndPos = HitResult.ImpactPoint;
			TestPathFinding();
			TestPathFindingState = ETestPathFindingState::KeepResult;
		}
		break;
	}
	case ETestPathFindingState::KeepResult:
	{
		TestPathFindingState = ETestPathFindingState::Standby;
		break;
	}
	default:
		break;
	};
}
/**
 * @brief Debug用Draw関数登録のon/off
 */
bool ACSKitDebug_DebugCameraController::LineTraceChannelToViewPoint(FHitResult& OutResult, const ECollisionChannel InTraceChannel) const
{
	FVector CamLoc;
	FRotator CamRot;
	GetPlayerViewPoint(CamLoc, CamRot);

	const FVector CameraTipPos = CamLoc + CamRot.RotateVector(FVector(10000.f, 0.f, 0.f));
	FCollisionQueryParams Params(FName(TEXT("DebugCameraControllerNavSys")), false);
	return GetWorld()->LineTraceSingleByChannel(
		OutResult,
		CamLoc,
		CameraTipPos,
		InTraceChannel,
		Params
	);
}
/**
 * @brief 経路探索テスト
 */
void ACSKitDebug_DebugCameraController::TestPathFinding()
{
	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	if (NavSys == nullptr)
	{
		return;
	}
	if(NavSys->NavDataSet.Num() == 0)
	{
		return;
	}
	const ANavigationData* NavData = NavSys->NavDataSet[0];
	if(NavData == nullptr)
	{
		return;
	}
	FPathFindingQuery Query(NavSys, *NavData, TestPathFindingBeginPos, TestPathFindingEndPos);
	Query.SetAllowPartialPaths(false);
	TestPathFindingResult = NavSys->FindPathSync(Query);
}
/**
 * @brief Debug用Draw関数登録のon/off
 */
void ACSKitDebug_DebugCameraController::DebugRequestDraw(const bool bInActive)
{
	if (bInActive)
	{
		if (!mDebugDrawHandle.IsValid())
		{
			const auto DebugDrawDelegate = FDebugDrawDelegate::CreateUObject(this, &ACSKitDebug_DebugCameraController::DebugDraw);
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
 * @brief Debug表示
 */
void ACSKitDebug_DebugCameraController::DebugDraw(UCanvas* InCanvas, class APlayerController* InPlayerController) const
{
	if(bDrawViewPointInfo)
	{
		DebugDrawViewPointInfo(InCanvas);
	}
	DebugDrawTestPathFinding(InCanvas);
}
/**
 * @brief 注視点情報表示
 */
void ACSKitDebug_DebugCameraController::DebugDrawViewPointInfo(UCanvas* InCanvas) const
{
	FHitResult HitResult;
	if (!LineTraceChannelToViewPoint(HitResult, ECC_Pawn))
	{
		return;
	}

	//DrawDebugLine(GetWorld(), HitResult.ImpactPoint, HitResult.ImpactPoint + HitResult.ImpactNormal*50.f, FColor::Orange, false, -1.f, 0, 3.f);
	UCSKitDebug_Draw::OctahedronArrow OctahedronArrow;
	OctahedronArrow.mTargetPos = HitResult.ImpactPoint;
	OctahedronArrow.mBasePos = HitResult.ImpactPoint + HitResult.ImpactNormal * 100.f;
	OctahedronArrow.Draw(GetWorld(), FColor::Orange, 0, 2.f);

	FCSKitDebug_ScreenWindowText Window;
	Window.SetWindowName(FString(TEXT("DebugCamera")));
	Window.AddText(FString::Printf(TEXT("ImpactPos : %s"), *HitResult.ImpactPoint.ToCompactString()));
	const float SlopeAngle = FMath::RadiansToDegrees(FMath::Abs(FMath::Acos(FVector::DotProduct(HitResult.ImpactNormal, FVector::UpVector))));
	Window.AddText(FString::Printf(TEXT("ImpactNormal : %s(%.1f)"), *HitResult.ImpactNormal.ToCompactString(), SlopeAngle));

	Window.AddText(FString::Printf(TEXT("HitObject")));
	if(const AActor* HitActor = HitResult.GetActor())
	{
		Window.AddText(FString::Printf(TEXT("  Level : %s"), *UCSKitDebug_Utility::GetActorLevelName(HitActor)));
		Window.AddText(FString::Printf(TEXT("  Actor : %s"), *UCSKitDebug_Utility::GetObjectDisplayName(HitActor)));
	}
	if(const UPrimitiveComponent* HitComponent = HitResult.GetComponent())
	{
		Window.AddText(FString::Printf(TEXT("  Component : %s"), *UCSKitDebug_Utility::GetObjectDisplayName(HitComponent)));
		Window.AddText(FString::Printf(TEXT("    CanEverAffectNavigation : %d"), HitComponent->CanEverAffectNavigation()));
		Window.AddText(FString::Printf(TEXT("    IsNavigationRelevant : %d"), HitComponent->IsNavigationRelevant()));
		Window.AddText(FString::Printf(TEXT("    SimulatePhysics : %d"), HitComponent->BodyInstance.bSimulatePhysics));
		if (const INavRelevantInterface* NavInterface = Cast<INavRelevantInterface>(HitComponent))
		{
			const FVector NavPos = NavInterface->GetNavigationBounds().GetCenter();
			const FVector NavExtent = NavInterface->GetNavigationBounds().GetExtent();
			Window.AddText(FString::Printf(TEXT("    NavRelevantInterface")));
			Window.AddText(FString::Printf(TEXT("      Pos : %s"), *NavPos.ToCompactString()));
			Window.AddText(FString::Printf(TEXT("      Extent : %s"), *NavExtent.ToCompactString()));
			DrawDebugBox(GetWorld(), NavPos, NavExtent, FColor::Yellow, false, -1.f, 255, 3.f);
			DrawDebugPoint(GetWorld(), NavPos, 20.f, FColor::Yellow, false, -1.f, 255);
		}
	}
	Window.Draw(InCanvas, OctahedronArrow.mBasePos);
}
/**
 * @brief 経路探索テスト表示
 */
void ACSKitDebug_DebugCameraController::DebugDrawTestPathFinding(UCanvas* InCanvas) const
{
	if(TestPathFindingState == ETestPathFindingState::Standby)
	{
		return;
	}

	const FVector ArrowOffsetV(0.f, 0.f,100.f);
	constexpr float Thickness = 2.f;
	if (TestPathFindingState == ETestPathFindingState::FindBeginPos
		|| TestPathFindingState == ETestPathFindingState::FindEndPos)
	{//座標決め中
		FHitResult HitResult;
		if (LineTraceChannelToViewPoint(HitResult, ECC_Pawn))
		{
			const FColor LineColor = FColor::White;
			UCSKitDebug_Draw::OctahedronArrow OctahedronArrow;
			OctahedronArrow.mTargetPos = HitResult.ImpactPoint;
			OctahedronArrow.mBasePos = HitResult.ImpactPoint + ArrowOffsetV;
			OctahedronArrow.Draw(GetWorld(), LineColor, 0, Thickness);

			FCSKitDebug_ScreenWindowText Window;
			Window.SetWindowFrameColor(LineColor);
			if(TestPathFindingState == ETestPathFindingState::FindBeginPos)
			{
				Window.SetWindowName(FString(TEXT("Bボタンで始点指定")));
			}
			else
			{
				Window.SetWindowName(FString(TEXT("Bボタンで終点指定")));
			}
			Window.AddText(FString::Printf(TEXT("%s"), *HitResult.ImpactPoint.ToCompactString()));
			Window.Draw(InCanvas, OctahedronArrow.mBasePos);
		}
	}

	if (TestPathFindingState == ETestPathFindingState::FindEndPos
		|| TestPathFindingState == ETestPathFindingState::KeepResult)
	{//BeginPos
		const FColor LineColor = FColor::Blue;
		UCSKitDebug_Draw::OctahedronArrow OctahedronArrow;
		OctahedronArrow.mTargetPos = TestPathFindingBeginPos;
		OctahedronArrow.mBasePos = TestPathFindingBeginPos + ArrowOffsetV;
		OctahedronArrow.Draw(GetWorld(), LineColor, 0, Thickness);

		FCSKitDebug_ScreenWindowText Window;
		Window.SetWindowFrameColor(LineColor);
		Window.SetWindowName(FString(TEXT("BeginPos")));
		Window.AddText(FString::Printf(TEXT("%s"), *TestPathFindingBeginPos.ToCompactString()));
		Window.Draw(InCanvas, OctahedronArrow.mBasePos);
	}

	if (TestPathFindingState != ETestPathFindingState::KeepResult)
	{
		return;
	}

	const bool bFailedPathFind = (TestPathFindingResult.Result != ENavigationQueryResult::Success);

	{//EndPos
		FColor LineColor = FColor::Blue;
		if(bFailedPathFind)
		{
			LineColor = FColor::Red;
			DrawDebugLine(GetWorld(), TestPathFindingBeginPos + ArrowOffsetV, TestPathFindingEndPos + ArrowOffsetV, LineColor, false, -1.f, 0, Thickness);
		}
		UCSKitDebug_Draw::OctahedronArrow OctahedronArrow;
		OctahedronArrow.mTargetPos = TestPathFindingEndPos;
		OctahedronArrow.mBasePos = TestPathFindingEndPos + ArrowOffsetV;
		OctahedronArrow.Draw(GetWorld(), LineColor, 0, Thickness);

		FCSKitDebug_ScreenWindowText Window;
		Window.SetWindowFrameColor(LineColor);
		Window.SetWindowName(FString(TEXT("EndPos")));
		Window.AddText(FString::Printf(TEXT("%s"), *TestPathFindingEndPos.ToCompactString()));
		const FVector EndPosV = TestPathFindingEndPos - TestPathFindingBeginPos;
		Window.AddText(FString::Printf(TEXT("EndV : %s(%.1f)"), *EndPosV.ToCompactString(), FVector::Dist(TestPathFindingEndPos, TestPathFindingBeginPos)));
		if(bFailedPathFind)
		{
			Window.AddText(FString::Printf(TEXT("Result Miss")));
		}
		Window.Draw(InCanvas, OctahedronArrow.mBasePos);
	}

	if(bFailedPathFind)
	{
		return;
	}

	//経路探索結果
	const FNavigationPath* PathInstance = TestPathFindingResult.Path.Get();
	if (PathInstance == nullptr)
	{
		return;
	}

	const FColor LineColor = FColor::Green;
	const int32 PathPointNum = PathInstance->GetPathPoints().Num();
	constexpr int32 NeedPointNum = 2;
	if (PathPointNum < NeedPointNum	)
	{
		return;
	}

	FVector BeginLocation = PathInstance->GetPathPoints()[0].Location;
	for (int32 i = 0; i + 1 < PathPointNum; ++i)
	{
		//const FNavPathPoint& PathPoint = PathInstance->GetPathPoints()[i];
		const FNavPathPoint& NextPathPoint = PathInstance->GetPathPoints()[i + 1];
		const FVector EndLocation = NextPathPoint.Location;

		DrawDebugLine(GetWorld(), BeginLocation, EndLocation, LineColor);
		DrawDebugLine(GetWorld(), EndLocation, EndLocation + ArrowOffsetV, LineColor);

		FCSKitDebug_ScreenWindowText ScreenWindowText;
		ScreenWindowText.AddText(FString::Printf(TEXT("Index : %d"), i + 1));
		ScreenWindowText.AddText(FString::Printf(TEXT("pos(%s)"), *EndLocation.ToString()));
		ScreenWindowText.Draw(InCanvas, EndLocation);

		BeginLocation = EndLocation;
	}
}
