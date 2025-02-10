// Copyright 2022 megasuraman
#include "EditorUtilityWidget/CSKitEditor_EUW_Base.h"

#include "AssetToolsModule.h"
#include "IAssetTools.h"
#include "Debug/DebugDrawService.h"
#include "LevelEditor.h"
#include "Editor/UnrealEdEngine.h"
#include "EditorLevelUtils.h"
#include "Editor.h"
#include "UnrealEdGlobals.h"
#include "SLevelViewport.h"
#include "AssetRegistry/AssetRegistryModule.h"


void UCSKitEditor_EUW_Base::NativeDestruct()
{
	Super::NativeDestruct();
	SetActiveDraw(false);
}

/**
 * @brief	EditorのWorld取得
 */
UWorld* UCSKitEditor_EUW_Base::GetWorld_Editor() const
{
	const TIndirectArray<FWorldContext>& WorldContextList = GUnrealEd->GetWorldContexts();
	for (auto& WorldContext : WorldContextList)
	{
		if (WorldContext.WorldType == EWorldType::Editor)
		{
			return WorldContext.World();
		}
	}

	return nullptr;
}

/**
 * @brief	GameでServer(DGS or Standalone)のWorld取得
 */
UWorld* UCSKitEditor_EUW_Base::GetWorld_GameServer() const
{
	const TIndirectArray<FWorldContext>& WorldContextList = GUnrealEd->GetWorldContexts();
	for (auto& WorldContext : WorldContextList)
	{
		if (WorldContext.WorldType == EWorldType::PIE)
		{
			if (UWorld* World = WorldContext.World())
			{
				if (!World->IsNetMode(NM_Client))
				{
					return World;
				}
			}
		}
	}

	return nullptr;
}

/**
 * @brief	GameでClientのWorld取得
 */
UWorld* UCSKitEditor_EUW_Base::GetWorld_GameClient() const
{
	const TIndirectArray<FWorldContext>& WorldContextList = GUnrealEd->GetWorldContexts();
	for (auto& WorldContext : WorldContextList)
	{
		if (WorldContext.WorldType == EWorldType::PIE)
		{
			if (UWorld* World = WorldContext.World())
			{
				if (World->IsNetMode(NM_Client))
				{
					return World;
				}
			}
		}
	}

	return nullptr;
}

/**
 * @brief	指定のコマンドライン引数が指定されてるかどうか
 */
bool UCSKitEditor_EUW_Base::ParseCommandLine(FString& OutValue, const FString& InKey) const
{
	if( FParse::Value(FCommandLine::Get(), *InKey, OutValue) )
	{
		return true;
	}
	return false;
}

/**
 * @brief	デバッグ表示on/off
 */
void UCSKitEditor_EUW_Base::SetActiveDraw(const bool bInDraw)
{
	if (bInDraw)
	{
		if (!mDebugDrawHandle.IsValid())
		{
			const FDebugDrawDelegate DrawDebugDelegate = FDebugDrawDelegate::CreateUObject(this, &UCSKitEditor_EUW_Base::Draw);
			mDebugDrawHandle = UDebugDrawService::Register(TEXT("GameplayDebug"), DrawDebugDelegate);

			SetRealTimeDraw_LevelEditorViewport(true);
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
 * @brief	エディタのAutoSaveフラグを切る
 */
void UCSKitEditor_EUW_Base::OffAutoSaveFlag()
{
	if(UEditorLoadingSavingSettings* LoadingSavingSettings = GetMutableDefault<UEditorLoadingSavingSettings>())
	{
		if(LoadingSavingSettings->bAutoSaveEnable)
		{
			mbNeedRecoverAutoSaveFlag = true;
			LoadingSavingSettings->bAutoSaveEnable = false;
		}
		else
		{
			mbNeedRecoverAutoSaveFlag = false;
		}
	}
}

/**
 * @brief	エディタのAutoSaveフラグを元に戻す
 */
void UCSKitEditor_EUW_Base::RecoverAutoSaveFlag()
{
	if(mbNeedRecoverAutoSaveFlag)
	{
		if(UEditorLoadingSavingSettings* LoadingSavingSettings = GetMutableDefault<UEditorLoadingSavingSettings>())
		{
			LoadingSavingSettings->bAutoSaveEnable = true;
		}
		mbNeedRecoverAutoSaveFlag = false;
	}
}

/**
 * @brief	指定されたアセット名のリダイレクタの解消
 */
void UCSKitEditor_EUW_Base::FixupRedirects(const TArray<FString>& InTargetAssetNameList)
{
#if 0
	const FAssetRegistryModule* AssetRegistryModule = &FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");

	TArray<UObjectRedirector*> TargetRedirectorList;
	TArray<FAssetData> RedirectorAssetDataList;
	AssetRegistryModule->Get().GetAssetsByClass(UObjectRedirector::StaticClass()->GetClassPathName(), RedirectorAssetDataList);
	
	if (RedirectorAssetDataList.Num() == 0)
	{
		return;
	}
	for (FAssetData& AssetData : RedirectorAssetDataList)
	{
		UObjectRedirector* Redirector = Cast<UObjectRedirector>(AssetData.GetAsset());
		if (Redirector == nullptr
			|| Redirector->DestinationObject == nullptr)
		{
			continue;
		}

		const FString& RedirectorName = Redirector->GetName();
		if(InTargetAssetNameList.Find(RedirectorName) != INDEX_NONE)
		{
			TargetRedirectorList.Add(Redirector);
		}
	}

	if (TargetRedirectorList.Num() == 0)
	{
		return;
	}

	const IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
	AssetTools.FixupReferencers(TargetRedirectorList);
#endif
}

/**
 * @brief	ViewportのRealTime表示のon/off
 */
void UCSKitEditor_EUW_Base::SetRealTimeDraw_LevelEditorViewport(const bool bInActive) const
{
	FLevelEditorModule& LevelEditorModule = FModuleManager::GetModuleChecked<FLevelEditorModule>("LevelEditor");
	const TSharedPtr<SLevelViewport> ActiveLevelViewport = LevelEditorModule.GetFirstActiveLevelViewport();
	if (ActiveLevelViewport.IsValid())
	{
		FLevelEditorViewportClient& LevelViewportClient = ActiveLevelViewport->GetLevelViewportClient();
		LevelViewportClient.RemoveRealtimeOverride(FText::FromString(FString(TEXT("Remote Desktop"))), false);
		LevelViewportClient.SetRealtime(bInActive);
	}
}

/**
 * @brief	EditorViewportのカメラを指定位置へ向ける
 */
void UCSKitEditor_EUW_Base::LookAtPos(FVector InPos)
{
	GCurrentLevelEditingViewportClient->SetViewLocation(InPos + FVector(500.f));
	GCurrentLevelEditingViewportClient->SetLookAtLocation(InPos, true);
}

/**
 * @brief	エディタでロードしてるサブレベルを削除
 */
void UCSKitEditor_EUW_Base::ClearAllSubLevel()
{
	const UWorld* EditorWorld = GetWorld_Editor();
	const TArray<ULevel*> Levels = EditorWorld->GetLevels();
	TArray<ULevel*> SubLevelList;
	for (ULevel* Level : Levels)
	{
		if(Level == nullptr
			|| Level->IsPersistentLevel())
		{
			continue;
		}
		SubLevelList.Add(Level);
	}
	if(SubLevelList.Num() > 0)
	{
#if 0
		EditorLevelUtils::RemoveLevelsFromWorld(SubLevelList);
#else
		for (ULevel* Level : SubLevelList)
		{
			EditorLevelUtils::RemoveLevelFromWorld(Level);
		}
#endif
	}
}

/**
 * @brief	NativeTick呼べないのでDrawを利用したTick
 *			レベル操作等は危険
 */
void UCSKitEditor_EUW_Base::FakeTick()
{
	const UWorld* World = GetWorld_GameClient();
	if(World == nullptr)
	{
		World = GetWorld_GameServer();
	}
	if (World == nullptr)
	{
		mbRunGameWorld = false;
		return;
	}

	if (!mbRunGameWorld)
	{
		mbRunGameWorld = true;
		OnRunGame(*World);
	}
}

/**
 * @brief	デバッグ表示
 */
void UCSKitEditor_EUW_Base::Draw(UCanvas* InCanvas, APlayerController* InPlayerController)
{
	FakeTick();
}