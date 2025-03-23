// Copyright 2020 megasuraman.
/**
 * @file CSKitDebug_Subsystem.h
 * @brief CSKitDebug_Subsystem
 * @author megasuraman
 * @date 2020/7/24
 */
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "CSKitDebug_SaveData.h"
#include "CSKitDebug_Subsystem.generated.h"

class UCSKitDebug_ScreenWindowManager;
class UCSKitDebug_ShortcutCommand;
class UCSKitDebug_ActorSelectManager;
class UCSKitDebugMenuManager;
class UCSKitDebugInfoWindowManager;
class UCSKitDebug_DebugMenuManager;

DECLARE_LOG_CATEGORY_EXTERN(CSKitDebugLog, Log, All);

/**
 * 
 */
UCLASS(BlueprintType)
class CSKITDEBUG_API UCSKitDebug_Subsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	static FCSKitDebug_SaveData& sGetSaveData();

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UFUNCTION(BlueprintCallable, meta = (DevelopmentOnly, Category = "CSKitDebug"))
	UCSKitDebug_ScreenWindowManager* GetScreenWindowManagerBP() const;

	UCSKitDebug_ShortcutCommand* GetShortcutCommand() const { return mGCObject.mShortcutCommand; }
	UCSKitDebug_ActorSelectManager* GetActorSelectManager() const { return mGCObject.mActorSelectManager; }
	UCSKitDebug_DebugMenuManager* GetDebugMenuManager() const { return mGCObject.mDebugMenuManager; }
	UCSKitDebug_ScreenWindowManager* GetScreenWindowManager() const { return mGCObject.mScreenWindowManager; }

protected:
	void	RequestTick(const bool bInActive);
	void	RequestDraw(const bool bInActive);

	bool	DebugTick(float InDeltaSecond) const;
	void	DebugDraw(class UCanvas* InCanvas, class APlayerController* InPlayerController) const;

protected:
	struct FGCObjectCSKitDebug : public FGCObject
	{
		UCSKitDebug_ShortcutCommand* mShortcutCommand = nullptr;
		UCSKitDebug_ActorSelectManager* mActorSelectManager = nullptr;
		UCSKitDebug_DebugMenuManager* mDebugMenuManager = nullptr;
		UCSKitDebug_ScreenWindowManager* mScreenWindowManager = nullptr;
		virtual void AddReferencedObjects(FReferenceCollector& Collector) override
		{
			Collector.AddReferencedObject(mShortcutCommand);
			Collector.AddReferencedObject(mActorSelectManager);
			Collector.AddReferencedObject(mDebugMenuManager);
			Collector.AddReferencedObject(mScreenWindowManager);
		}
	};
	FGCObjectCSKitDebug	mGCObject;

private:
	TWeakObjectPtr<AActor>	mOwner;
	FDelegateHandle	mDebugTickHandle;
	FDelegateHandle	mDebugDrawHandle;
	static FCSKitDebug_SaveData mSaveData;
};
