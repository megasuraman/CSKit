// Copyright 2020 megasuraman
/**
 * @file CSKitDebug_ActorSelectManager.cpp
 * @brief デバッグ選択管理用Manager
 * @author megasuraman
 * @date 2020/05/27
 */
#include "ActorSelect/CSKitDebug_ActorSelectManager.h"

#include "Engine/DebugCameraController.h"
#include "CollisionQueryParams.h"
#include "DebugMenu/CSKitDebug_DebugMenuManager.h"
#include "ActorSelect/CSKitDebug_ActorSelectComponent.h"
#include "CSKitDebug_Config.h"
#include "ScreenWindow/CSKitDebug_ScreenWindowText.h"

#include "Engine/Canvas.h"
#include "CanvasItem.h"
#include "CSKitDebug_Subsystem.h"
#include "CSKitDebug_Utility.h"
#include "Kismet/GameplayStatics.h"

#if USE_CSKIT_DEBUG

UCSKitDebug_ActorSelectManager* UCSKitDebug_ActorSelectManager::sGet(const UWorld* InWorld)
{
	UGameInstance* GameInstance = InWorld->GetGameInstance();
	UCSKitDebug_Subsystem* CSKitDebugSubsystem = GameInstance->GetSubsystem<UCSKitDebug_Subsystem>();
	return CSKitDebugSubsystem->GetActorSelectManager();
}

/**
 * @brief	Init
 */
void	UCSKitDebug_ActorSelectManager::Init() const
{
 	UCSKitDebug_DebugMenuManager* DebugMenuManager = UCSKitDebug_DebugMenuManager::sGet(this);

 	const FString BaseDebugMenuPath(TEXT("CSKitDebug/ActorSelect"));
	DebugMenuManager->AddNode_Bool(BaseDebugMenuPath, FString(TEXT("Active")), false);
	DebugMenuManager->AddNode_Bool(BaseDebugMenuPath, FString(TEXT("UpdateOnlySelectActor")), false);
	DebugMenuManager->AddNode_Bool(BaseDebugMenuPath + FString(TEXT("/Draw")), FString(TEXT("Info")), false);
	DebugMenuManager->AddNode_Bool(BaseDebugMenuPath + FString(TEXT("/Draw")), FString(TEXT("Mark")), false);
	DebugMenuManager->AddNode_Bool(BaseDebugMenuPath + FString(TEXT("/Draw")), FString(TEXT("Axis")), false);
	DebugMenuManager->AddNode_Bool(BaseDebugMenuPath + FString(TEXT("/Draw")), FString(TEXT("Bone")), false);
	DebugMenuManager->AddNode_Bool(BaseDebugMenuPath + FString(TEXT("/Draw")), FString(TEXT("PathFollow")), false);
	DebugMenuManager->AddNode_Bool(BaseDebugMenuPath + FString(TEXT("/Draw")), FString(TEXT("LastEQS")), false);
	DebugMenuManager->AddNode_Bool(BaseDebugMenuPath + FString(TEXT("/Draw")), FString(TEXT("BehaviorTree")), false);
	DebugMenuManager->AddNode_Bool(BaseDebugMenuPath + FString(TEXT("/Draw")), FString(TEXT("Perception")), false);
}

/**
 * @brief	Tick
 */
bool	UCSKitDebug_ActorSelectManager::DebugTick(float InDeltaSecond)
{
	const UCSKitDebug_DebugMenuManager* DebugMenuManager = UCSKitDebug_DebugMenuManager::sGet(this);
	mbActive = DebugMenuManager->GetNodeValue_Bool(FString(TEXT("CSKitDebug/ActorSelect/Active")));
	SetOnlyUpdateSelectActor(DebugMenuManager->GetNodeValue_Bool(FString(TEXT("CSKitDebug/ActorSelect/UpdateOnlySelectActor"))));
	mbShowInfo = DebugMenuManager->GetNodeValue_Bool(FString(TEXT("CSKitDebug/ActorSelect/Draw/Info")));
	mbShowMark = DebugMenuManager->GetNodeValue_Bool(FString(TEXT("CSKitDebug/ActorSelect/Draw/Mark")));
	mbShowSelectAxis = DebugMenuManager->GetNodeValue_Bool(FString(TEXT("CSKitDebug/ActorSelect/Draw/Axis")));
	mbShowSelectBone = DebugMenuManager->GetNodeValue_Bool(FString(TEXT("CSKitDebug/ActorSelect/Draw/Bone")));
	mbShowSelectPathFollow = DebugMenuManager->GetNodeValue_Bool(FString(TEXT("CSKitDebug/ActorSelect/Draw/PathFollow")));
	mbShowSelectLastEQS = DebugMenuManager->GetNodeValue_Bool(FString(TEXT("CSKitDebug/ActorSelect/Draw/LastEQS")));
	mbShowSelectBehaviorTree = DebugMenuManager->GetNodeValue_Bool(FString(TEXT("CSKitDebug/ActorSelect/Draw/BehaviorTree")));
	mbShowSelectPerception = DebugMenuManager->GetNodeValue_Bool(FString(TEXT("CSKitDebug/ActorSelect/Draw/Perception")));
	if (!mbActive)
	{
		return true;
	}
	CheckDebugCameraController();
	CheckSelectTarget();
	CheckAutoSelect();

	return true;
}

/**
 * @brief	Draw
 */
void	UCSKitDebug_ActorSelectManager::DebugDraw(UCanvas* InCanvas)
{
	if (!mbActive)
	{
		return;
	}
	DrawSelectMarker(InCanvas);

	if (mbShowInfo)
	{
		DrawInfo(InCanvas);
	}
	if (mbShowMark)
	{
		DrawMarkAllSelectList(InCanvas);
	}
	for (TWeakObjectPtr<UCSKitDebug_ActorSelectComponent> WeakPtr : mSelectList)
	{
		if (UCSKitDebug_ActorSelectComponent* DebugSelectComponent = WeakPtr.Get())
		{
			DebugSelectComponent->DebugDraw(InCanvas);
		}
	}
}

/**
 * @brief	UCSKitDebug_ActorSelectComponentの登録解除
 */
void	UCSKitDebug_ActorSelectManager::EntryDebugSelectComponent(UCSKitDebug_ActorSelectComponent* InComponent)
{
	mAllSelectList.Add(InComponent);

	if (AActor* Owner = InComponent->GetOwner())
	{
		SetActiveTickActor(Owner, !mbOnlyUpdateSelectActor);
	}
}
void	UCSKitDebug_ActorSelectManager::ExitDebugSelectComponent(UCSKitDebug_ActorSelectComponent* InComponent)
{
	mAllSelectList.RemoveSwap(InComponent);
}

/**
 * @brief	選択中かどうか
 */
bool UCSKitDebug_ActorSelectManager::IsSelected(const AActor& InActor)
{
	const UCSKitDebug_ActorSelectComponent* DebugSelectComponent = InActor.FindComponentByClass<UCSKitDebug_ActorSelectComponent>();
	if (DebugSelectComponent == nullptr)
	{
		return false;
	}

	return DebugSelectComponent->IsSelect();
}

/**
 * @brief	DebugCameraController監視
 */
void UCSKitDebug_ActorSelectManager::CheckDebugCameraController()
{
	if (mDebugCameraController.Get())
	{
		return;
	}

	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		if (ADebugCameraController* PlayerController = Cast<ADebugCameraController>(Iterator->Get()) )
		{
			mDebugCameraController = PlayerController;
			break;
		}
	}

#if 0//BindActionを使うほうが自然な気もするけど、プロジェクト設定でボタン変更したいのと、寿命の前後関係を気にしたくないので
	if (ADebugCameraController* DebugCameraController = mDebugCameraController.Get())
	{
		if (UInputComponent* InputComponent = DebugCameraController->InputComponent)
		{
			InputComponent->BindAction("DebugCamera_Select", IE_Pressed, this, &UCSKitDebug_ActorSelectManager::SelectTarget);
		}
		else
		{
			mDebugCameraController = nullptr;
		}
	}
#endif
}

/**
 * @brief	指定Actorに対して選択処理
 */
void UCSKitDebug_ActorSelectManager::RequestSelect(const AActor* InActor)
{
	OnSelect(InActor);
}

/**
 * @brief	自動選択するクラス
 */
void UCSKitDebug_ActorSelectManager::RequestAutoSelect(UClass* InTargetClass)
{
	if (const UCSKitDebug_ActorSelectComponent* ActorWatcher = mAutoSelectTarget.Get())
	{
		RequestSelect(ActorWatcher->GetOwner());
	}
	mAutoSelectTargetClass = InTargetClass;
}

/**
 * @brief	最後に選択したUCSKitDebug_ActorSelectComponent
 */
const UCSKitDebug_ActorSelectComponent* UCSKitDebug_ActorSelectManager::GetLastSelectTarget() const
{
	return mLastSelectTarget.Get();
}
/**
 * @brief	最後に選択したActor
 */
AActor* UCSKitDebug_ActorSelectManager::GetLastSelectTargetActor() const
{
	if(const UCSKitDebug_ActorSelectComponent* Component = GetLastSelectTarget())
	{
		return Component->GetOwner();
	}
	return nullptr;
}

/**
 * @brief	最後に選択したターゲットを見るカメラにする
 */
void UCSKitDebug_ActorSelectManager::SetLookMode(const bool bInLook)
{
	if(mbLookMode == bInLook)
	{
		return;
	}

	mbLookMode = bInLook;
	if(mbLookMode)
	{
		AActor* Target = GetLastSelectTargetActor();
		if(Target == nullptr)
		{
			return;
		}
		if (const UGameInstance* GameInstance = GetWorld()->GetGameInstance())
		{
			if (UCSKitDebug_Subsystem* CSKitDebugSubsystem = GameInstance->GetSubsystem<UCSKitDebug_Subsystem>())
			{
				CSKitDebugSubsystem->BeginGhostController(Target);
			}
		}
	}
	else
	{
		if (const UGameInstance* GameInstance = GetWorld()->GetGameInstance())
		{
			if (UCSKitDebug_Subsystem* CSKitDebugSubsystem = GameInstance->GetSubsystem<UCSKitDebug_Subsystem>())
			{
				CSKitDebugSubsystem->EndGhostController();
			}
		}
	}
}

/**
 * @brief	デバッグカメラ状態ならDebugCameraController取得
 */
ADebugCameraController* UCSKitDebug_ActorSelectManager::GetActiveDebugCameraController() const
{
	if(ADebugCameraController* DebugCameraController = mDebugCameraController.Get())
	{
		if (DebugCameraController->Player)
		{
			return DebugCameraController;
		}
	}

	return nullptr;
}

/**
 * @brief	対象選択監視
 */
void UCSKitDebug_ActorSelectManager::CheckSelectTarget()
{
	ADebugCameraController* DebugCameraController = GetActiveDebugCameraController();
	if (DebugCameraController == nullptr)
	{
		return;
	}

	const UPlayerInput* PlayerInput = DebugCameraController->PlayerInput;
	if (PlayerInput == nullptr)
	{
		return;
	}
	const UCSKitDebug_Config* CSKitDebugConfig = GetDefault<UCSKitDebug_Config>();
	if (CSKitDebugConfig->mDebugSelect_SelectKey.IsJustPressed(*PlayerInput))
	{
		FVector CamLoc;
		FRotator CamRot;
		DebugCameraController->GetPlayerViewPoint(CamLoc, CamRot);

		FHitResult HitResult;
		FCollisionQueryParams TraceParams(NAME_None, FCollisionQueryParams::GetUnknownStatId(), true, DebugCameraController);
		bool const bHit = DebugCameraController->GetWorld()->LineTraceSingleByChannel(HitResult, CamLoc, CamRot.Vector() * 5000.f * 20.f + CamLoc, ECC_Pawn, TraceParams);
		if (bHit)
		{
			if (AActor* HitActor = HitResult.Actor.Get())
			{
				OnSelect(HitActor);
			}
		}
	}
}

/**
 * @brief	指定クラスのActorで一番近いのを自動選択
 */
void UCSKitDebug_ActorSelectManager::CheckAutoSelect()
{
	if (mAutoSelectTarget.IsValid())
	{
		return;
	}
	UClass* TargetClass = mAutoSelectTargetClass.Get();
	if (TargetClass == nullptr)
	{
		return;
	}

	FVector BasePos = FVector::ZeroVector;
	if (const APlayerController* PlayerController = UCSKitDebug_Utility::FindMainPlayerController(GetWorld()))
	{
		if (const APlayerCameraManager* CameraManager = PlayerController->PlayerCameraManager)
		{
			BasePos = CameraManager->GetCameraLocation();
		}
	}

	TArray<AActor*> ActorList;
	UGameplayStatics::GetAllActorsOfClass(this, TargetClass, ActorList);
	const AActor* SelectActor = nullptr;
	float MinDistanceSq = FLT_MAX;
	for (const AActor* TargetActor : ActorList)
	{
		const float DistanceSq = FVector::DistSquared(BasePos, TargetActor->GetActorLocation());
		if (DistanceSq < MinDistanceSq)
		{
			MinDistanceSq = DistanceSq;
			SelectActor = TargetActor;
		}
	}

	if (SelectActor == nullptr)
	{
		return;
	}

	if (!IsSelected(*SelectActor))
	{
		RequestSelect(SelectActor);
	}
	if (UCSKitDebug_ActorSelectComponent* DebugSelectComponent = SelectActor->FindComponentByClass<UCSKitDebug_ActorSelectComponent>())
	{
		mAutoSelectTarget = DebugSelectComponent;
	}
}

/**
 * @brief	対象選択時
 */
void UCSKitDebug_ActorSelectManager::OnSelect(const AActor* InActor)
{
	UCSKitDebug_ActorSelectComponent* DebugSelectComponent = InActor->FindComponentByClass<UCSKitDebug_ActorSelectComponent>();
	if (DebugSelectComponent == nullptr)
	{
		return;
	}
	
	DebugSelectComponent->SetSelect(!DebugSelectComponent->IsSelect());

	if (DebugSelectComponent->IsSelect())
	{
		mSelectList.Add(DebugSelectComponent);

		if (AActor* Owner = DebugSelectComponent->GetOwner())
		{
			SetActiveTickActor(Owner, true);
		}
		mLastSelectTarget = DebugSelectComponent;
	}
	else
	{
		mSelectList.RemoveSwap(DebugSelectComponent);

		if (AActor* Owner = DebugSelectComponent->GetOwner())
		{
			SetActiveTickActor(Owner, !mbOnlyUpdateSelectActor);
		}
		if (DebugSelectComponent == mLastSelectTarget.Get())
		{
			mLastSelectTarget = nullptr;
		}
	}
}
/**
 * @brief	ActorのTickをon/off
 */
void UCSKitDebug_ActorSelectManager::SetActiveTickActor(AActor* InActor, const bool bInActive)
{
	InActor->SetActorTickEnabled(bInActive);
	for (auto Element : InActor->GetComponents())
	{
		Element->SetActive(bInActive);
	}

	if (APawn* Pawn = Cast<APawn>(InActor))
	{
		if (AController* Controller = Pawn->Controller)
		{
			SetActiveTickActor(Controller, bInActive);
		}
	}
}

/**
 * @brief	全体情報表示
 */
void	UCSKitDebug_ActorSelectManager::DrawInfo(UCanvas* InCanvas) const
{
	FCSKitDebug_ScreenWindowText ScreenWindow;
	ScreenWindow.SetWindowName(FString(TEXT("CSKitDebug_ActorSelectManager")));
	ScreenWindow.AddText(FString::Printf(TEXT("AllSelectListNum : %d"), mAllSelectList.Num()));
	ScreenWindow.AddText(FString::Printf(TEXT("SelectListNum : %d"), mSelectList.Num()));
	ScreenWindow.Draw(InCanvas, FVector2D(InCanvas->ClipX*0.5f,50.f));
}
/**
 * @brief	画面中央の選択マーク
 */
void	UCSKitDebug_ActorSelectManager::DrawSelectMarker(UCanvas* InCanvas) const
{
	ADebugCameraController* DebugCameraController = GetActiveDebugCameraController();
	if (DebugCameraController == nullptr)
	{
		return;
	}

	FVector CamLoc;
	FRotator CamRot;
	DebugCameraController->GetPlayerViewPoint(CamLoc, CamRot);
	const FVector CenterPos(CamLoc + CamRot.Vector() * 5000.f * 20.f);
	//const FVector2D CenterPos2D(InCanvas->SizeX * 0.5f, InCanvas->SizeY * 0.5f);
	const FVector2D CenterPos2D( InCanvas->Project(CenterPos) );
	constexpr float ExtentLen = 10.f;
	const FColor Color = FColor::Red;
	{
		FCanvasLineItem Item(FVector2D(CenterPos2D + FVector2D(ExtentLen)), FVector2D(CenterPos2D - FVector2D(ExtentLen)));
		Item.SetColor(Color);
		InCanvas->DrawItem(Item);
	}
	{
		FCanvasLineItem Item(FVector2D(CenterPos2D + FVector2D(ExtentLen,-ExtentLen)), FVector2D(CenterPos2D + FVector2D(-ExtentLen,ExtentLen)));
		Item.SetColor(Color);
		InCanvas->DrawItem(Item);
	}
}

/**
 * @brief	選択対象のマーク表示
 */
void	UCSKitDebug_ActorSelectManager::DrawMarkAllSelectList(UCanvas* InCanvas)
{
	for (TWeakObjectPtr<UCSKitDebug_ActorSelectComponent> WeakPtr : mAllSelectList)
	{
		if (const UCSKitDebug_ActorSelectComponent* DebugSelectComponent = WeakPtr.Get())
		{
			if (DebugSelectComponent->IsSelect())
			{
				continue;
			}
			DebugSelectComponent->DrawMark(InCanvas);
		}
	}
}

void	UCSKitDebug_ActorSelectManager::SetOnlyUpdateSelectActor(const bool bInOnlyUpdate)
{
	if (bInOnlyUpdate == mbOnlyUpdateSelectActor)
	{
		return;
	}
	mbOnlyUpdateSelectActor = bInOnlyUpdate;

	for (TWeakObjectPtr<UCSKitDebug_ActorSelectComponent> WeakPtr : mAllSelectList)
	{
		if (UCSKitDebug_ActorSelectComponent* DebugSelectComponent = WeakPtr.Get())
		{
			if (AActor* Owner = DebugSelectComponent->GetOwner())
			{
				const bool bActive = (!mbOnlyUpdateSelectActor || DebugSelectComponent->IsSelect());
				SetActiveTickActor(Owner, bActive);
			}
		}
	}
}

#endif//USE_CSKIT_DEBUG