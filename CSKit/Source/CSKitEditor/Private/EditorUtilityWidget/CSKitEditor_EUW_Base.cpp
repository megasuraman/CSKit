// Copyright 2022 megasuraman
#include "EditorUtilityWidget/CSKitEditor_EUW_Base.h"


#include "CSKit_SubLevelPresetTableRow.h"
#include "Debug/DebugDrawService.h"
#include "LevelEditor.h"
#include "Editor/UnrealEdEngine.h"
#include "EditorLevelUtils.h"
#include "Editor.h"
#include "FileHelpers.h"
#include "ISourceControlModule.h"
#include "UnrealEdGlobals.h"
#include "SLevelViewport.h"
#include "SourceControlOperations.h"
#include "Engine/LevelStreamingDynamic.h"


void UCSKitEditor_EUW_Base::NativeDestruct()
{
	Super::NativeDestruct();
	SetActiveDraw(false);
	EndAutoRunTickObject();
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
 * @brief	指定アセットをセーブ
 */
bool UCSKitEditor_EUW_Base::SaveAsset(UObject* InAsset)
{
	if (InAsset == nullptr)
	{
		return false;
	}
	UPackage* AssetPackage = InAsset->GetOutermost();
	if (AssetPackage == nullptr)
	{
		return false;
	}
	// CheckOut
	ISourceControlProvider& SourceControlProvider = ISourceControlModule::Get().GetProvider();
	const FSourceControlStatePtr SourceControlState = SourceControlProvider.GetState(AssetPackage, EStateCacheUsage::ForceUpdate);
	if (SourceControlState.IsValid())
	{
		if (!SourceControlState->CanCheckout())
		{
			return false;
		}
		const ECommandResult::Type Result = SourceControlProvider.Execute(ISourceControlOperation::Create<FCheckOut>(), AssetPackage);
		if (Result != ECommandResult::Succeeded)
		{
			return false;
		}
	}

	// Save
	TArray<UPackage*> PackagesToSave;
	PackagesToSave.Add(AssetPackage);
	const FEditorFileUtils::EPromptReturnCode PromptReturnCode = FEditorFileUtils::PromptForCheckoutAndSave(PackagesToSave, false, false, nullptr, true);
	if (PromptReturnCode != FEditorFileUtils::PR_Success)
	{
		return false;
	}
	return true;
}

bool UCSKitEditor_EUW_Base::SaveLevel(ULevel* InLevel)
{
	if (InLevel == nullptr)
	{
		return false;
	}
	UPackage* Package = InLevel->GetOutermost();
	if (Package == nullptr)
	{
		return false;
	}
	FString AssetPath, AssetFName, AssetExt;
	FPaths::Split(InLevel->GetPathName(), AssetPath, AssetFName, AssetExt);
	const FString FileName = FPackageName::LongPackageNameToFilename(AssetPath / AssetFName, FString(TEXT(".umap")));

	// check out
	ISourceControlProvider& SourceControlProvider = ISourceControlModule::Get().GetProvider();
	const FSourceControlStatePtr SourceControlState = SourceControlProvider.GetState(*FileName, EStateCacheUsage::ForceUpdate);
	if (SourceControlState.IsValid())
	{
		if (SourceControlState->CanCheckout())
		{
			const ECommandResult::Type Result = SourceControlProvider.Execute(ISourceControlOperation::Create<FCheckOut>(), *FileName);
			if (Result != ECommandResult::Succeeded)
			{
				return false;
			}
		}
	}

	bool bSaveResult = false;

	const FFileStatData FileStat = IFileManager::Get().GetStatData(*FileName);
	if (FileStat.bIsValid
		&& FileStat.bIsReadOnly
		)
	{	// 読み込み専用の場合は保存できない（CheckOutしてあれば外れてるはず） //
		UE_LOG(LogTemp, Warning, TEXT("File is read-only [%s]."), *FileName);
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Saved [%s]."), *FileName);
		// SavePackage
		Package->SetDirtyFlag(true);
#if 0
		FSavePackageArgs SavePackageArgs;
		SavePackageArgs.TopLevelFlags = EObjectFlags::RF_Standalone;
		bSaveResult = GEditor->SavePackage(Package, nullptr, *FileName, SavePackageArgs);
#else
		bSaveResult = GEditor->SavePackage(Package, nullptr, EObjectFlags::RF_Standalone, *FileName);
#endif
	}

	return bSaveResult;
}

/**
 * @brief	AutoRun開始
 */
void UCSKitEditor_EUW_Base::BeginAutoRun()
{
	BeginAutoRunTickObject(FCSKitEditor_EUW_TickEvent::CreateUObject(this, &UCSKitEditor_EUW_Base::UpdateAutoRun));
}
/**
 * @brief	AutoRun開始
 */
void UCSKitEditor_EUW_Base::EndAutoRun()
{
	EndAutoRunTickObject();
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

/**
 * @brief	/Game/Hoge.Hoge をフルパスに変換
 */
FString UCSKitEditor_EUW_Base::GetAssetFullPath(const FString& InAssetPath)
{
	FString AssetFullPath;
	FString TempString;
	InAssetPath.Split(TEXT("."), &AssetFullPath, &TempString, ESearchCase::IgnoreCase, ESearchDir::FromEnd);
	AssetFullPath += FString(TEXT(".uasset"));
	AssetFullPath = AssetFullPath.Replace(TEXT("/Game"), *FPaths::ProjectContentDir());
	AssetFullPath = FPaths::ConvertRelativePathToFull(AssetFullPath);
	return AssetFullPath;
}

/**
 * @brief	ファイルのタイムスタンプ取得
 */
int64 UCSKitEditor_EUW_Base::GetFileTimeStampSecond(const FString& InAssetFullPath)
{
#if 0
	std::filesystem::file_time_type FileTime = std::filesystem::last_write_time(*InAssetFullPath);
	const auto FileTimeSecond = std::chrono::duration_cast<std::chrono::seconds>(FileTime.time_since_epoch());
	return FileTimeSecond.count();
#else
	return 0;
#endif
}

/**
 * @brief	自動実行用のTickObject開始
 */
void UCSKitEditor_EUW_Base::BeginAutoRunTickObject(const FCSKitEditor_EUW_TickEvent& InDelegate)
{
	if (mAutoRunTickObject == nullptr)
	{
		mAutoRunTickObject = NewObject<UCSKitEditor_EUW_TickObjectBase>(GetWorld_Editor());
	}

	if (mAutoRunTickObject != nullptr)
	{
		mAutoRunTickObject->SetActive(true);
		mAutoRunTickObject->SetTickDelegate(InDelegate);
		AutoRun_OnBegin();
	}
}

/**
 * @brief	自動実行用のTickObject終了
 */
void UCSKitEditor_EUW_Base::EndAutoRunTickObject()
{
	if (mAutoRunTickObject != nullptr)
	{
		AutoRun_OnEnd();
		mAutoRunTickObject->SetActive(false);
		mAutoRunTickObject = nullptr;
	}
}

/**
 * @brief	自動実行更新
 */
void UCSKitEditor_EUW_Base::UpdateAutoRun(const float InDeltaTime)
{
	const ECSKit_CCResult CCResult = UpdateAutoRunCC(mCCAutoRun, GetWorld_Editor()->DeltaTimeSeconds);
	if(CCResult == ECSKit_CCResult::Finish)
	{
		EndAutoRunTickObject();
	}
}

/**
 * @brief	自動実行のCoroutine
 */
CC_FUNC( UCSKitEditor_EUW_Base::UpdateAutoRunCC )
{
	const UDataTable* DataTable = mSubLevelPresetDataTable.LoadSynchronous();
	if(DataTable == nullptr)
	{
		return ECSKit_CCResult::Finish;
	}
	const TArray<FName> RowNames = DataTable->GetRowNames();
	if(mAutoRunDataTableRowIndex >= RowNames.Num())
	{
		return ECSKit_CCResult::Finish;
	}
	const FName TargetUnitName = RowNames[mAutoRunDataTableRowIndex];
	const FCSKit_SubLevelPresetTableRow* TableRaw = DataTable->FindRow<FCSKit_SubLevelPresetTableRow>(TargetUnitName, TEXT(""));
	if(TableRaw == nullptr)
	{
		return ECSKit_CCResult::Finish;
	}
	
	CC_BEGIN();
	{
		if(!TableRaw->mbEditorEUWAutoRunTarget)
		{
			++mAutoRunDataTableRowIndex;
			CC_RESTART();
		}

		//対象のSubLevelロード
		CC_SLEEP_SEC( 1.f );
		RequestLoadSubLevel(*TableRaw);

		//SubLevelロード待ち
		CC_YIELD();
		CC_WAIT(!IsAllLoadedLevelStreaming());
		CC_SLEEP_SEC( 5.f );

		CC_WAIT(!AutoRun_ExecPostLoadSubLevel());
		CC_SLEEP_SEC( 1.f );

		ClearAllSubLevel();
		++mAutoRunDataTableRowIndex;
		CC_RESTART();
	}
	CC_END();

	return ECSKit_CCResult::Yield;
}

/**
 * @brief	SubLevelPresetをSubLevelとして追加
 */
bool UCSKitEditor_EUW_Base::RequestLoadSubLevel(const FCSKit_SubLevelPresetTableRow& InSubLevelPreset)
{
	mRequestLevelStreamingList.Empty();
	for(const TSoftObjectPtr<class UWorld>& ObjectPtr : InSubLevelPreset.mNeedLevelList)
	{
		const FString PackageName = ObjectPtr.GetLongPackageName();
		if(PackageName.IsEmpty())
		{
			continue;
		}
		if(ULevelStreaming* LevelStreaming = EditorLevelUtils::AddLevelToWorld(GetWorld(), *PackageName, ULevelStreamingDynamic::StaticClass()))
		{
			mRequestLevelStreamingList.Add(LevelStreaming);
		}
	}
	for(const TSoftObjectPtr<class UWorld>& ObjectPtr : InSubLevelPreset.mOptionLevelList)
	{
		const FString PackageName = ObjectPtr.GetLongPackageName();
		if(PackageName.IsEmpty())
		{
			continue;
		}
		if(ULevelStreaming* LevelStreaming = EditorLevelUtils::AddLevelToWorld(GetWorld(), *PackageName, ULevelStreamingDynamic::StaticClass()))
		{
			mRequestLevelStreamingList.Add(LevelStreaming);
		}
	}
	return mRequestLevelStreamingList.Num() > 0;
}

/**
 * @brief	SubLevelのロード終わったかどうか
 */
bool UCSKitEditor_EUW_Base::IsAllLoadedLevelStreaming() const
{
	for(const auto& WeakPtr : mRequestLevelStreamingList)
	{
		if(const ULevelStreaming* LevelStreaming = WeakPtr.Get())
		{
			if(!LevelStreaming->IsLevelLoaded())
			{
				return false;
			}
		}
	}
	return true;
}

/**
 * @brief	自動実行開始時処理
 */
void UCSKitEditor_EUW_Base::AutoRun_OnBegin()
{
}

/**
 * @brief	自動実行時のSubLevelロード後の処理
 *			trueを返す間継続
 */
bool UCSKitEditor_EUW_Base::AutoRun_ExecPostLoadSubLevel()
{
	return false;
}

/**
 * @brief	自動実行終了時処理
 */
void UCSKitEditor_EUW_Base::AutoRun_OnEnd()
{
}
