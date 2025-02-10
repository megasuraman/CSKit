// Copyright 2022 megasuraman
#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"
#include "CSKitEditor_EUW_Base.generated.h"

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
	
	virtual void FakeTick();
	virtual void OnRunGame(const UWorld& InWorld){}
	virtual void Draw(UCanvas* InCanvas, APlayerController* InPlayerController);

	
protected:
	FDelegateHandle mDebugDrawHandle;
	bool mbRunGameWorld = false;
	bool mbNeedRecoverAutoSaveFlag = false;
};
