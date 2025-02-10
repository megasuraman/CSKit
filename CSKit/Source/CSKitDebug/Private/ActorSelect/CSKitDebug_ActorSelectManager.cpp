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
void	UCSKitDebug_ActorSelectManager::Init()
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
 * @brief	対象選択時
 */
void UCSKitDebug_ActorSelectManager::OnSelect(AActor* InActor)
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
	}
	else
	{
		mSelectList.RemoveSwap(DebugSelectComponent);

		if (AActor* Owner = DebugSelectComponent->GetOwner())
		{
			SetActiveTickActor(Owner, !mbOnlyUpdateSelectActor);
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
void	UCSKitDebug_ActorSelectManager::DrawInfo(UCanvas* InCanvas)
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
void	UCSKitDebug_ActorSelectManager::DrawSelectMarker(UCanvas* InCanvas)
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
	const float ExtentLen = 10.f;
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