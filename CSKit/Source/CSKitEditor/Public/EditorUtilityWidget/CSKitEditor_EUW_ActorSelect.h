// Copyright 2022 megasuraman
/**
 * @file CSKitEditor_EUW_Base.h
 * @brief ActorSelector操作EUW
 * @author megasuraman
 * @date 2023/03/30
 */
#pragma once

#include "CoreMinimal.h"
#include "CSKitEditor_EUW_Base.h"
#include "CSKitEditor_EUW_ActorSelect.generated.h"

class UCSKitDebug_ActorSelectManager;

UCLASS()
class CSKITEDITOR_API UCSKitEditor_EUW_ActorSelect : public UCSKitEditor_EUW_Base
{
	GENERATED_BODY()

public:
	virtual void PostInitProperties() override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

protected:
	UFUNCTION(BlueprintCallable, Category = "EUW_ActorSelector")
	bool SetupTargetObjectList();
	UFUNCTION(BlueprintCallable, Category = "EUW_ActorSelector")
	FString GetDrawObjectName(UObject* InTarget) const;
	UFUNCTION(BlueprintCallable, Category = "EUW_ActorSelector")
	void Clear();
	UFUNCTION(BlueprintCallable, Category = "EUW_ActorSelector")
	void SelectActorSelector();
	UFUNCTION(BlueprintCallable, Category = "EUW_ActorSelector")
	void SetAutoSelect(bool bInAutoSelect);
	UFUNCTION(BlueprintCallable, Category = "EUW_ActorSelector")
	void SetOnlyUpdateSelectActor(bool bInOnlyUpdate);
	UFUNCTION(BlueprintCallable, Category = "EUW_ActorSelector")
	bool IsAutoSelect() const { return mbAutoSelect; }
	UFUNCTION(BlueprintCallable, Category = "EUW_ActorSelector")
	bool IsOnlyUpdateSelectActor() const { return mbOnlyUpdateSelectActor; }
	//UFUNCTION(BlueprintCallable, Category = "EUW_ActorSelector")
	//void ClearDrawFlagMap();
	UFUNCTION(BlueprintCallable, Category = "EUW_ActorSelector")
	bool IsDrawFlag(FName InName) const;
	UFUNCTION(BlueprintCallable, Category = "EUW_ActorSelector")
	const TMap<FName, bool>& GetDrawFlagMap() const { return mDrawFlagMap; }
	UFUNCTION(BlueprintCallable, Category = "EUW_ActorSelector")
	void SetLookMode(bool bInLook);
	UFUNCTION(BlueprintCallable, Category = "EUW_ActorSelector")
	void LastSelectTargetWarp();
	UFUNCTION()
	TArray<FString> GetBookmarkSelectorList();
	UFUNCTION()
	TArray<FString> GetTargetObjectSelectorList();
	UFUNCTION(BlueprintCallable, Category = "EUW_ActorSelector")
	void AddBookmarkFromTargetClass();
	UFUNCTION(BlueprintCallable, Category = "EUW_ActorSelector")
	void SubBookmarkFromSelector();

	virtual void FakeTick() override;
	virtual void OnRunGame(const UWorld& InWorld) override;

	static UCSKitDebug_ActorSelectManager* GetActorSelectorManager(const UWorld& InWorld);
	void AssignParameterToGame() const;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EUW_ActorSelect", meta = (DisplayName = "選択対象クラス", DisplayPriority = 2))
	TSoftClassPtr<UObject>	mTargetClass = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EUW_ActorSelect", meta = (DisplayName = "Bookmark", GetOptions="GetBookmarkSelectorList", DisplayPriority = 3))
	FString mBookmarkClassSelector;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EUW_ActorSelect", meta = (DisplayName = "TargetObject", GetOptions="GetTargetObjectSelectorList", DisplayPriority = 4))
	FString mTargetObjectSelector;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EUW_ActorSelect", meta = (DisplayName = "Active", DisplayPriority = 1))
	bool mbActive = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EUW_ActorSelect", meta = (DisplayName = "自動選択", DisplayPriority = 1))
	bool mbAutoSelect = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EUW_ActorSelect", meta = (DisplayName = "選択対象のみTick実行", DisplayPriority = 1))
	bool mbOnlyUpdateSelectActor = false;

	TMap<FName, bool> mDrawFlagMap;
	TArray<TSoftClassPtr<UObject>>	mBookmarkClass;
	TArray<TSoftObjectPtr<UObject>> mTargetObjectList;
	TWeakObjectPtr<UObject> mTargetObject;
	float mTargetObjectSearchIntervalSec = 0.f;
};
