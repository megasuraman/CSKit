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

class UCSKitDebug_CollisionInspectorManager;
class ACSKitDebug_GhostController;
class ACSKitDebug_GhostPawn;
class UCSKitDebug_ScreenWindowManager;
class UCSKitDebug_ShortcutCommand;
class UCSKitDebug_ActorSelectManager;
class UCSKitDebugMenuManager;
class UCSKitDebugInfoWindowManager;
class UCSKitDebug_DebugMenuManager;

DECLARE_LOG_CATEGORY_EXTERN(CSKitDebugLog, Log, All);


UCLASS(BlueprintType)
class CSKITDEBUG_API UCSKitDebug_Subsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	static void sOneShotWarning(const UWorld* InWorld, const bool bInExpression, const FName& InKey, const FString& InLog);
	static FCSKitDebug_SaveData& sGetSaveData();

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UFUNCTION(BlueprintCallable, meta = (DevelopmentOnly, Category = "CSKitDebug"))
	UCSKitDebug_ScreenWindowManager* GetScreenWindowManagerBP() const;

	UCSKitDebug_ShortcutCommand* GetShortcutCommand() const { return mGCObject.mShortcutCommand; }
	UCSKitDebug_ActorSelectManager* GetActorSelectManager() const { return mGCObject.mActorSelectManager; }
	UCSKitDebug_DebugMenuManager* GetDebugMenuManager() const { return mGCObject.mDebugMenuManager; }
	UCSKitDebug_ScreenWindowManager* GetScreenWindowManager() const { return mGCObject.mScreenWindowManager; }
	UCSKitDebug_CollisionInspectorManager* GetCollisionInspectorManager() const{return mGCObject.mCollisionInspectorManager;}

	void OneShotWarning(const bool bInExpression, const FName& InKey, const FString& InLog);
	void BeginGhostController(AActor* InTarget);
	void EndGhostController();
	ACSKitDebug_GhostPawn* GetGhostPawn() const{return mGCObject.mGhostPawn;}
	ACSKitDebug_GhostController* GetGhostController() const{return mGCObject.mGhostController;}
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
		UCSKitDebug_CollisionInspectorManager* mCollisionInspectorManager = nullptr;
		ACSKitDebug_GhostPawn* mGhostPawn = nullptr;
		ACSKitDebug_GhostController* mGhostController = nullptr;
		virtual void AddReferencedObjects(FReferenceCollector& Collector) override
		{
			Collector.AddReferencedObject(mShortcutCommand);
			Collector.AddReferencedObject(mActorSelectManager);
			Collector.AddReferencedObject(mDebugMenuManager);
			Collector.AddReferencedObject(mScreenWindowManager);
			Collector.AddReferencedObject(mCollisionInspectorManager);
			Collector.AddReferencedObject(mGhostPawn);
			Collector.AddReferencedObject(mGhostController);
		}
	};
	FGCObjectCSKitDebug	mGCObject;

private:
	static FCSKitDebug_SaveData mSaveData;
	TWeakObjectPtr<AActor>	mOwner;
	FDelegateHandle	mDebugTickHandle;
	FDelegateHandle	mDebugDrawHandle;
	TArray<FName> mOneShotWarningKeyList;
	TWeakObjectPtr<APlayerController> mOriginalPlayerController;
};
