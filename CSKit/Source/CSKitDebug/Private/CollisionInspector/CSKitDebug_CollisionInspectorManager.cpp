// Copyright 2020 megasuraman
/**
 * @file CSKitDebug_CollisionInspectorManager.cpp
 * @brief コリジョンチェック管理クラス
 * @author megasuraman
 * @date 2025/12/12
 */

#include "CollisionInspector/CSKitDebug_CollisionInspectorManager.h"

#include "CSKitDebug_Subsystem.h"
#include "CollisionInspector/CSKitDebug_CollisionInspectorComponent.h"
#include "CollisionInspector/CSKitDebug_CollisionInspectorError.h"
#include "DebugMenu/CSKitDebug_DebugMenuManager.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/FileHelper.h"

UCSKitDebug_CollisionInspectorManager* UCSKitDebug_CollisionInspectorManager::sGet(const UWorld* InWorld)
{
	if (InWorld == nullptr)
	{
		return nullptr;
	}
	if (const UGameInstance* GameInstance = InWorld->GetGameInstance())
	{
		if (UCSKitDebug_Subsystem* DebugSubsystem = GameInstance->GetSubsystem<UCSKitDebug_Subsystem>())
		{
			return DebugSubsystem->GetCollisionInspectorManager();
		}
	}
	return nullptr;
}

/**
 * @brief	
 */
void UCSKitDebug_CollisionInspectorManager::Init()
{
	UCSKitDebug_DebugMenuManager* DebugMenuManager = UCSKitDebug_DebugMenuManager::sGet(this);

	const FString BaseDebugMenuPath(TEXT("CSKitDebug/CollisionInspector"));
	DebugMenuManager->AddNode_Bool(BaseDebugMenuPath, FString(TEXT("AutoAttachPlayer")), false);
	DebugMenuManager->AddNode_Bool(BaseDebugMenuPath, FString(TEXT("DrawLastSafePoint")), false);
	DebugMenuManager->AddNode_Bool(BaseDebugMenuPath, FString(TEXT("DrawVoxel")), false);
	DebugMenuManager->AddNode_Bool(BaseDebugMenuPath, FString(TEXT("DrawError")), false);
}

/**
 * @brief	
 */
void UCSKitDebug_CollisionInspectorManager::Entry(UCSKitDebug_CollisionInspectorComponent* InComponent)
{
	mComponentList.AddUnique(InComponent);
}

/**
 * @brief	
 */
void UCSKitDebug_CollisionInspectorManager::Exit(UCSKitDebug_CollisionInspectorComponent* InComponent)
{
	mComponentList.Remove(InComponent);
}

/**
 * @brief	
 */
void UCSKitDebug_CollisionInspectorManager::DebugTick(const float InDeltaSec)
{
	if (UCSKitDebug_DebugMenuManager::sGetNodeValue_Bool(this, FString(TEXT("CSKitDebug/CollisionInspector/AutoAttachPlayer"))))
	{
		AttachComponentToPlayer();
	}
}

/**
 * @brief	
 */
void UCSKitDebug_CollisionInspectorManager::DebugDraw(UCanvas* InCanvas)
{
	for (const auto& WeakPtr : mComponentList)
	{
		if (const UCSKitDebug_CollisionInspectorComponent* CollisionInspectorComponent = WeakPtr.Get())
		{
			CollisionInspectorComponent->DebugDraw(InCanvas);
		}
	}
}

/**
 * @brief	プレイヤーにUCSKitDebug_CollisionInspectorComponentをつける
 */
void UCSKitDebug_CollisionInspectorManager::AttachComponentToPlayer() const
{
	if (APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0))
	{
		if (PlayerPawn->FindComponentByClass<UCSKitDebug_CollisionInspectorComponent>() != nullptr)
		{//既にAttach済み
			return;
		}
		if (UCSKitDebug_CollisionInspectorComponent* CollisionInspectorComponent = NewObject<UCSKitDebug_CollisionInspectorComponent>(PlayerPawn, TEXT("CollisionInspector")))
		{
			CollisionInspectorComponent->RegisterComponent();
		}
	}
}

/**
 * @brief	プレイヤーからUCSKitDebug_CollisionInspectorComponentを外す
 */
void UCSKitDebug_CollisionInspectorManager::DetachComponentToPlayer() const
{
	if (APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0))
	{
		if (UCSKitDebug_CollisionInspectorComponent* CollisionInspectorComponent = PlayerPawn->FindComponentByClass<UCSKitDebug_CollisionInspectorComponent>())
		{
			CollisionInspectorComponent->UnregisterComponent();
			CollisionInspectorComponent->DestroyComponent();
		}
	}
}

/**
 * @brief	エラー結果出力
 */
void UCSKitDebug_CollisionInspectorManager::OutputErrorFile() const
{
	FCollisionInspectorError AllError;
	for (const auto& WeakPtr : mComponentList)
	{
		if (const UCSKitDebug_CollisionInspectorComponent* CollisionInspectorComponent = WeakPtr.Get())
		{
			AllError.Merge(CollisionInspectorComponent->GetError());
		}
	}
	
	FString FilePath = FPaths::ConvertRelativePathToFull(FPaths::ProjectSavedDir());
	const FString LevelName = UGameplayStatics::GetCurrentLevelName(GetWorld());
	FilePath += FString::Printf(TEXT("CollisionInspector/%s.txt"), *LevelName);
	FFileHelper::SaveStringToFile(AllError.ToJson(), *FilePath, FFileHelper::EEncodingOptions::ForceUTF8);
}