// Copyright 2022 megasuraman
#pragma once

#include "CoreMinimal.h"
#include "CSKitEditor_EUW_TickObjectBase.h"
#include "CSKit_CppCoroutine.h"
#include "EditorUtilityWidget.h"
#include "CSKitEditor_EUW_Base.generated.h"

struct FCSKit_SubLevelPresetTableRow;

UCLASS()
class CSKITEDITOR_API UCSKitEditor_EUW_Base : public UEditorUtilityWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeDestruct() override;

	UFUNCTION(BlueprintCallable, Category = "CSKitEditor_EUW")
	UWorld* GetWorld_Editor() const;
	UFUNCTION(BlueprintCallable, Category = "CSKitEditor_EUW")
	UWorld* GetWorld_GameServer() const;
	UFUNCTION(BlueprintCallable, Category = "CSKitEditor_EUW")
	UWorld* GetWorld_GameClient() const;
	UFUNCTION(BlueprintCallable, Category = "CSKitEditor_EUW")
	bool ParseCommandLine(FString& OutValue, const FString& InKey) const;
	UFUNCTION(BlueprintCallable, Category = "CSKitEditor_EUW")
	void SetActiveDraw(const bool bInDraw);
	UFUNCTION(BlueprintCallable, Category = "CSKitEditor_EUW")
	void OffAutoSaveFlag();
	UFUNCTION(BlueprintCallable, Category = "CSKitEditor_EUW")
	void RecoverAutoSaveFlag();
	UFUNCTION(BlueprintCallable, Category = "CSKitEditor_EUW")
	void FixupRedirects(const TArray<FString>& InTargetAssetNameList);
	UFUNCTION(BlueprintCallable, Category = "CSKitEditor_EUW")
	void SetRealTimeDraw_LevelEditorViewport(const bool bInActive) const;
	UFUNCTION(BlueprintCallable, Category = "CSKitEditor_EUW")
	void LookAtPos(FVector InPos);
	UFUNCTION(BlueprintCallable, Category = "CSKitEditor_EUW")
	void ClearAllSubLevel();
	UFUNCTION(BlueprintCallable, Category = "CSKitEditor_EUW")
	bool SaveAsset(UObject* InAsset);
	UFUNCTION(BlueprintCallable, Category = "CSKitEditor_EUW")
	bool SaveLevel(ULevel* InLevel);
	UFUNCTION(BlueprintCallable, Category = "CSKitEditor_EUW")
	void BeginAutoRun();
	UFUNCTION(BlueprintCallable, Category = "CSKitEditor_EUW")
	void EndAutoRun();
	
	virtual void FakeTick();
	virtual void OnRunGame(const UWorld& InWorld){}
	virtual void Draw(UCanvas* InCanvas, APlayerController* InPlayerController);
	static FString GetAssetFullPath(const FString& InAssetPath);
	static int64 GetFileTimeStampSecond(const FString& InAssetFullPath);
	void BeginAutoRunTickObject(const FCSKitEditor_EUW_TickEvent& InDelegate);
	void EndAutoRunTickObject();
	void UpdateAutoRun(const float InDeltaTime);
	DECL_CC_FUNC(UpdateAutoRunCC);
	bool RequestLoadSubLevel(const FCSKit_SubLevelPresetTableRow& InSubLevelPreset);
	bool IsAllLoadedLevelStreaming() const;
	virtual void AutoRun_OnBegin();
	virtual bool AutoRun_ExecPostLoadSubLevel();
	virtual void AutoRun_OnEnd();

protected:
	UPROPERTY()
	UCSKitEditor_EUW_TickObjectBase* mAutoRunTickObject = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "CSKitEditor_EUW", meta = (DisplayName = "SubLevelPresetDataTable", DisplayPriority=1))
	TSoftObjectPtr<UDataTable> mSubLevelPresetDataTable;
	
	CSKit_CppCoroutine mCCAutoRun;
	FDelegateHandle mDebugDrawHandle;
	TArray<TWeakObjectPtr<ULevelStreaming>> mRequestLevelStreamingList;
	int32 mAutoRunDataTableRowIndex = 0;
	bool mbRunGameWorld = false;
	bool mbNeedRecoverAutoSaveFlag = false;
};
