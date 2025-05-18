// Copyright 2020 megasuraman.
/**
 * @file CSKit_AIController.cpp
 * @brief 
 * @author megasuraman
 * @date 2025/05/05
 */
#include "AI/CSKit_AIController.h"

#include "AI/CSKit_NeedLevelManager.h"
#include "AI/CSKit_PathFollowingComponent.h"
#include "CSKit_Subsystem.h"
#include "BehaviorTree/BehaviorTree.h"
#include "AI/BrainQuery/CSKit_BrainQueryComponent.h"
#include "AI/Community/CSKit_CommunityComponent.h"
//#include "EnvironmentMap/CSKit_EnvironmentAwarenessComponent.h"
#include "CSKit_Config.h"
#include "AI/Experience/CSKit_ExperienceComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Character.h"
#include "NavigationSystem.h"
#include "AI/AIFlow/CSKit_AIFlowComponent.h"
#include "AI/Blackboard/CSKit_BBInitDataTable.h"
#include "AI/Notice/CSKit_NoticeComponent.h"
#include "AI/Recognition/CSKit_RecognitionComponent.h"
#include "AI/Territory/CSKit_TerritoryComponent.h"
#include "AI/Worry/CSKit_WorryComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

#if USE_CSKIT_DEBUG
#include "EnvironmentQuery/EnvQueryDebugHelpers.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_VectorBase.h"
#include "CSKitDebug_Draw.h"
#include "CSKitDebug_Subsystem.h"
#include "ScreenWindow/CSKitDebug_ScreenWindowText.h"
#include "ActorSelect/CSKitDebug_ActorSelectManager.h"
#endif

ACSKit_AIController::ACSKit_AIController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UCSKit_PathFollowingComponent>(TEXT("PathFollowingComponent"))
	)
{
	bNetLoadOnClient = false;
	mCSKit_RecognitionComponent = CreateDefaultSubobject<UCSKit_RecognitionComponent>(TEXT("RecognitionComponent"));
	mCSKit_NoticeComponent = CreateDefaultSubobject<UCSKit_NoticeComponent>(TEXT("NoticeComponent"));
	mCSKit_ExperienceComponent = CreateDefaultSubobject<UCSKit_ExperienceComponent>(TEXT("ExperienceComponent"));
	mCSKit_BrainQueryComponent = CreateDefaultSubobject<UCSKit_BrainQueryComponent>(TEXT("BrainQueryComponent"));

	FCSKitDebug_SaveData Test;
	Test.SetBool(FString(), true);
}
/* ------------------------------------------------------------
   !
------------------------------------------------------------ */
void ACSKit_AIController::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	mCSKit_CommunityComponent = FindComponentByClass<UCSKit_CommunityComponent>();
	//mCSKit_EnvironmentAwarenessComponent = FindComponentByClass<UCSKit_EnvironmentAwarenessComponent>();
	mCSKit_TerritoryComponent = FindComponentByClass<UCSKit_TerritoryComponent>();
	mCSKit_WorryComponent = FindComponentByClass<UCSKit_WorryComponent>();
	mCSKit_AIFlowComponent = FindComponentByClass<UCSKit_AIFlowComponent>();

	if(UCSKit_CommunityComponent* CommunityComponent = GetCSKitCommunity())
	{
		CommunityComponent->AddDelegateOnEntryCommunity(
			FCSKit_OnEntryCommunity::FDelegate::CreateUObject(this, &ACSKit_AIController::OnEntryCommunity)
			);
		CommunityComponent->AddDelegateOnExitCommunity(
			FCSKit_OnEntryCommunity::FDelegate::CreateUObject(this, &ACSKit_AIController::OnExitCommunity)
			);
	}
}

/* ------------------------------------------------------------
   !
------------------------------------------------------------ */
void ACSKit_AIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (mbLowMode
		|| mbEverLowMode)
	{
		if (mLowModeInterval > 0.f)
		{
			mLowModeInterval -= DeltaTime;
			return;
		}
		else
		{
			mLowModeInterval = GetLowModeIntervalTime();
		}
	}

	if (mCSKit_RecognitionComponent)
	{
		mCSKit_RecognitionComponent->Update(DeltaTime);
	}
	if (mCSKit_NoticeComponent)
	{
		mCSKit_NoticeComponent->Update(DeltaTime);
	}
	if (UCSKit_ExperienceComponent* ExperienceComponent = GetCSKitExperience())
	{
		ExperienceComponent->Update(DeltaTime);
	}
	if (mCSKit_BrainQueryComponent)
	{
		mCSKit_BrainQueryComponent->Update(DeltaTime);
	}
	// if (UCSKit_EnvironmentAwarenessComponent* EnvironmentAwareness = GetCSKitEnvironmentAwareness())
	// {
	// 	EnvironmentAwareness->Update(DeltaTime);
	// }
	if (UCSKit_TerritoryComponent* TerritoryComponent = GetCSKitTerritoryComponent())
	{
		TerritoryComponent->Update(DeltaTime);
	}
	if (UCSKit_WorryComponent* WorryComponent = GetCSKitWorryComponent())
	{
		WorryComponent->Update(DeltaTime);
	}
}

/* ------------------------------------------------------------
   !BehaviorTree実行
------------------------------------------------------------ */
bool ACSKit_AIController::RunBehaviorTree(UBehaviorTree* BTAsset)
{
#if USE_CSKIT_DEBUG
	if (DebugIsLockBehaviorTree())
	{
		return false;
	}
#endif

	const bool bSuccess = Super::RunBehaviorTree(BTAsset);
	if (UBlackboardComponent* BlackboardComponent = GetBlackboardComponent())
	{
		const UCSKit_Config* CSKitConfig = GetDefault<UCSKit_Config>();
		if (UDataTable* DataTable = Cast<UDataTable>(CSKitConfig->mBBInitDataTablePath.LoadSynchronous()))
		{
			if (FCSKit_BBInitValueTableRow* InitValue = DataTable->FindRow<FCSKit_BBInitValueTableRow>(mBBInitDataTableRowName, nullptr))
			{
				InitValue->ApplyBlackboardValue(*BlackboardComponent);
			}
		}
	}
	return bSuccess;
}

/* ------------------------------------------------------------
   !経路探索リクエスト
------------------------------------------------------------ */
void ACSKit_AIController::FindPathForMoveRequest(const FAIMoveRequest& MoveRequest, FPathFindingQuery& Query, FNavPathSharedPtr& OutPath) const
{
	Super::FindPathForMoveRequest(MoveRequest, Query, OutPath);

 	if (const UCSKit_PathFollowingComponent* CSKit_PathFollowingComponent = Cast<UCSKit_PathFollowingComponent>(GetPathFollowingComponent()))
 	{
 		if (FNavigationPath* PathInstance = OutPath.Get())
 		{
 			PathInstance->SetGoalActorTetherDistance(CSKit_PathFollowingComponent->GetNavPathGoalActorTetherDistance());
 		}
 	}
 
 #if USE_CSKIT_DEBUG
 	if (UCSKit_PathFollowingComponent* CSKit_PathFollowingComponent = Cast<UCSKit_PathFollowingComponent>(GetPathFollowingComponent()))
 	{
 		if (const APawn* OwnerPawn = GetPawn())
 		{
			CSKit_PathFollowingComponent->DebugSetFindPathPos(OwnerPawn->GetActorLocation(), MoveRequest.GetDestination());
 		}
 	}
 #endif
}

/* ------------------------------------------------------------
   !向き更新
   !Engineの実装が変なので、必要な部分だけ抜粋して自前で用意
   !（bUpdatePawnで直接PawnのSetActorRotation()されたり）
------------------------------------------------------------ */
void	ACSKit_AIController::UpdateControlRotation(float DeltaTime, bool bUpdatePawn)
{
	UpdateFocalPoint(DeltaTime);
	UpdateControlRotationSpeed(DeltaTime);

	const APawn* const OwnerPawn = GetPawn();
	if (OwnerPawn == nullptr)
	{
		return;
	}
	
	if (const ACharacter* OwnerCharacter = Cast<ACharacter>(GetPawn()))
	{
		if (UCharacterMovementComponent* CharacterMovementComponent = OwnerCharacter->GetCharacterMovement())
		{
			// PhysicsRotation()用にフラグ操作
			CharacterMovementComponent->bAllowPhysicsRotationDuringAnimRootMotion = true;//ルートモーション中もPhysicsRotation()を有効にするかどうか

			CharacterMovementComponent->bUseControllerDesiredRotation = true;
			CharacterMovementComponent->RotationRate = FRotator(0.f, mControlRotationSpeed, 0.f);
			if(mRequestForceRotation != FAISystem::InvalidRotation)
			{
				CharacterMovementComponent->RotationRate = FRotator(0.f, 36000.f, 0.f);
			}
		}
	}

	FRotator NewControlRotation = GetControlRotation();
	const FVector FocalPoint = GetFocalPoint();
	if(mRequestForceRotation != FAISystem::InvalidRotation)
	{
		NewControlRotation = mRequestForceRotation;
		if(FMath::Abs(OwnerPawn->GetActorRotation().Yaw - mRequestForceRotation.Yaw) < 1.f)
		{//原因分からないけど一瞬で回しても反映されないことがあるので
			mRequestForceRotation = FAISystem::InvalidRotation;
		}
	}
	else if (FAISystem::IsValidLocation(FocalPoint))
	{
		NewControlRotation = (FocalPoint - OwnerPawn->GetPawnViewLocation()).Rotation();
	}
	else if (bSetControlRotationFromPawnOrientation)
	{
		NewControlRotation = OwnerPawn->GetActorRotation();
	}

	NewControlRotation.Pitch = 0.f;

	SetControlRotation(NewControlRotation);
}

/* ------------------------------------------------------------
   !
------------------------------------------------------------ */
FAIRequestID ACSKit_AIController::RequestMove(const FAIMoveRequest& MoveRequest, FNavPathSharedPtr Path)
{
	//非同期リクエスト後に呼び出す際に移動開始可能か再度チェック
	if(!IsPossibleToMoveStart())
	{
		return FAIRequestID::InvalidRequest;
	}
	
	return Super::RequestMove(MoveRequest, Path);
}

/* ------------------------------------------------------------
   !被ダメージ時
------------------------------------------------------------ */
void ACSKit_AIController::OnTakeDamage(AActor* InDamageCauser, const float InDamage, const uint16 InParamBit)
{
	if (UCSKit_RecognitionComponent* RecognitionComponent = GetCSKitRecognition())
	{
		RecognitionComponent->OnTakeDamage(InDamageCauser, InDamage);
	}
	if (UCSKit_ExperienceComponent* ExperienceComponent = GetCSKitExperience())
	{
		ExperienceComponent->OnTakeDamage(InDamageCauser, InDamage, InParamBit);
	}
}

/* ------------------------------------------------------------
   !与ダメージ時
------------------------------------------------------------ */
void ACSKit_AIController::OnApplyDamage(AActor* InHitTarget, const float InDamage, const uint16 InParamBit)
{
	if (UCSKit_ExperienceComponent* ExperienceComponent = GetCSKitExperience())
	{
		ExperienceComponent->OnApplyDamage(InHitTarget, InDamage, InParamBit);
	}
}

/* ------------------------------------------------------------
   !
------------------------------------------------------------ */
void ACSKit_AIController::OnChangeNoticeTarget(AActor* InOldTarget, AActor* InNewTarget)
{
	if (UBlackboardComponent* BlackboardComponent = GetBlackboardComponent())
	{
		BlackboardComponent->SetValueAsObject(FName(TEXT("NoticeTarget")), InNewTarget);
	}
	if (UCSKit_ExperienceComponent* ExperienceComponent = GetCSKitExperience())
	{
		ExperienceComponent->OnChangeTarget(InNewTarget);
	}
	// if (UCSKit_EnvironmentAwarenessComponent* EnvironmentAwareness = GetCSKitEnvironmentAwareness())
	// {
	// 	EnvironmentAwareness->OnChangeTarget(InNewTarget);
	// }
}

/* ------------------------------------------------------------
   !
------------------------------------------------------------ */
UCSKit_CommunityComponent* ACSKit_AIController::GetCSKitCommunity() const
{
	return mCSKit_CommunityComponent.Get();
}

/* ------------------------------------------------------------
   !
------------------------------------------------------------ */
// UCSKit_EnvironmentAwarenessComponent* ACSKit_AIController::GetCSKitEnvironmentAwareness() const
// {
// 	return mCSKit_EnvironmentAwarenessComponent.Get();
// }

/* ------------------------------------------------------------
   !
------------------------------------------------------------ */
UCSKit_TerritoryComponent* ACSKit_AIController::GetCSKitTerritoryComponent() const
{
	return mCSKit_TerritoryComponent.Get();
}

/* ------------------------------------------------------------
   !
------------------------------------------------------------ */
UCSKit_WorryComponent* ACSKit_AIController::GetCSKitWorryComponent() const
{
	return mCSKit_WorryComponent.Get();
}

/* ------------------------------------------------------------
   !
------------------------------------------------------------ */
UCSKit_AIFlowComponent* ACSKit_AIController::GetCSKitAIFlowComponent() const
{
	return mCSKit_AIFlowComponent.Get();
}

/* ------------------------------------------------------------
   !経路探索なしの移動リクエスト
------------------------------------------------------------ */
FPathFollowingRequestResult ACSKit_AIController::MoveToDirect(const AActor* InTarget, const float InAcceptanceRadius)
{
	FPathFollowingRequestResult ResultData;

	UCSKit_PathFollowingComponent* PathFollowing = Cast<UCSKit_PathFollowingComponent>(GetPathFollowingComponent());
	if (PathFollowing == nullptr)
	{
		ResultData.Code = EPathFollowingRequestResult::Failed;
		return ResultData;
	}

	if (PathFollowing->IsPlayingDirectMoveToActor(InTarget))
	{
		ResultData.Code = EPathFollowingRequestResult::RequestSuccessful;
		return ResultData;
	}

	FAIMoveRequest MoveReq;
	MoveReq.SetNavigationFilter(GetDefaultNavigationFilterClass());//使わないけど、AAIController::MoveTo()のensureに引っかかるので
	MoveReq.SetProjectGoalLocation(false);
	MoveReq.SetUsePathfinding(false);
	MoveReq.SetAcceptanceRadius(InAcceptanceRadius);
	MoveReq.SetGoalActor(InTarget);

	ResultData = MoveTo(MoveReq);

	PathFollowing->RequestDirectMoveToActor();

	return ResultData;
}

/* ------------------------------------------------------------
   !経路探索なしの移動中かどうか
------------------------------------------------------------ */
bool ACSKit_AIController::IsMoveToDirect(const AActor* InTarget) const
{
	const UCSKit_PathFollowingComponent* PathFollowing = Cast<UCSKit_PathFollowingComponent>(GetPathFollowingComponent());
	if (PathFollowing == nullptr)
	{
		return false;
	}

	return PathFollowing->IsPlayingDirectMoveToActor(InTarget);
}

/* ------------------------------------------------------------
   !経路探索なしの移動中断
------------------------------------------------------------ */
void ACSKit_AIController::AbortMoveToDirect(const AActor* InTarget) const
{
	UCSKit_PathFollowingComponent* PathFollowing = Cast<UCSKit_PathFollowingComponent>(GetPathFollowingComponent());
	if (PathFollowing == nullptr)
	{
		return;
	}

	if (PathFollowing->IsPlayingDirectMoveToActor(InTarget))
	{
		PathFollowing->AbortMove(*this, FPathFollowingResultFlags::UserAbort);
	}
}
/* ------------------------------------------------------------
   !経路探索なしで外部から直接制御する移動リクエスト
------------------------------------------------------------ */
FPathFollowingRequestResult ACSKit_AIController::MoveToDirectControl(const FVector& InTargetPos, const float InAcceptanceRadius, const bool bInFocusNoticeTarget)
{
	FPathFollowingRequestResult ResultData;

	UCSKit_PathFollowingComponent* PathFollowing = Cast<UCSKit_PathFollowingComponent>(GetPathFollowingComponent());
	if (PathFollowing == nullptr)
	{
		ResultData.Code = EPathFollowingRequestResult::Failed;
		return ResultData;
	}

	if (PathFollowing->IsPlayingDirectMoveControl())
	{
		PathFollowing->SetDirectMoveControlTarget(InTargetPos);
		ResultData.Code = EPathFollowingRequestResult::RequestSuccessful;
		return ResultData;
	}

	FAIMoveRequest MoveReq;
	MoveReq.SetNavigationFilter(GetDefaultNavigationFilterClass());//使わないけど、AAIController::MoveTo()のensureに引っかかるので
	MoveReq.SetProjectGoalLocation(false);
	MoveReq.SetUsePathfinding(false);
	MoveReq.SetGoalLocation(InTargetPos);
	if(InAcceptanceRadius > 0.f)
	{
		MoveReq.SetAcceptanceRadius(InAcceptanceRadius);
	}
	else
	{
		MoveReq.SetAcceptanceRadius(0.f);
		MoveReq.SetReachTestIncludesAgentRadius(false);
		MoveReq.SetReachTestIncludesGoalRadius(false);
	}

	ResultData = MoveTo(MoveReq);

	PathFollowing->RequestDirectMoveControl();
	if (bInFocusNoticeTarget)
	{
		PathFollowing->RequestMoveFocusNoticeTarget();
	}

	return ResultData;
}

/* ------------------------------------------------------------
   !経路探索なしで外部から直接制御する移動中かどうか
------------------------------------------------------------ */
bool ACSKit_AIController::IsMoveToDirectControl() const
{
	const UCSKit_PathFollowingComponent* PathFollowing = Cast<UCSKit_PathFollowingComponent>(GetPathFollowingComponent());
	if (PathFollowing == nullptr)
	{
		return false;
	}

	return PathFollowing->IsPlayingDirectMoveControl();
}

/* ------------------------------------------------------------
   !経路探索なしで外部から直接制御する移動中断
------------------------------------------------------------ */
void ACSKit_AIController::AbortMoveToDirectControl() const
{
	UCSKit_PathFollowingComponent* PathFollowing = Cast<UCSKit_PathFollowingComponent>(GetPathFollowingComponent());
	if (PathFollowing == nullptr)
	{
		return;
	}

	if (PathFollowing->IsPlayingDirectMoveControl())
	{
		PathFollowing->AbortMove(*this, FPathFollowingResultFlags::UserAbort);
	}
}

/* ------------------------------------------------------------
   !非同期のナビメッシュ経路探索リクエスト
------------------------------------------------------------ */
uint32	ACSKit_AIController::MoveToAsync(const FAIMoveRequest& InMoveRequest, const FCSKit_AsyncFindPathFinishDelegate& InFinishDelegate)
{
	const APawn* OwnerPawn = GetPawn();
	const UPathFollowingComponent* PathFollowing = GetPathFollowingComponent();
	if (OwnerPawn == nullptr
		|| PathFollowing == nullptr)
	{
		return INVALID_NAVQUERYID;
	}
	if (PathFollowing->HasReached(InMoveRequest))
	{
		return INVALID_NAVQUERYID;
	}

	if (mAsyncPathQueryId != INVALID_NAVQUERYID)
	{
		//多重リクエスト時は前回のを破棄
		AbortAsyncFindPathRequest(mAsyncPathQueryId);
	}

	const FNavAgentProperties& AgentProperties = OwnerPawn->GetNavAgentPropertiesRef();

	FPathFindingQuery Query;
	const bool bValidQuery = BuildPathfindingQuery(InMoveRequest, Query);
	if (!bValidQuery)
	{
		return INVALID_NAVQUERYID;
	}

	const FNavPathQueryDelegate ResultDelegate = FNavPathQueryDelegate::CreateUObject(this, &ACSKit_AIController::OnFinishFindPathAsync);

	constexpr EPathFindingMode::Type Mode = EPathFindingMode::Regular;

	UNavigationSystemV1* NavigationSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	if (NavigationSystem == nullptr)
	{
		return INVALID_NAVQUERYID;
	}
	mAsyncPathQueryId = NavigationSystem->FindPathAsync(AgentProperties, Query, ResultDelegate, Mode);
	if (mAsyncPathQueryId != INVALID_NAVQUERYID)
	{
		mAsyncFindPathRequest = InMoveRequest;
		mAsyncFindPathFinishDelegate = InFinishDelegate;
	}

	mLastResultAsyncPathQueryId = INVALID_NAVQUERYID;
	mLastAsyncFindPathResult = FPathFollowingRequestResult();

#if USE_CSKIT_DEBUG
	if (UCSKit_PathFollowingComponent* CSKit_PathFollowingComponent = Cast<UCSKit_PathFollowingComponent>(GetPathFollowingComponent()))
	{
		CSKit_PathFollowingComponent->DebugSetFindPathPos(OwnerPawn->GetActorLocation(), InMoveRequest.GetDestination());
	}
#endif

	return mAsyncPathQueryId;
}
/* ------------------------------------------------------------
   !非同期のナビメッシュ経路探索破棄
------------------------------------------------------------ */
void	ACSKit_AIController::AbortAsyncFindPathRequest(const uint32 InAsyncPathQueryId)
{
	if (mAsyncPathQueryId != INVALID_NAVQUERYID
		&& mAsyncPathQueryId == InAsyncPathQueryId)
	{
		if (UNavigationSystemV1* NavigationSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld()))
		{
			NavigationSystem->AbortAsyncFindPathRequest(mAsyncPathQueryId);
		}
		mAsyncPathQueryId = INVALID_NAVQUERYID;
	}
}


/* ------------------------------------------------------------
   !注目対象取得
------------------------------------------------------------ */
AActor* ACSKit_AIController::GetNoticeTarget() const
{
	if (const UCSKit_NoticeComponent* NoticeComponent = GetCSKitNotice())
	{
		return NoticeComponent->GetSelectTarget();
	}
	return nullptr;
}
/* ------------------------------------------------------------
   !AI停止リクエスト
------------------------------------------------------------ */
void ACSKit_AIController::RequestStopAI() const
{
	if (UBehaviorTreeComponent* BehaviorTreeComponent = Cast<UBehaviorTreeComponent>(BrainComponent))
	{
		if (!BehaviorTreeComponent->IsPaused())
		{
			BehaviorTreeComponent->PauseLogic(FString("RequestStopAI"));
		}
	}
}
/* ------------------------------------------------------------
   !AI再開リクエスト
------------------------------------------------------------ */
void ACSKit_AIController::RequestResumeAI() const
{
	if (UBehaviorTreeComponent* BehaviorTreeComponent = Cast<UBehaviorTreeComponent>(BrainComponent))
	{
		if (BehaviorTreeComponent->IsPaused())
		{
			BehaviorTreeComponent->ResumeLogic(FString("RequestStopAI"));
		}
	}
}

/* ------------------------------------------------------------
   !AI停止中
------------------------------------------------------------ */
bool ACSKit_AIController::IsPausedAI() const
{
	if (UBehaviorTreeComponent* BehaviorTreeComponent = Cast<UBehaviorTreeComponent>(BrainComponent))
	{
		return BehaviorTreeComponent->IsPaused();
	}
	return false;
}

/* ------------------------------------------------------------
   !低処理モードon/off
------------------------------------------------------------ */
void ACSKit_AIController::SetLowMode(const bool bInLowMode)
{
	if (mbLowMode == bInLowMode)
	{
		return;
	}
	mbLowMode = bInLowMode;
	mLowModeInterval = GetLowModeIntervalTime();
}

/* ------------------------------------------------------------
   !対象が仲間かどうか
------------------------------------------------------------ */
bool ACSKit_AIController::IsFriend(const AActor* InTarget) const
{
	if (const IGenericTeamAgentInterface* TeamAgentInterface = Cast<IGenericTeamAgentInterface>(GetPawn()))
	{
		const ETeamAttitude::Type TeamAttitudeType = TeamAgentInterface->GetTeamAttitudeTowards(*InTarget);
		return TeamAttitudeType == ETeamAttitude::Friendly;
	}
	return false;
}

/* ------------------------------------------------------------
   !
------------------------------------------------------------ */
void ACSKit_AIController::SetAbility(const FName& InName, const bool bInFlag)
{
	bool& Flag = mAIAbilityMap.FindOrAdd(InName);
	Flag = bInFlag;
}

/* ------------------------------------------------------------
   !
------------------------------------------------------------ */
bool ACSKit_AIController::IsOwnAbility(const FName& InName) const
{
	if(const bool* Flag = mAIAbilityMap.Find(InName))
	{
		return *Flag;
	}
	return false;
}

/* ------------------------------------------------------------
   !
------------------------------------------------------------ */
void ACSKit_AIController::SetAIFlow(const ACSKit_AIFlow* InAIFlow)
{
	if(UCSKit_AIFlowComponent* AIFlowComponent = SafeRegisterAIFlowComponent())
	{
		AIFlowComponent->SetAIFlow(InAIFlow);
	}
}

const ANavigationData* ACSKit_AIController::GetAgentNavigationData() const
{
	if (UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld()))
	{
		const FNavAgentProperties& NavAgentProps = GetNavAgentPropertiesRef();
		return NavSys->GetNavDataForProps(NavAgentProps);
	}
	return nullptr;
}

/* ------------------------------------------------------------
   !
------------------------------------------------------------ */
void ACSKit_AIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	SetHomePosRot();
	if (mDefaultBehaviorTree)
	{
		RunBehaviorTree(mDefaultBehaviorTree);
	}

	if (const IGenericTeamAgentInterface* OwnerTeamAgent = Cast<const IGenericTeamAgentInterface>(GetPawn()))
	{
		SetGenericTeamId(OwnerTeamAgent->GetGenericTeamId());
	}

	// if (UCSKit_EnvironmentAwarenessComponent* EnvironmentAwareness = GetCSKitEnvironmentAwareness())
	// {
	// 	EnvironmentAwareness->OnPossess(InPawn);
	// }
}

/* ------------------------------------------------------------
   !
------------------------------------------------------------ */
void ACSKit_AIController::BeginPlay()
{
	Super::BeginPlay();

	if (const UCSKit_Subsystem* CSKitSubsystem = GetWorld()->GetSubsystem<UCSKit_Subsystem>())
	{
		if (UCSKit_NeedLevelManager* NeedLevelManager = CSKitSubsystem->GetNeedLevelManager())
		{
			NeedLevelManager->EntryTarget(this);
		}
	}

	if (UCSKit_RecognitionComponent* RecognitionComponent = GetCSKitRecognition())
	{
		RecognitionComponent->SetTerritoryComponent(GetCSKitTerritoryComponent());
	}
	if (UCSKit_NoticeComponent* NoticeComponent = GetCSKitNotice())
	{
		NoticeComponent->SetTerritoryComponent(GetCSKitTerritoryComponent());
	}
}

/* ------------------------------------------------------------
   !
------------------------------------------------------------ */
void ACSKit_AIController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (const UCSKit_Subsystem* CSKitSubsystem = GetWorld()->GetSubsystem<UCSKit_Subsystem>())
	{
		if (UCSKit_NeedLevelManager* NeedLevelManager = CSKitSubsystem->GetNeedLevelManager())
		{
			NeedLevelManager->ExitTarget(this);
		}
	}
}

/* ------------------------------------------------------------
   !
------------------------------------------------------------ */
UCSKit_AIFlowComponent* ACSKit_AIController::SafeRegisterAIFlowComponent()
{
	if(UCSKit_AIFlowComponent* AIFlowComponent = GetCSKitAIFlowComponent())
	{
		return AIFlowComponent;
	}
	if(UCSKit_AIFlowComponent* AIFlowComponent = NewObject<UCSKit_AIFlowComponent>(this))
	{
		AIFlowComponent->RegisterComponent();
		mCSKit_AIFlowComponent = AIFlowComponent;
		return AIFlowComponent;
	}
	return nullptr;
}

/* ------------------------------------------------------------
   !非同期のナビメッシュ経路探索終了時処理
   !TaskGraphでUNavigationSystemV1::PerformAsyncQueries()が実行されて、その結果のDelegateをGameThreadに登録して呼び出される
------------------------------------------------------------ */
// ReSharper disable once CppPassValueParameterByConstReference
void	ACSKit_AIController::OnFinishFindPathAsync(uint32 InQueryId, ENavigationQueryResult::Type InResult, FNavPathSharedPtr InPath)
{
	if (mAsyncPathQueryId != InQueryId)
	{
		return;
	}

	FPathFollowingRequestResult Result;
	switch (InResult)
	{
	case ENavigationQueryResult::Error:
		Result.Code = EPathFollowingRequestResult::Failed;
		break;
	case ENavigationQueryResult::Fail:
		Result.Code = EPathFollowingRequestResult::Failed;
		break;
	case ENavigationQueryResult::Success:
	{
		float NavPathGoalActorTetherDistance = 100.f;
		if (const UCSKit_PathFollowingComponent* CSKit_PathFollowingComponent = Cast<UCSKit_PathFollowingComponent>(GetPathFollowingComponent()))
		{
			NavPathGoalActorTetherDistance = CSKit_PathFollowingComponent->GetNavPathGoalActorTetherDistance();
		}
		if (mAsyncFindPathRequest.IsMoveToActorRequest())
		{
			InPath->SetGoalActorObservation(*mAsyncFindPathRequest.GetGoalActor(), NavPathGoalActorTetherDistance);
		}
		const FAIRequestID RequestID = InPath.IsValid() ? RequestMove(mAsyncFindPathRequest, InPath) : FAIRequestID::InvalidRequest;
		if(RequestID == FAIRequestID::InvalidRequest)
		{
			Result.Code = EPathFollowingRequestResult::Failed;
		}
		else
		{
			Result.MoveId = RequestID;
			Result.Code = EPathFollowingRequestResult::RequestSuccessful;
		}
		break;
	}
	default:
		break;
	}

	mLastResultAsyncPathQueryId = mAsyncPathQueryId;
	mLastAsyncFindPathResult = Result;
	mAsyncFindPathFinishDelegate.Execute(Result);
	mAsyncPathQueryId = INVALID_NAVQUERYID;
}

/* ------------------------------------------------------------
  !
------------------------------------------------------------ */
void ACSKit_AIController::SetAIFlowComponent(UCSKit_AIFlowComponent* InComponent)
{
	mCSKit_AIFlowComponent = InComponent;
}

/* ------------------------------------------------------------
  !
------------------------------------------------------------ */
void ACSKit_AIController::AttachTerritoryComponent(const FName& InUserName)
{
	if(UCSKit_TerritoryComponent* TerritoryComponent = GetCSKitTerritoryComponent())
	{
		TerritoryComponent->SetUserName(InUserName);
		return;
	}
	UCSKit_TerritoryComponent* TerritoryComponent = NewObject<UCSKit_TerritoryComponent>(this);
	if(TerritoryComponent == nullptr)
	{
		return;
	}
	TerritoryComponent->SetUserName(InUserName);
	TerritoryComponent->RegisterComponent();
	mCSKit_TerritoryComponent = TerritoryComponent;
	if (UCSKit_RecognitionComponent* RecognitionComponent = GetCSKitRecognition())
	{
		RecognitionComponent->SetTerritoryComponent(GetCSKitTerritoryComponent());
	}
	if (UCSKit_NoticeComponent* NoticeComponent = GetCSKitNotice())
	{
		NoticeComponent->SetTerritoryComponent(GetCSKitTerritoryComponent());
	}
	if (UCSKit_WorryComponent* WorryComponent = GetCSKitWorryComponent())
	{
		WorryComponent->SetTerritoryComponent(GetCSKitTerritoryComponent());
	}
}

/* ------------------------------------------------------------
  !
------------------------------------------------------------ */
void ACSKit_AIController::DetachTerritoryComponent()
{
	UCSKit_TerritoryComponent* TerritoryComponent = GetCSKitTerritoryComponent();
	if(TerritoryComponent == nullptr)
	{
		return;
	}
	TerritoryComponent->UnregisterComponent();
	mCSKit_TerritoryComponent = nullptr;
	if (UCSKit_RecognitionComponent* RecognitionComponent = GetCSKitRecognition())
	{
		RecognitionComponent->SetTerritoryComponent(nullptr);
	}
	if (UCSKit_NoticeComponent* NoticeComponent = GetCSKitNotice())
	{
		NoticeComponent->SetTerritoryComponent(nullptr);
	}
}

/* ------------------------------------------------------------
  !
------------------------------------------------------------ */
void ACSKit_AIController::SetHomePosRot()
{
	if(const APawn* OwnerPawn = GetPawn())
	{
		mHomePos = OwnerPawn->GetActorLocation();
		mHomeRot = OwnerPawn->GetActorRotation();
	}
}

/* ------------------------------------------------------------
  !ActorWatcher選択時
------------------------------------------------------------ */
FString ACSKit_AIController::DebugDrawCallActorWatcher(UCanvas* InCanvas) const
{
#if USE_CSKIT_DEBUG
	return DebugDrawSelectedActorWatcher(InCanvas);
#else
	return FString();
#endif
}

/* ------------------------------------------------------------
   !GhostPlay有効時の入力処理
------------------------------------------------------------ */
void ACSKit_AIController::DebugCallGhostInput(APlayerController* InController)
{
#if USE_CSKIT_DEBUG
	if(InController)
	{
		DebugGhostPlayInput(*InController);
	}
#endif
}

void ACSKit_AIController::DebugCallGhostInputDraw(UCanvas* InCanvas)
{
#if USE_CSKIT_DEBUG
	DebugGhostPlayInputDraw(InCanvas);
#endif
}

#if USE_CSKIT_DEBUG
/* ------------------------------------------------------------
   !詳細ログ取得
------------------------------------------------------------ */
FString ACSKit_AIController::DebugGetDetailLog() const
{
	FString LogString(TEXT("[AIController]\n"));
	LogString += DebugGetDetailLogBase();

	LogString += FString::Printf(TEXT("[PathFollowing]\n"));
	if (const UCSKit_PathFollowingComponent* CSKit_PathFollowingComponent = Cast<UCSKit_PathFollowingComponent>(GetPathFollowingComponent()))
	{
		LogString += CSKit_PathFollowingComponent->DebugGetDetailLog();
	}
	if(const UCSKit_BrainQueryComponent* BrainQueryComponent = GetCSKitBrainQuery())
	{
		LogString += FString::Printf(TEXT("[BrainQuery]\n"));
		for(const auto& MapElement : BrainQueryComponent->DebugGetLastSelectTestMap())
		{
			LogString += FString::Printf(TEXT("   %s - %s\n"), *MapElement.Key.ToString(), *MapElement.Value.ToString());
		}
	}
	if(const UBehaviorTreeComponent* BTComponent = Cast<UBehaviorTreeComponent>(BrainComponent))
	{
		LogString += FString::Printf(TEXT("[BehaviorTree]\n"));
		LogString += BTComponent->GetDebugInfoString();
	}

	return LogString;
}
/* ------------------------------------------------------------
   !テスト用BehaviorTree実行リクエスト
------------------------------------------------------------ */
bool	ACSKit_AIController::DebugTestBehaviorTree(const FString& InFolderPath, const FString& InAssetName, const bool bInLock)
{
	FString Path = InFolderPath;
	Path += FString(TEXT("/"));
	Path += InAssetName;
	Path += FString(TEXT("."));
	Path += InAssetName;

	if (mDebugTestBehaviorTreePath == Path
		|| mbDebugLockBehaviorTree)
	{
		return false;
	}


	const FSoftObjectPath BehaviorTreePath(Path);
	UBehaviorTree* BehaviorTreeObject = Cast<UBehaviorTree>(BehaviorTreePath.ResolveObject());
	if (BehaviorTreeObject == nullptr)
	{
		BehaviorTreePath.TryLoad();
		BehaviorTreeObject = Cast<UBehaviorTree>(BehaviorTreePath.ResolveObject());
	}
	if (BehaviorTreeObject == nullptr)
	{
		return false;
	}
	if (RunBehaviorTree(BehaviorTreeObject))
	{
		mDebugGCObject.mDebugTestBehaviorTree = BehaviorTreeObject;
		if (bInLock)
		{
			mbDebugLockBehaviorTree = true;
		}
		mDebugTestBehaviorTreePath = Path;
		return true;
	}
	return false;
}
/* ------------------------------------------------------------
   !
------------------------------------------------------------ */
void ACSKit_AIController::DebugBeginCollectEQSResult()
{
	mDebugCollectEQSResult = FCSKitDebug_CollectEQSResult();
	if (const APawn* OwnerPawn = GetPawn())
	{
		mDebugCollectEQSResult.SetCenterPos(OwnerPawn->GetActorLocation());
	}
}
/* ------------------------------------------------------------
   !
------------------------------------------------------------ */
void ACSKit_AIController::DebugEndCollectEQSResult(const FString& InResult)
{
	mDebugCollectEQSResult.mResult = InResult;
	mDebugCollectEQSResult.mbCollected = true;
#if USE_EQS_DEBUGGER
	float TimeStamp = 0.f;
	FEnvQueryInstance* QueryInstance = DebugGetLastEnvQueryInstance(TimeStamp);
	if (QueryInstance == nullptr)
	{
		return;
	}
	const bool bUseMidResults = QueryInstance && (QueryInstance->Items.Num() < QueryInstance->DebugData.DebugItems.Num());
	const FEnvQueryDebugData& InstanceDebugData = QueryInstance->DebugData;
	const TArray<FEnvQueryItemDetails>& Details = InstanceDebugData.DebugItemDetails;
	const TArray<FEnvQueryItem>& Items = bUseMidResults ? QueryInstance->DebugData.DebugItems : QueryInstance->Items;
	const TArray<uint8>& RawData = QueryInstance->DebugData.RawData;
	const bool bNoTestsPerformed = QueryInstance->CurrentTest <= 0;
	//const bool bSingleItemResult = QueryInstance->DebugData.bSingleItemResult;

	// float MinScore = 0.f;
	// float MaxScore = -BIG_NUMBER;
	// if (!bSingleItemResult)
	// {
	// 	if (bUseMidResults /*|| HighlightRangePct < 1.0f*/)
	// 	{
	// 		const FEnvQueryItem* ItemInfo = Items.GetData();
	// 		for (int32 ItemIndex = 0; ItemIndex < Items.Num(); ItemIndex++, ItemInfo++)
	// 		{
	// 			if (ItemInfo->IsValid())
	// 			{
	// 				MinScore = FMath::Min(MinScore, ItemInfo->Score);
	// 				MaxScore = FMath::Max(MaxScore, ItemInfo->Score);
	// 			}
	// 		}
	// 	}
	// }

	const auto ItemType = QueryInstance->ItemType;
	if (!ItemType->IsChildOf(UEnvQueryItemType_VectorBase::StaticClass()))
	{
		return;
	}
	const UEnvQueryItemType_VectorBase* DefTypeOb = ItemType->GetDefaultObject<UEnvQueryItemType_VectorBase>();

	EQSDebug::FQueryData DebugItem;
	UEnvQueryDebugHelpers::QueryToDebugData(*QueryInstance, DebugItem, MAX_int32);

	// スコアを求めたアイテム
	for (int32 i = 0; i < Items.Num(); ++i)
	{
		const FEnvQueryItem& Item = Items[i];
		if (Item.IsValid())
		{
			const FVector Pos = DefTypeOb->GetItemLocation(RawData.GetData() + Item.DataOffset);
			const float Score = bNoTestsPerformed ? 1 : Item.Score;

			FCSKitDebug_CollectEQSResultNode ResultNode;
			ResultNode.SetPos(Pos);
			ResultNode.mScore = Score;
			ResultNode.mbWinner = (i == 0);
			const int32 TestNum = DebugItem.Tests.Num();
			if (i < DebugItem.Items.Num())
			{
				const EQSDebug::FItemData& ItemData = DebugItem.Items[i];
				for (int32 TestIndex = 0; TestIndex < TestNum; ++TestIndex)
				{
					const EQSDebug::FTestData& TestData = DebugItem.Tests[TestIndex];
					const FString TestString = FString::Printf(TEXT("[%s] : %.3f(%.3f)"), *TestData.ShortName, ItemData.TestScores[TestIndex], ItemData.TestValues[TestIndex]);
					ResultNode.mTestList.Add(TestString);
				}
			}
			mDebugCollectEQSResult.mNodeList.Add(ResultNode);
		}
	}

	// スコア求まらなかったアイテム表示
	const TArray<FEnvQueryItem>& DebugQueryItems = InstanceDebugData.DebugItems;

	const int32 DebugItemCountLimit = DebugQueryItems.Num() == Details.Num() ? DebugQueryItems.Num() : 0;

	for (int32 i = 0; i < DebugItemCountLimit; ++i)
	{
		if (DebugQueryItems[i].IsValid())
		{
			continue;
		}

		const float Score = bNoTestsPerformed ? 1 : DebugQueryItems[i].Score;
		const FVector Pos = DefTypeOb->GetItemLocation(RawData.GetData() + DebugQueryItems[i].DataOffset);

		FCSKitDebug_CollectEQSResultNode ResultNode;
		ResultNode.SetPos(Pos);
		ResultNode.mScore = Score;

		if (Details[i].FailedTestIndex != INDEX_NONE)
		{
			int32 FailedTestIndex = Details[i].FailedTestIndex;
			//float FailedScore = Details[i].TestResults[FailedTestIndex];

			const FString Label = InstanceDebugData.PerformedTestNames[FailedTestIndex] + FString::Printf(TEXT("(%d)"), FailedTestIndex);
			ResultNode.mFailedLabel = Label;
		}
		mDebugCollectEQSResult.mNodeList.Add(ResultNode);
	}
#endif
}

/* ------------------------------------------------------------
   !
------------------------------------------------------------ */
void ACSKit_AIController::DebugSetGhostPlayMode(const bool bInGhostPlay)
{
	APawn* OwnerPawn = GetPawn();
	if( OwnerPawn == nullptr
		|| mbDebugGhostPlayMode == bInGhostPlay)
	{
		return;
	}

	const UGameInstance* GameInstance = GetWorld()->GetGameInstance();
	if(GameInstance == nullptr)
	{
		return;
	}
	UCSKitDebug_Subsystem* CSKitDebugSubsystem = GameInstance->GetSubsystem<UCSKitDebug_Subsystem>();
	if(CSKitDebugSubsystem == nullptr)
	{
		return;
	}
	
	mbDebugGhostPlayMode = bInGhostPlay;
	if(mbDebugGhostPlayMode)
	{
		CSKitDebugSubsystem->BeginGhostController(OwnerPawn);
	}
	else
	{
		CSKitDebugSubsystem->EndGhostController();
	}
}

/* ------------------------------------------------------------
  !AIControllerの詳細ログ取得
------------------------------------------------------------ */
FString ACSKit_AIController::DebugGetDetailLogBase() const
{
	FString LogString;
	if(const AActor* NoticeTarget = GetNoticeTarget())
	{
		LogString += FString::Printf(TEXT("   NoticeTarget : %s\n"), *NoticeTarget->GetName());
	}
	return LogString;
}

/* ------------------------------------------------------------
  !ActorWatcher選択時
------------------------------------------------------------ */
FString ACSKit_AIController::DebugDrawSelectedActorWatcher(UCanvas* InCanvas) const
{
	FString DebugInfo;
	if (mbLowMode)
	{
		DebugInfo += FString::Printf(TEXT("LowMode(%.1f)\n"), mLowModeInterval);
	}
	DebugInfo += FString::Printf(TEXT("mControlRotationSpeed : %.1f\n"), mControlRotationSpeed);

#if 0
	if (const UCSKitDebug_ActorSelectManager* ActorSelectManager = UCSKitDebug_ActorSelectManager::sGet(GetWorld()))
	{
		if (ActorSelectManager->IsDrawFlag(TEXT("BrainQuery")))
		{
			if (const UCSKit_BrainQueryComponent* BrainQueryComponent = GetCSKitBrainQuery())
			{
				DebugInfo += BrainQueryComponent->DebugDrawSelectedActorWatcher(InCanvas);
			}
		}
		if (ActorSelectManager->IsDrawFlag(TEXT("Experience")))
		{
			if (const UCSKit_ExperienceComponent* ExperienceComponent = GetCSKitExperience())
			{
				DebugInfo += ExperienceComponent->DebugDrawSelectedActorWatcher(InCanvas);
			}
		}
		if (ActorSelectManager->IsDrawFlag(TEXT("Notice")))
		{
			if (const UCSKit_NoticeComponent* NoticeComponent = GetCSKitNotice())
			{
				DebugInfo += NoticeComponent->DebugDrawSelectedActorWatcher(InCanvas);
			}
		}
		if (ActorSelectManager->IsDrawFlag(TEXT("PathFollowing")))
		{
			if (const UCSKit_PathFollowingComponent* PathFollowing = Cast<UCSKit_PathFollowingComponent>(GetPathFollowingComponent()))
			{
				DebugInfo += PathFollowing->DebugDrawSelectedActorWatcher(InCanvas);
			}
		}
		if (ActorSelectManager->IsDrawFlag(TEXT("Recognition")))
		{
			if (const UCSKit_RecognitionComponent* Recognition = GetCSKitRecognition())
			{
				DebugInfo += Recognition->DebugDrawSelectedActorWatcher(InCanvas);
			}
		}
		// if (ActorWatcherManager->IsDrawFlag(TEXT("EnvironmentAwareness")))
		// {
		// 	if (const UCSKit_EnvironmentAwarenessComponent* EnvironmentAwareness = GetCSKitEnvironmentAwareness())
		// 	{
		// 		DebugInfo += EnvironmentAwareness->DebugDrawSelectedActorWatcher(InCanvas);
		// 	}
		// }
		if (ActorSelectManager->IsDrawFlag(TEXT("Territory")))
		{
			if (const UCSKit_TerritoryComponent* TerritoryComponent = GetCSKitTerritoryComponent())
			{
				DebugInfo += TerritoryComponent->DebugDrawSelectedActorWatcher(InCanvas);
			}
		}
		if (ActorSelectManager->IsDrawFlag(TEXT("Worry")))
		{
			if (const UCSKit_WorryComponent* WorryComponent = GetCSKitWorryComponent())
			{
				DebugInfo += WorryComponent->DebugDrawSelectedActorWatcher(InCanvas);
			}
		}
		if (ActorSelectManager->IsDrawFlag(TEXT("AIFlow")))
		{
			if (const UCSKit_AIFlowComponent* AIFlowComponent = GetCSKitAIFlowComponent())
			{
				DebugInfo += AIFlowComponent->DebugDrawSelectedActorWatcher(InCanvas);
			}
		}
		if (ActorSelectManager->IsDrawFlag(TEXT("EQS")))
		{
			DebugInfo += DebugDrawSelectedActorWatcherLastEQS(InCanvas);
		}
	}
#endif

	return DebugInfo;
}

/* ------------------------------------------------------------
   !最終EQS取得
------------------------------------------------------------ */
FEnvQueryInstance*	ACSKit_AIController::DebugGetLastEnvQueryInstance(float& OutTimeStamp) const
{
#if USE_EQS_DEBUGGER
	const APawn* OwnerPawn = GetPawn();
	if (OwnerPawn == nullptr)
	{
		return nullptr;
	}
	UWorld* World = GetWorld();
	UEnvQueryManager* QueryManager = UEnvQueryManager::GetCurrent(World);
	if (QueryManager == nullptr)
	{
		return nullptr;
	}

	const TArray<FEQSDebugger::FEnvQueryInfo>& QueryDataList = QueryManager->GetDebugger().GetAllQueriesForOwner(OwnerPawn);
	int32 LastQueryDataIndex = INDEX_NONE;
	float MaxTimeStamp = 0.f;
	for (int32 i = 0; i < QueryDataList.Num(); ++i)
	{
		const FEQSDebugger::FEnvQueryInfo& Info = QueryDataList[i];
		if (Info.Timestamp > MaxTimeStamp)
		{
			MaxTimeStamp = Info.Timestamp;
			LastQueryDataIndex = i;
		}
	}

	if (LastQueryDataIndex == INDEX_NONE)
	{
		return nullptr;
	}

	OutTimeStamp = MaxTimeStamp;
	return QueryDataList[LastQueryDataIndex].Instance.Get();
#else
	return nullptr;
#endif
}

/* ------------------------------------------------------------
   !EQS情報デバッグ表示
------------------------------------------------------------ */
FString ACSKit_AIController::DebugDrawSelectedActorWatcherLastEQS(UCanvas* InCanvas) const
{
	FString OutString;
#if USE_EQS_DEBUGGER
	float TimeStamp = 0.f;
	FEnvQueryInstance* QueryInstance = DebugGetLastEnvQueryInstance(TimeStamp);
	if (QueryInstance == nullptr)
	{
		return OutString;
	}
	constexpr FColor LineColor_Safe(20, 200, 20);
	constexpr FColor LineColor_Out(20, 20, 200);
	const bool bUseMidResults = QueryInstance && (QueryInstance->Items.Num() < QueryInstance->DebugData.DebugItems.Num());
	const FEnvQueryDebugData& InstanceDebugData = QueryInstance->DebugData;
	const TArray<FEnvQueryItemDetails>& Details = InstanceDebugData.DebugItemDetails;
	const TArray<FEnvQueryItem>& Items = bUseMidResults ? QueryInstance->DebugData.DebugItems : QueryInstance->Items;
	const TArray<uint8>& RawData = QueryInstance->DebugData.RawData;
	const bool bNoTestsPerformed = QueryInstance->CurrentTest <= 0;
	const bool bSingleItemResult = QueryInstance->DebugData.bSingleItemResult;

	float MinScore = 0.f;
	float MaxScore = -BIG_NUMBER;
	if (!bSingleItemResult)
	{
		if (bUseMidResults /*|| HighlightRangePct < 1.0f*/)
		{
			const FEnvQueryItem* ItemInfo = Items.GetData();
			for (int32 ItemIndex = 0; ItemIndex < Items.Num(); ItemIndex++, ItemInfo++)
			{
				if (ItemInfo->IsValid())
				{
					MinScore = FMath::Min(MinScore, ItemInfo->Score);
					MaxScore = FMath::Max(MaxScore, ItemInfo->Score);
				}
			}
		}
	}
	const float ScoreNormalizer = bUseMidResults && (MaxScore != MinScore) ? (1.f / (MaxScore - MinScore)) : 1.f;

	const auto ItemType = QueryInstance->ItemType;
	if (!ItemType->IsChildOf(UEnvQueryItemType_VectorBase::StaticClass()))
	{
		return OutString;
	}
	const UEnvQueryItemType_VectorBase* DefTypeOb = ItemType->GetDefaultObject<UEnvQueryItemType_VectorBase>();

	EQSDebug::FQueryData DebugItem;
	UEnvQueryDebugHelpers::QueryToDebugData(*QueryInstance, DebugItem, MAX_int32);

	constexpr float DrawInfoDistance = 200.f;
	UWorld* World = GetWorld();
	// スコアを求めたアイテムを表示
	for (int32 i = 0; i < Items.Num(); ++i)
	{
		const FEnvQueryItem& Item = Items[i];
		if (Item.IsValid())
		{
			const FVector Pos = DefTypeOb->GetItemLocation(RawData.GetData() + Item.DataOffset);
			const FVector TopPos = Pos + FVector(0.f, 0.f, 50.f);
			FColor LineColor = LineColor_Safe;
			if (!bSingleItemResult)
			{
				const float NormalizedScore = bNoTestsPerformed ? 1 : (Item.Score - MinScore) * ScoreNormalizer;
				LineColor = FColor::MakeRedToGreenColorFromScalar(NormalizedScore);
			}
			UCSKitDebug_Draw::OctahedronArrow OctahedronArrow;
			OctahedronArrow.mBasePos = TopPos;
			OctahedronArrow.mTargetPos = Pos;
			OctahedronArrow.Draw(World, LineColor);

			const float Score = bNoTestsPerformed ? 1 : Item.Score;


			FCSKitDebug_ScreenWindowText ScreenWindowText;
			if (i == 0)
			{
				ScreenWindowText.SetWindowName(FString::Printf(TEXT("Winner")));
			}
			ScreenWindowText.AddText(FString::Printf(TEXT("[%d] Score : %.3f"), i, Score));
			{
				ScreenWindowText.AddText(TEXT("Tests"));
				const int32 TestNum = DebugItem.Tests.Num();
				if (i < DebugItem.Items.Num())
				{
					const EQSDebug::FItemData& ItemData = DebugItem.Items[i];
					for (int32 TestIndex = 0; TestIndex < TestNum; ++TestIndex)
					{
						const EQSDebug::FTestData& TestData = DebugItem.Tests[TestIndex];
						ScreenWindowText.AddText(FString::Printf(TEXT("   [%s] : %.3f(%.3f)"), *TestData.ShortName, ItemData.TestScores[TestIndex], ItemData.TestValues[TestIndex]));
					}
				}
			}
			ScreenWindowText.SetWindowFrameColor(FLinearColor(LineColor));
			ScreenWindowText.Draw(InCanvas, TopPos, DrawInfoDistance);
		}
	}

	// スコア求まらなかったアイテム表示
	const TArray<FEnvQueryItem>& DebugQueryItems = InstanceDebugData.DebugItems;

	const int32 DebugItemCountLimit = DebugQueryItems.Num() == Details.Num() ? DebugQueryItems.Num() : 0;

	for (int32 i = 0; i < DebugItemCountLimit; ++i)
	{
		if (DebugQueryItems[i].IsValid())
		{
			continue;
		}

		FColor LineColor = LineColor_Out;

		const float Score = bNoTestsPerformed ? 1 : DebugQueryItems[i].Score;
		const FVector Pos = DefTypeOb->GetItemLocation(RawData.GetData() + DebugQueryItems[i].DataOffset);
		const FVector TopPos = Pos + FVector(0.f, 0.f, 50.f);

		UCSKitDebug_Draw::OctahedronArrow OctahedronArrow;
		OctahedronArrow.mBasePos = TopPos;
		OctahedronArrow.mTargetPos = Pos;
		OctahedronArrow.Draw(World, LineColor);

		FCSKitDebug_ScreenWindowText ScreenWindowText;
		ScreenWindowText.AddText(FString::Printf(TEXT("[%d] Score : %.3f"), i, Score));

		if (Details[i].FailedTestIndex != INDEX_NONE)
		{
			int32 FailedTestIndex = Details[i].FailedTestIndex;
			//float FailedScore = Details[i].TestResults[FailedTestIndex];

			const FString Label = InstanceDebugData.PerformedTestNames[FailedTestIndex] + FString::Printf(TEXT("(%d)"), FailedTestIndex);
			ScreenWindowText.AddText(FString::Printf(TEXT("%s"), *Label));
		}

		ScreenWindowText.SetWindowFrameColor(FLinearColor(LineColor));
		ScreenWindowText.Draw(InCanvas, TopPos, DrawInfoDistance);
	}
#endif

	return OutString;
}
#endif//USE_CSKIT_DEBUG
