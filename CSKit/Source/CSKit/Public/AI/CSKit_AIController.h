// Copyright 2020 megasuraman.
/**
 * @file CSKit_AIController.h
 * @brief 
 * @author megasuraman
 * @date 2025/05/05
 */
#pragma once

#include "CoreMinimal.h"
#include "Runtime/AIModule/Classes/AIController.h"
#include "Runtime/AIModule/Classes/Navigation/PathFollowingComponent.h"

#if USE_CSKIT_DEBUG
#include "CSKitDebug_CollectEQSResult.h"
#include "UObject/GCObject.h"
#endif

#include "CSKit_AIController.generated.h"

class ACSKit_AIFlow;
class UCSKit_AIFlowComponent;
class UCSKit_BrainQueryComponent;
class UCSKit_CommunityComponent;
//class UCSKit_EnvironmentAwarenessComponent;
class UCSKit_ExperienceComponent;
class UCSKit_NoticeComponent;
class UCSKit_RecognitionComponent;
class UCSKit_TerritoryComponent;
class UCSKit_WorryComponent;
struct FEnvQueryInstance;

DECLARE_DELEGATE_OneParam(FCSKit_AsyncFindPathFinishDelegate, FPathFollowingRequestResult);


UCLASS(Blueprintable, BlueprintType)
class CSKIT_API ACSKit_AIController : public AAIController
{
	GENERATED_BODY()

public:
	ACSKit_AIController(const FObjectInitializer& ObjectInitializer);
	virtual void PostInitializeComponents() override;
	virtual void Tick(float DeltaTime) override;
	virtual bool RunBehaviorTree(UBehaviorTree* BTAsset) override;
	// 経路探索リクエスト(ゴールがActorの場合の再検索の距離間隔が固定値なのでoverride)
	virtual void FindPathForMoveRequest(const FAIMoveRequest& MoveRequest, FPathFindingQuery& Query, FNavPathSharedPtr& OutPath) const override;
	virtual void UpdateControlRotation(float DeltaTime, bool bUpdatePawn = true) override;
	virtual FAIRequestID RequestMove(const FAIMoveRequest& MoveRequest, FNavPathSharedPtr Path) override;

	virtual	void OnTakeDamage(AActor* InDamageCauser, const float InDamage, const uint16 InParamBit);
	virtual	void OnApplyDamage(AActor* InHitTarget, const float InDamage, const uint16 InParamBit);
	virtual void OnChangeNoticeTarget(AActor* InOldTarget, AActor* InNewTarget);
	virtual bool IsPossibleToMoveStart() const { return true; }
	virtual bool IsPossibleToUpdateBrainQuery() const {return true;}
	virtual bool IsPossibleToRotate() const { return true; }
	
	UFUNCTION(BlueprintCallable, Category = "CSKit_AIController")
	void SetAIFlowBP(const ACSKit_AIFlow* InAIFlow)
	{
		SetAIFlow(InAIFlow);
	}

	UCSKit_RecognitionComponent* GetCSKitRecognition() const { return mCSKit_RecognitionComponent; }
	UCSKit_NoticeComponent* GetCSKitNotice() const { return mCSKit_NoticeComponent; }
	UCSKit_ExperienceComponent* GetCSKitExperience() const { return mCSKit_ExperienceComponent; }
	UCSKit_BrainQueryComponent* GetCSKitBrainQuery() const { return mCSKit_BrainQueryComponent; }
	UCSKit_CommunityComponent* GetCSKitCommunity() const;
	//UCSKit_EnvironmentAwarenessComponent* GetCSKitEnvironmentAwareness() const;
	UCSKit_TerritoryComponent* GetCSKitTerritoryComponent() const;
	UCSKit_WorryComponent* GetCSKitWorryComponent() const;
	UCSKit_AIFlowComponent* GetCSKitAIFlowComponent() const;

	FPathFollowingRequestResult MoveToDirect(const AActor* InTarget, const float InAcceptanceRadius);
	bool IsMoveToDirect(const AActor* InTarget) const;
	void AbortMoveToDirect(const AActor* InTarget) const;
	FPathFollowingRequestResult MoveToDirectControl(const FVector& InTargetPos, const float InAcceptanceRadius, const bool bInFocusNoticeTarget);
	bool IsMoveToDirectControl() const;
	void AbortMoveToDirectControl() const;
	uint32 MoveToAsync(const FAIMoveRequest& InMoveRequest, const FCSKit_AsyncFindPathFinishDelegate& InFinishDelegate);
	void AbortAsyncFindPathRequest(const uint32 InAsyncPathQueryId);
	// これだとリクエストが上書きされたときにわからない。。。
	const FPathFollowingRequestResult*	GetLastAsyncFindPathResult(const uint32 InAsyncPathQueryId) const
	{
		if (mLastResultAsyncPathQueryId == InAsyncPathQueryId)
		{
			return &mLastAsyncFindPathResult;
		}
		return nullptr;
	}
	AActor* GetNoticeTarget() const;
	void RequestStopAI() const;
	void RequestResumeAI() const;
	bool IsPausedAI() const;
	void SetLowMode(const bool bInLowMode);
	void RequestForceRotation(const FRotator& InRot) { mRequestForceRotation = InRot; }
	bool IsFriend(const AActor* InTarget) const;
	void SetAbility(const FName& InName, const bool bInFlag);
	bool IsOwnAbility(const FName& InName) const;
	void SetAIFlow(const ACSKit_AIFlow* InAIFlow);
	const FVector& GetHomePos() const{return mHomePos;}
	const FRotator& GetHomeRot() const{return mHomeRot;}
	void OverrideHomePosRot(){SetHomePosRot();}
	const ANavigationData* GetAgentNavigationData() const;
	void SetAIKindName(const FName& InName){mAIKindName=InName;}
	const FName& GetAIKindName() const{return mAIKindName;}

protected:
	virtual void OnPossess(APawn* InPawn) override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void UpdateFocalPoint(const float InDeltaTime) {}
	virtual void UpdateControlRotationSpeed(const float InDeltaTime){}
	virtual void OnEntryCommunity(){}
	virtual void OnExitCommunity(){}
	virtual UCSKit_AIFlowComponent* SafeRegisterAIFlowComponent();

	void OnFinishFindPathAsync(uint32 InQueryId, ENavigationQueryResult::Type InResult, FNavPathSharedPtr InPath);
	void SetControlRotationSpeed(const float InSpeed) { mControlRotationSpeed = InSpeed; }
	float GetLowModeIntervalTime() const { return mLowModeIntervalTime; }
	void SetAIFlowComponent(UCSKit_AIFlowComponent* InComponent);
	void AttachTerritoryComponent(const FName& InUserName);
	void DetachTerritoryComponent();
	virtual void SetHomePosRot();

protected:
	UPROPERTY(EditDefaultsOnly, Category = CSKit)
	UBehaviorTree* mDefaultBehaviorTree = nullptr;
	UPROPERTY(VisibleDefaultsOnly, Category = CSKit)
	UCSKit_RecognitionComponent* mCSKit_RecognitionComponent = nullptr;
	UPROPERTY(VisibleDefaultsOnly, Category = CSKit)
	UCSKit_NoticeComponent* mCSKit_NoticeComponent = nullptr;
	UPROPERTY(VisibleDefaultsOnly, Category = CSKit)
	UCSKit_ExperienceComponent* mCSKit_ExperienceComponent = nullptr;
	UPROPERTY(VisibleDefaultsOnly, Category = CSKit)
	UCSKit_BrainQueryComponent* mCSKit_BrainQueryComponent = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = CSKit)
	TMap<FName, bool> mAIAbilityMap;
	UPROPERTY(EditDefaultsOnly, Category = CSKit)
	FName mBBInitDataTableRowName;
	UPROPERTY(EditDefaultsOnly, Category = CSKit)
	float mLowModeIntervalTime = 3.f;
	UPROPERTY(EditDefaultsOnly, Category = CSKit)
	bool mbEverLowMode = false;
	
private:
	FAIMoveRequest mAsyncFindPathRequest;
	FCSKit_AsyncFindPathFinishDelegate mAsyncFindPathFinishDelegate;
	FPathFollowingRequestResult	mLastAsyncFindPathResult;
	FVector mHomePos = FVector::ZeroVector;
	FRotator mHomeRot = FRotator::ZeroRotator;
	FRotator mRequestForceRotation = FAISystem::InvalidRotation;
	TWeakObjectPtr<UCSKit_CommunityComponent> mCSKit_CommunityComponent;
	//TWeakObjectPtr<UCSKit_EnvironmentAwarenessComponent> mCSKit_EnvironmentAwarenessComponent;
	TWeakObjectPtr<UCSKit_TerritoryComponent> mCSKit_TerritoryComponent;
	TWeakObjectPtr<UCSKit_WorryComponent> mCSKit_WorryComponent;
	TWeakObjectPtr<UCSKit_AIFlowComponent> mCSKit_AIFlowComponent;
	float mControlRotationSpeed = 360.f;
	float mLowModeInterval = 0.f;
	uint32 mAsyncPathQueryId = INVALID_NAVQUERYID;//非同期経路探索リクエスト時のID
	uint32 mLastResultAsyncPathQueryId = INVALID_NAVQUERYID;
	FName mAIKindName;//AIの区分け用
	bool mbLowMode = false;

	/*** Debug機能 ***/
protected:
	UFUNCTION()
	FString DebugDrawCallActorWatcher(UCanvas* InCanvas) const;
	UFUNCTION()
	void DebugCallGhostInput(class APlayerController* InController);
	UFUNCTION()
	void DebugCallGhostInputDraw(UCanvas* InCanvas);
	/*****/

#if USE_CSKIT_DEBUG
public:
	virtual FString DebugGetDetailLog() const;
	bool DebugTestBehaviorTree(const FString& InFolderPath, const FString& InAssetName, const bool bInLock);

	void DebugBeginCollectEQSResult();
	void DebugEndCollectEQSResult(const FString& InResult);
	const FCSKitDebug_CollectEQSResult& DebugGetCollectEQSResult() const { return mDebugCollectEQSResult; }
	void DebugSetGhostPlayMode(const bool bInGhostPlay);
	bool DebugIsGhostPlayMode() const{return mbDebugGhostPlayMode;}

protected:
	virtual FString DebugGetDetailLogBase() const;
	virtual FString DebugDrawSelectedActorWatcher(UCanvas* InCanvas) const;
	virtual void DebugGhostPlayInput(class APlayerController& InController){}
	virtual void DebugGhostPlayInputDraw(UCanvas* InCanvas){}

	bool DebugIsLockBehaviorTree() const { return mbDebugLockBehaviorTree; }
	FEnvQueryInstance* DebugGetLastEnvQueryInstance(float& OutTimeStamp) const;
	FString DebugDrawSelectedActorWatcherLastEQS(UCanvas* InCanvas) const;

private:
	struct FDebugGCObject : public FGCObject
	{
		UBehaviorTree* mDebugTestBehaviorTree = nullptr;
		virtual void AddReferencedObjects(FReferenceCollector& Collector) override
		{
			Collector.AddReferencedObject(mDebugTestBehaviorTree);
		}
		virtual FString GetReferencerName() const override
		{
			return FString(TEXT("ACSKit_AIController_Debug"));
		}
	};
	FDebugGCObject mDebugGCObject;
	FCSKitDebug_CollectEQSResult mDebugCollectEQSResult;
	FString mDebugTestBehaviorTreePath;
	union
	{
		uint8 mDebugFlags = 0;
		struct
		{
			uint8 mbDebugLockBehaviorTree : 1;
			uint8 mbDebugGhostPlayMode : 1;
			uint8 : 7;
		};
	};
#endif
};
