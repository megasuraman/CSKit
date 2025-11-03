// Copyright 2022 megasuraman
/**
 * @file CSKitEditor_EUW_Base.cpp
 * @brief ActorSelector操作EUW
 * @author megasuraman
 * @date 2023/03/30
 */
#include "EditorUtilityWidget/CSKitEditor_EUW_ActorSelect.h"

#include "ActorSelect/CSKitDebug_ActorSelectManager.h"
#include "CSKitDebug_Subsystem.h"
#include "DebugMenu/CSKitDebug_DebugMenuManager.h"
#include "Engine/DebugCameraController.h"

/**
 * @brief	
 */
void UCSKitEditor_EUW_ActorSelect::PostInitProperties()
{
	Super::PostInitProperties();
	
	const FCSKitDebug_SaveData& SaveData = UCSKitDebug_Subsystem::sGetSaveData();
	mbActive = SaveData.GetBool(FString(TEXT("EUW_ActorSelect.mbActive")));
	mbAutoSelect = SaveData.GetBool(FString(TEXT("EUW_ActorSelect.mbAutoSelect")));
	mbOnlyUpdateSelectActor = SaveData.GetBool(FString(TEXT("EUW_ActorSelect.mbOnlyUpdateSelectActor")));
	const FString TargetAssetClassPath = SaveData.GetString(FString(TEXT("EUW_ActorSelect.mTargetClass")));
	if (!TargetAssetClassPath.IsEmpty())
	{
		mTargetClass = TSoftObjectPtr<UObject>( FSoftObjectPath(TargetAssetClassPath)).LoadSynchronous();
	}

	mBookmarkClass.Empty();
	const FString BookmarkClassString = SaveData.GetString(FString(TEXT("EUW_ActorSelect.mBookmarkClass")));
	TArray<FString> BookmarkClassStringList;
	BookmarkClassString.ParseIntoArray(BookmarkClassStringList, TEXT("/"));
	for (const FString& ClassString : BookmarkClassStringList)
	{
		if (TSoftObjectPtr<UObject> ClassPtr = TSoftObjectPtr<UObject>( FSoftObjectPath(ClassString)).LoadSynchronous())
		{
			mBookmarkClass.Add(ClassPtr->GetClass());
		}
	}
}

/**
 * @brief	
 */
void UCSKitEditor_EUW_ActorSelect::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	FCSKitDebug_SaveData& SaveData = UCSKitDebug_Subsystem::sGetSaveData();
	SaveData.SetBool(FString(TEXT("EUW_ActorSelect.mbActive")), mbActive);
	SaveData.SetBool(FString(TEXT("EUW_ActorSelect.mbAutoSelect")), mbAutoSelect);
	SaveData.SetBool(FString(TEXT("EUW_ActorSelect.mbOnlyUpdateSelectActor")), mbOnlyUpdateSelectActor);
	if (mTargetClass != nullptr)
	{
		SaveData.SetString(FString(TEXT("EUW_ActorSelect.mTargetClass")), mTargetClass->GetPathName());
	}
	FString BookmarkClassString;
	for (const TSoftClassPtr<UObject>& ClassPtr : mBookmarkClass)
	{
		if (ClassPtr.IsValid())
		{
			BookmarkClassString += FString::Printf(TEXT("%s/"), *ClassPtr->GetPathName());
		}
	}
	SaveData.SetString(FString(TEXT("EUW_ActorSelect.mBookmarkClass")), BookmarkClassString);
}

/**
 * @brief	
 */
bool	UCSKitEditor_EUW_ActorSelect::SetupTargetObjectList()
{
	mTargetObjectList.Empty();

	const UWorld* World = GetWorld_GameServer();
	const UWorld* SecondWorld = GetWorld_GameClient();

	TArray<UObject*> ObjectList;
	GetObjectsOfClass(mTargetClass->GetClass(), ObjectList);
	for (const UObject* Object : ObjectList)
	{
		if (Object->GetWorld() == World
			|| Object->GetWorld() == SecondWorld)
		{
			mTargetObjectList.Add(Object);
		}
	}

	return mTargetObjectList.Num() > 0;
}

/**
 * @brief	
 */
FString	UCSKitEditor_EUW_ActorSelect::GetDrawObjectName(UObject* InTarget) const
{
	if (InTarget)
	{
		FString DrawName = FString::Printf(TEXT("%s(%d)"), *InTarget->GetName(), InTarget->GetUniqueID());
		const UWorld* OwnWorld = InTarget->GetWorld();
		if (const UActorComponent* TargetComponent = Cast<UActorComponent>(InTarget))
		{
			if (const AActor* OwnerActor = TargetComponent->GetOwner())
			{
				DrawName += FString::Printf(TEXT("[%s]"), *OwnerActor->GetName());
				OwnWorld = OwnerActor->GetWorld();
			}
		}
		if (OwnWorld)
		{
			if (InTarget->GetWorld()->WorldType == EWorldType::Editor)
			{
				DrawName += FString(TEXT("[Editor]"));
			}
			else if (InTarget->GetWorld()->GetNetMode() == ENetMode::NM_DedicatedServer)
			{
				DrawName += FString(TEXT("[DGS]"));
			}
			else if (InTarget->GetWorld()->GetNetMode() == ENetMode::NM_Client)
			{
				DrawName += FString(TEXT("[Client]"));
			}
		}
		return DrawName;
	}
	return FString(TEXT("None"));
}

/**
 * @brief	
 */
void	UCSKitEditor_EUW_ActorSelect::Clear()
{
	mTargetClass = nullptr;
	mTargetObjectList.Empty();
	mTargetObject.Reset();
}

/**
 * @brief	
 */
void UCSKitEditor_EUW_ActorSelect::SelectActorSelector()
{
	const AActor* TargetActor = Cast<AActor>(mTargetObject.Get());
	if (TargetActor == nullptr)
	{
		return;
	}

	const UWorld* TargetWorld = TargetActor->GetWorld();
	if (TargetWorld == nullptr)
	{
		return;
	}

	UCSKitDebug_ActorSelectManager* ActorSelectManager = GetActorSelectorManager(*TargetWorld);
	if (ActorSelectManager == nullptr)
	{
		return;
	}

	if (UCSKitDebug_DebugMenuManager* DebugMenuManager = UCSKitDebug_DebugMenuManager::sGet(this))
	{
		DebugMenuManager->SetNodeValue_Bool(FString(TEXT("CSKitDebug/ActorSelect/Active")), true);
	}
	
	ActorSelectManager->RequestSelect(TargetActor);
	ActorSelectManager->RequestSetOnlyUpdateSelectActor(mbOnlyUpdateSelectActor);
}

/**
 * @brief	
 */
void UCSKitEditor_EUW_ActorSelect::SetAutoSelect(bool bInAutoSelect)
{
	if (mbAutoSelect == bInAutoSelect)
	{
		return;
	}
	mbAutoSelect = bInAutoSelect;

	if (mTargetClass == nullptr)
	{
		return;
	}

	if (const UWorld* TargetWorld = GetWorld_GameServer())
	{
		if (UCSKitDebug_ActorSelectManager* ActorSelectorManager = GetActorSelectorManager(*TargetWorld))
		{
			ActorSelectorManager->RequestAutoSelect(mTargetClass->GetClass());
		}
	}
}

/**
 * @brief	
 */
void UCSKitEditor_EUW_ActorSelect::SetOnlyUpdateSelectActor(bool bInOnlyUpdate)
{
	if (mbOnlyUpdateSelectActor == bInOnlyUpdate)
	{
		return;
	}
	mbOnlyUpdateSelectActor = bInOnlyUpdate;

	if (const UWorld* TargetWorld = GetWorld_GameServer())
	{
		if (UCSKitDebug_ActorSelectManager* ActorSelectorManager = GetActorSelectorManager(*TargetWorld))
		{
			ActorSelectorManager->RequestSetOnlyUpdateSelectActor(mbOnlyUpdateSelectActor);
		}
	}
}

/**
 * @brief	
 */
// void UCSKitEditor_EUW_ActorSelector::ClearDrawFlagMap()
// {
// 	mDrawFlagMap.Empty();
// }

/**
 * @brief	
 */
bool UCSKitEditor_EUW_ActorSelect::IsDrawFlag(FName InName) const
{
	if (const bool* Flag = mDrawFlagMap.Find(InName))
	{
		return *Flag;
	}
	return false;
}

/**
 * @brief	
 */
void UCSKitEditor_EUW_ActorSelect::SetLookMode(bool bInLook)
{
	const UWorld* World = GetWorld_GameServer();
	if(World == nullptr)
	{
		return;
	}
	UCSKitDebug_ActorSelectManager* ActorSelectorManager = GetActorSelectorManager(*World);
	if (ActorSelectorManager == nullptr)
	{
		return;
	}
	ActorSelectorManager->SetLookMode(bInLook);
}

/**
 * @brief	最後に選択した対象をプレイヤー正面か、デバッグカメラ位置へワープ
 */
void UCSKitEditor_EUW_ActorSelect::LastSelectTargetWarp()
{
	const UWorld* World = GetWorld_GameServer();
	if(World == nullptr)
	{
		return;
	}
	UCSKitDebug_ActorSelectManager* ActorSelectorManager = GetActorSelectorManager(*World);
	if (ActorSelectorManager == nullptr)
	{
		return;
	}
	AActor* LastSelectActor = ActorSelectorManager->GetLastSelectTargetActor();
	if(LastSelectActor == nullptr)
	{
		return;
	}

	APlayerController* MainPlayerController = nullptr;
	for (FConstPlayerControllerIterator Iterator = World->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		if (APlayerController* PlayerController = Iterator->Get())
		{
			if (PlayerController->Player)
			{
				MainPlayerController = PlayerController;
			}
		}
	}
	if(MainPlayerController == nullptr)
	{
		return;
	}

	FVector WarpPos = FVector::ZeroVector;
	if(const APawn* Pawn = MainPlayerController->GetPawn())
	{
		WarpPos = Pawn->GetActorLocation() + Pawn->GetActorRotation().RotateVector(FVector(100.f,0.f,0.f));
	}
	if (const ADebugCameraController* DebugCameraController = Cast<ADebugCameraController>(MainPlayerController))
	{
		FVector CamLoc;
		FRotator CamRot;
		DebugCameraController->GetPlayerViewPoint(CamLoc, CamRot);
	
		FVector CameraTipPos = CamLoc + CamRot.RotateVector(FVector(3000.f,0.f,0.f));
		FHitResult HitResult;
		if (GetWorld()->LineTraceSingleByChannel(
			HitResult,
			CamLoc,
			CameraTipPos,
			ECC_WorldStatic)
			)
		{
			CameraTipPos = HitResult.ImpactPoint;
		}
		WarpPos = CameraTipPos;
	}
	LastSelectActor->SetActorLocation(WarpPos);
}

/**
 * @brief	ゲーム実行開始時処理
 */
void UCSKitEditor_EUW_ActorSelect::OnRunGame(const UWorld& InWorld)
{
	Super::OnRunGame(InWorld);
	
	UCSKitDebug_ActorSelectManager* ActorSelectorManager = GetActorSelectorManager(InWorld);
	if (ActorSelectorManager == nullptr)
	{
		return;
	}
	if (UCSKitDebug_DebugMenuManager* DebugMenuManager = UCSKitDebug_DebugMenuManager::sGet(this))
	{
		DebugMenuManager->SetNodeValue_Bool(FString(TEXT("CSKitDebug/ActorSelect/Active")), true);
	}

	if (mbAutoSelect
		&& mTargetClass != nullptr)
	{
		ActorSelectorManager->RequestAutoSelect(mTargetClass->GetClass());
	}
	ActorSelectorManager->RequestSetOnlyUpdateSelectActor(mbOnlyUpdateSelectActor);
}

/**
 * @brief	
 */
UCSKitDebug_ActorSelectManager* UCSKitEditor_EUW_ActorSelect::GetActorSelectorManager(const UWorld& InWorld)
{
	const UGameInstance* GameInstance = InWorld.GetGameInstance();
	if (GameInstance == nullptr)
	{
		return nullptr;
	}
	const UCSKitDebug_Subsystem* CSKitEditorSubsystem = GameInstance->GetSubsystem<UCSKitDebug_Subsystem>();
	if (CSKitEditorSubsystem == nullptr)
	{
		return nullptr;
	}
	return CSKitEditorSubsystem->GetActorSelectManager();
}