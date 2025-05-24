// Copyright 2020 megasuraman.
/**
 * @file CSKit_BTTask_StopMove.cpp
 * @brief 追跡Task(単純に対象にMoveToだと機能不足なので)
 * @author megasuraman
 * @date 2025/05/24
 */
#include "AI/BehaviorTree/Tasks/CSKit_BTTask_Chase.h"

#include "AI/CSKit_AIController.h"
#include "AI/CSKit_PathFollowingComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Float.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
//#include "AI/EnvironmentMap/CSKit_EnvironmentAwarenessComponent.h"
#include "AI/Recognition/CSKit_RecognitionComponent.h"


UCSKit_BTTask_Chase::UCSKit_BTTask_Chase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, mbAllowPartialPath(false)
	, mbUseNavmesh(true)
	, mbOverrideTaskFinishRadius(false)
	, mbUseTargetPredictMovePos(false)
{
	NodeName = TEXT("CSKit_Chase");
	bNotifyTick = true;
	bNotifyTaskFinished = true;

	mBBKey_OverrideFinishTaskRadius.AddFloatFilter(this, GET_MEMBER_NAME_CHECKED(UCSKit_BTTask_Chase, mBBKey_OverrideFinishTaskRadius));
	mBBKey_TargetPredictMovePosSec.AddFloatFilter(this, GET_MEMBER_NAME_CHECKED(UCSKit_BTTask_Chase, mBBKey_TargetPredictMovePosSec));
}

/* ------------------------------------------------------------
   !FBlackboardKeySelectorの初期化のために
------------------------------------------------------------ */
void UCSKit_BTTask_Chase::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);

	if (const UBlackboardData* BBAsset = GetBlackboardAsset())
	{
		mBBKey_OverrideFinishTaskRadius.ResolveSelectedKey(*BBAsset);
		mBBKey_TargetPredictMovePosSec.ResolveSelectedKey(*BBAsset);
	}
	else
	{
		mBBKey_OverrideFinishTaskRadius.InvalidateResolvedKey();
		mBBKey_TargetPredictMovePosSec.InvalidateResolvedKey();
	}
}
/* ------------------------------------------------------------
   !Task開始
------------------------------------------------------------ */
EBTNodeResult::Type UCSKit_BTTask_Chase::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	FCSKit_ChaseTaskMemory* MyMemory = CastInstanceNodeMemory<FCSKit_ChaseTaskMemory>(NodeMemory);
	MyMemory->mOldNavigateGoalPos = FAISystem::InvalidLocation;
	MyMemory->mMoveRequestID = FAIRequestID();
	MyMemory->mRevengeIntervalSec = 0.f;
	MyMemory->mRevengeCount = 0;
	MyMemory->mbOriginalGoalInNavNullSpace = false;
	MyMemory->mbDirectMove = false;
	MyMemory->mbResultPathDirect = false;

	return RequestChase(OwnerComp, NodeMemory);
}
/* ------------------------------------------------------------
   !Task中断
------------------------------------------------------------ */
EBTNodeResult::Type UCSKit_BTTask_Chase::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	FCSKit_ChaseTaskMemory* MyMemory = CastInstanceNodeMemory<FCSKit_ChaseTaskMemory>(NodeMemory);
	MyMemory->mMoveRequestID = FAIRequestID();
	return Super::AbortTask(OwnerComp, NodeMemory);
}
/* ------------------------------------------------------------
   !実行時のパラメータ表示？
------------------------------------------------------------ */
void UCSKit_BTTask_Chase::DescribeRuntimeValues(const UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTDescriptionVerbosity::Type Verbosity, TArray<FString>& Values) const
{
	Super::DescribeRuntimeValues(OwnerComp, NodeMemory, Verbosity, Values);
}
/* ------------------------------------------------------------
   !静的なパラメータ表示？
------------------------------------------------------------ */
FString UCSKit_BTTask_Chase::GetStaticDescription() const
{
	const FString KeyDesc("invalid");

	return FString::Printf(TEXT("%s: %s"), *Super::GetStaticDescription(), *KeyDesc);
}

/* ------------------------------------------------------------
   !タスク更新
------------------------------------------------------------ */
void UCSKit_BTTask_Chase::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	UpdateChase(OwnerComp, NodeMemory, DeltaSeconds);
}

/* ------------------------------------------------------------
   !追跡リクエスト
------------------------------------------------------------ */
EBTNodeResult::Type UCSKit_BTTask_Chase::RequestChase(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ACSKit_AIController* MyController = Cast<ACSKit_AIController>(OwnerComp.GetAIOwner());
	if (MyController == nullptr)
	{
		return EBTNodeResult::Failed;
	}
	const AActor* TargetActor = MyController->GetNoticeTarget();
	if (TargetActor == nullptr)
	{
	return EBTNodeResult::Failed;
	}
	if (!MyController->IsPossibleToMoveStart())
	{
		return EBTNodeResult::InProgress;
	}

	OnRequestMove(OwnerComp, NodeMemory);
	FCSKit_ChaseTaskMemory* TaskMemory = CastInstanceNodeMemory<FCSKit_ChaseTaskMemory>(NodeMemory);
	if (mbUseNavmesh)
	{
		if (IsTargetSameGroundSpace(*MyController))
		{
			return RequestChaseDirect(*MyController, *TargetActor, TaskMemory);
		}
		return RequestChaseNavigate(*MyController, *TargetActor, TaskMemory);
	}
	else
	{
		return RequestChaseDirect(*MyController, *TargetActor, TaskMemory);
	}
}

/* ------------------------------------------------------------
   !追跡リクエスト(経路探索なし)
------------------------------------------------------------ */
EBTNodeResult::Type UCSKit_BTTask_Chase::RequestChaseDirect(ACSKit_AIController& InAIController, const AActor& InTargetActor, FCSKit_ChaseTaskMemory* InTaskMemory)
{
	const APawn* OwnerPawn = InAIController.GetPawn();
	if (OwnerPawn == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	if (CheckNearTarget(InAIController, InTargetActor))
	{
		return EBTNodeResult::Succeeded;
	}

	constexpr float GoalRadius = 50.f;//GetTaskFinishRadius(InAIController);
	const FPathFollowingRequestResult RequestResult = InAIController.MoveToDirect(&InTargetActor, GoalRadius);
	if (RequestResult.Code == EPathFollowingRequestResult::AlreadyAtGoal)
	{
		return EBTNodeResult::Succeeded;
	}
	InTaskMemory->mbDirectMove = true;
	//InTaskMemory->mbResultPathDirect = false; UpdateChaseNavigate()から一時的に呼ぶこともあるので
	return EBTNodeResult::InProgress;
}

/* ------------------------------------------------------------
   !追跡リクエスト(経路探索あり)
------------------------------------------------------------ */
EBTNodeResult::Type UCSKit_BTTask_Chase::RequestChaseNavigate(ACSKit_AIController& InAIController, const AActor& InTargetActor, FCSKit_ChaseTaskMemory* InTaskMemory)
{
	const APawn* OwnerPawn = InAIController.GetPawn();
	if (OwnerPawn == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	if (CheckNearTarget(InAIController, InTargetActor))
	{
	return EBTNodeResult::Succeeded;
	}

	FAIMoveRequest MoveReq;
	MoveReq.SetNavigationFilter(InAIController.GetDefaultNavigationFilterClass());
	MoveReq.SetAllowPartialPath(mbAllowPartialPath);
	MoveReq.SetAcceptanceRadius(5.f);
	MoveReq.SetCanStrafe(false);
	MoveReq.SetReachTestIncludesAgentRadius(true);
	MoveReq.SetReachTestIncludesGoalRadius(true);
	MoveReq.SetProjectGoalLocation(!mbAllowPartialPath);//ナビメッシュ無いと探索失敗になって辿れるところまでの経路が取得できないので
	MoveReq.SetUsePathfinding(true);

	const FVector GoalPos = FindBestTargetGoalPos(InTargetActor);
	MoveReq.SetGoalLocation(GoalPos);

	if (const UCSKit_PathFollowingComponent* PathFollowingComponent = Cast<UCSKit_PathFollowingComponent>(InAIController.GetPathFollowingComponent()))
	{
		const FVector OldGoalPos = InTaskMemory->mOldNavigateGoalPos;
		if (OldGoalPos != FAISystem::InvalidLocation
			&& FVector::DistSquared2D(OldGoalPos, GoalPos) < FMath::Square(PathFollowingComponent->GetNavPathGoalActorTetherDistance())
			)
		{
			//移動中のゴールと差がないならスルー
			return EBTNodeResult::InProgress;
		}
	}

	const FPathFollowingRequestResult RequestResult = InAIController.MoveTo(MoveReq);
	if (RequestResult.Code == EPathFollowingRequestResult::RequestSuccessful)
	{
		InTaskMemory->mOldNavigateGoalPos = GoalPos;
		InTaskMemory->mMoveRequestID = RequestResult.MoveId;
		InTaskMemory->mRevengeIntervalSec = 0.f;
		InTaskMemory->mRevengeCount = 0;
		InTaskMemory->mbDirectMove = false;

		if (const UCSKit_PathFollowingComponent* PathFollowingComponent = Cast<UCSKit_PathFollowingComponent>(InAIController.GetPathFollowingComponent()))
		{
			InTaskMemory->mbResultPathDirect = PathFollowingComponent->IsStraightPath();
		}	
		return EBTNodeResult::InProgress;
	}
	else if (RequestResult.Code == EPathFollowingRequestResult::AlreadyAtGoal)
	{
		return EBTNodeResult::Succeeded;
	}

	//探索に失敗しても何度かリベンジする
	//ここでターゲットの足元コリジョンチェックすべき？
	//自分の足元コリジョンもチェックすべき？
	if (InTaskMemory->mRevengeCount < mMaxRevengeNum)
	{
		InTaskMemory->mRevengeCount += 1;
		InTaskMemory->mRevengeIntervalSec = mRevengeIntervalTime;
		return EBTNodeResult::InProgress;
	}

	return EBTNodeResult::Failed;
}

/* ------------------------------------------------------------
   !追跡更新
------------------------------------------------------------ */
void UCSKit_BTTask_Chase::UpdateChase(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	if (mbUseNavmesh)
	{
		if (const ACSKit_AIController* MyController = Cast<ACSKit_AIController>(OwnerComp.GetAIOwner()))
		{
			const FCSKit_ChaseTaskMemory* TaskMemory = CastInstanceNodeMemory<FCSKit_ChaseTaskMemory>(NodeMemory);
			if (IsTargetSameGroundSpace(*MyController))
			{
				UpdateChaseDirect(OwnerComp, NodeMemory, DeltaSeconds);
				return;
			}
			else if (TaskMemory->mbDirectMove
					 && !TaskMemory->mbResultPathDirect)
			{
				MyController->AbortMoveToDirect(MyController->GetNoticeTarget());
			}
		}

		const EBTNodeResult::Type Result = UpdateChaseNavigate(OwnerComp, NodeMemory, DeltaSeconds);
		if (Result == EBTNodeResult::Succeeded)
		{
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		}
		else if (Result == EBTNodeResult::Failed)
		{
			FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		}
	}
	else
	{
		UpdateChaseDirect(OwnerComp, NodeMemory, DeltaSeconds);
	}
}

/* ------------------------------------------------------------
   !追跡更新(経路探索なし)
------------------------------------------------------------ */
void UCSKit_BTTask_Chase::UpdateChaseDirect(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	const EBTNodeResult::Type ResultType = RequestChase(OwnerComp, NodeMemory);
	if (ResultType != EBTNodeResult::InProgress)
	{
		FinishLatentTask(OwnerComp, ResultType);
	}
}

/* ------------------------------------------------------------
   !追跡更新(経路探索あり)
------------------------------------------------------------ */
EBTNodeResult::Type UCSKit_BTTask_Chase::UpdateChaseNavigate(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	ACSKit_AIController* AIController = Cast<ACSKit_AIController>(OwnerComp.GetAIOwner());
	if (AIController == nullptr)
	{
		return EBTNodeResult::Failed;
	}
	const AActor* TargetActor = AIController->GetNoticeTarget();
	if (TargetActor == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	const UCSKit_PathFollowingComponent* PathFollowingComponent = Cast<UCSKit_PathFollowingComponent>(AIController->GetPathFollowingComponent());
	if (PathFollowingComponent == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	FCSKit_ChaseTaskMemory* TaskMemory = CastInstanceNodeMemory<FCSKit_ChaseTaskMemory>(NodeMemory);
	const FVector OldGoalPos = TaskMemory->mOldNavigateGoalPos;
	if (OldGoalPos == FAISystem::InvalidLocation)
	{
		return RequestChaseNavigate(*AIController, *TargetActor, TaskMemory);
	}

	if (CheckNearTarget(*AIController, *TargetActor))
	{
		return EBTNodeResult::Succeeded;
	}

	const FVector BestTargetGoalPos = FindBestTargetGoalPos(*TargetActor);
	if (FVector::DistSquared2D(OldGoalPos, BestTargetGoalPos) > FMath::Square(PathFollowingComponent->GetNavPathGoalActorTetherDistance()))
	{
		return RequestChaseNavigate(*AIController, *TargetActor, TaskMemory);
	}
	else if(TaskMemory->mbResultPathDirect)
	{
		return RequestChaseDirect(*AIController, *TargetActor, TaskMemory);
	}

	return EBTNodeResult::InProgress;
}

/* ------------------------------------------------------------
   !対象を追跡するための最適座標取得
------------------------------------------------------------ */
FVector UCSKit_BTTask_Chase::FindBestTargetGoalPos(const AActor& InTarget) const
{
	return InTarget.GetActorLocation();
}

/* ------------------------------------------------------------
   !対象が近い
------------------------------------------------------------ */
bool UCSKit_BTTask_Chase::CheckNearTarget(ACSKit_AIController& InAIController, const AActor& InTargetActor) const
{
	const APawn* OwnerPawn = InAIController.GetPawn();
	if (OwnerPawn == nullptr)
	{
		return EBTNodeResult::Failed;
	}
	const float CheckRadius = GetCheckNearTargetRadius(InAIController);
	const FVector OwnerPos = OwnerPawn->GetActorLocation();
	FVector TargetPos = InTargetActor.GetActorLocation();
	if(mbUseTargetPredictMovePos)
	{
		float TargetPredictSec = 0.5f;
		if (const UBlackboardComponent* BlackboardComponent = InAIController.GetBlackboardComponent())
		{
			TargetPredictSec = BlackboardComponent->GetValue<UBlackboardKeyType_Float>(mBBKey_TargetPredictMovePosSec.GetSelectedKeyID());
		}
		if(const ACharacter* TargetCharacter = Cast<ACharacter>(&InTargetActor))
		{
			if(const UCharacterMovementComponent* TargetMovement = TargetCharacter->GetCharacterMovement())
			{
				TargetPos += TargetMovement->Velocity * TargetPredictSec;
			}
		}
	}

	if (FMath::Abs(OwnerPos.Z - TargetPos.Z) > 200.f
		|| FVector::DistSquaredXY(OwnerPos, TargetPos) > FMath::Square(CheckRadius)
		)
	{
		return false;
	}

#if 1//見えてない場合は壁越しとする
	if (const UCSKit_RecognitionComponent* RecognitionComponent = InAIController.GetCSKitRecognition())
	{
		if(const FCSKitRecognitionTarget* RecognitionTarget = RecognitionComponent->FindRecognitionTarget(&InTargetActor))
		{
			if( !RecognitionTarget->mbRecognize)
			{
				return false;
			}
		}
	}
#else
	//壁越しに近寄ったことになると困るけど、ここでコリジョンチェックするのも微妙
	FComponentQueryParams CollisionParams(FName("CheckNearTarget"));
	CollisionParams.AddIgnoredActor(OwnerPawn);
	CollisionParams.AddIgnoredActor(&InTargetActor);
	FHitResult HitResult;
	if (GetWorld()->LineTraceSingleByChannel(HitResult, OwnerPos, TargetPos, ECC_WorldStatic, CollisionParams))
	{
		//DrawDebugLine(GetWorld(), OwnerPos, HitResult.ImpactPoint, FColor::Red, false, -1.f, 255, 3.f);
		return false;
	}
#endif

	return true;
}

/* ------------------------------------------------------------
   !タスク終了半径取得
------------------------------------------------------------ */
float UCSKit_BTTask_Chase::GetTaskFinishRadius(const ACSKit_AIController& InAIController) const
{
	float FinishTaskRadius = mTaskFinishDistance;
	if (mbOverrideTaskFinishRadius)
	{
		if (const UBlackboardComponent* BlackboardComponent = InAIController.GetBlackboardComponent())
		{
			FinishTaskRadius = BlackboardComponent->GetValue<UBlackboardKeyType_Float>(mBBKey_OverrideFinishTaskRadius.GetSelectedKeyID());
		}
	}
	return FinishTaskRadius;
}

/* ------------------------------------------------------------
   !ターゲットが同じ空間に居るか
------------------------------------------------------------ */
bool UCSKit_BTTask_Chase::IsTargetSameGroundSpace(const ACSKit_AIController& InAIController)
{
	// if (const UCSKit_EnvironmentAwarenessComponent* EnvironmentAwareness = InAIController.GetCSKitEnvironmentAwareness())
	// {
	// 	return EnvironmentAwareness->IsMatchTargetSameSpace();
	// }
	return false;
}
