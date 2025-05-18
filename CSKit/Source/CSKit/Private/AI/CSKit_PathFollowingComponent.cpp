// Copyright 2020 megasuraman.
/**
 * @file CSKit_PathFollowingComponent.cpp
 * @brief 慣性ブレーキや方向基準での移動機能等を追加
 * @author megasuraman
 * @date 2025/05/06
 */
#include "AI/CSKit_PathFollowingComponent.h"

#include "AI/CSKit_AIController.h"
#include "DrawDebugHelpers.h"
#include "NavigationSystem.h"
#include "NavLinkCustomComponent.h"

#if USE_CSKIT_DEBUG
#include "CSKitDebug_Draw.h"
#include "ScreenWindow/CSKitDebug_ScreenWindowText.h"
#endif

UCSKit_PathFollowingComponent::UCSKit_PathFollowingComponent(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
	, mbFocusBasedMoveMode(false)
	, mbFocusBasedMoveVelocityXY(false)
	, mbOwnInertial(false)
	, mbDirectMoveToActor(false)
	, mbDirectMoveControl(false)
	, mbMoveFocusNoticeTarget(false)
	, mbBreakManualMoveFocusRotationSpeed(false)
{
}

/* ------------------------------------------------------------
   !更新
------------------------------------------------------------ */
void UCSKit_PathFollowingComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	if (mbDirectMoveToActor
		&& GetStatus() == EPathFollowingStatus::Moving)
	{
		UpdateDirectMoveToActor();
	}

	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if(mbOwnInertial)
	{
		if(GetStatus() == EPathFollowingStatus::Moving)
		{
#if ENGINE_MAJOR_VERSION == 5
			if(const INavMovementInterface* NavMovement = NavMovementInterface.Get())
			{
				mInertialSpeedV = NavMovement->GetVelocityForNavMovement();
			}
#else
			if(MovementComp != nullptr)
			{
				mInertialSpeedV = MovementComp->Velocity;
			}
#endif
		}
		else if(!mInertialSpeedV.IsZero())
		{
			UpdateInertialBreak(DeltaTime);
		}
	}
}

/* ------------------------------------------------------------
   !移動リクエスト
------------------------------------------------------------ */
FAIRequestID UCSKit_PathFollowingComponent::RequestMove(const FAIMoveRequest& RequestData, FNavPathSharedPtr InPath)
{
	mbDirectMoveToActor = false;
	mbDirectMoveControl = false;
	mbMoveFocusNoticeTarget = false;
	mManualMoveFocusRotationSpeed = 0.f;
	mbBreakManualMoveFocusRotationSpeed = false;
	mInertialSpeedV = FVector::ZeroVector;
	return Super::RequestMove(RequestData, InPath);
}

/* ------------------------------------------------------------
   !パス移動終了時
------------------------------------------------------------ */
void UCSKit_PathFollowingComponent::OnPathFinished(const FPathFollowingResult& Result)
{
#if USE_CSKIT_DEBUG
	if (Result.Code == EPathFollowingResult::Blocked)
	{
		mDebugBlockedPathStartPos = mDebugFindPathStartPos;
		mDebugBlockedPathGoalPos = mDebugFindPathGoalPos;
		if (const FNavigationPath* PathInstance = Path.Get())
		{
			const int32 PathPointNum = PathInstance->GetPathPoints().Num();
			if (PathPointNum >= 2)
			{
				mDebugBlockedPathStartPos = PathInstance->GetPathPoints()[0];
				mDebugBlockedPathGoalPos = PathInstance->GetPathPoints()[PathPointNum - 1];
			}
		}
	}
	else
	{
		mDebugBlockedPathStartPos = FVector::ZeroVector;
		mDebugBlockedPathGoalPos = FVector::ZeroVector;
	}
#endif

	Super::OnPathFinished(Result);
}

/* ------------------------------------------------------------
   !移動中の注視対象
------------------------------------------------------------ */
FVector UCSKit_PathFollowingComponent::GetMoveFocus(bool bAllowStrafe) const
{
	if(mbFocusBasedMoveMode)
	{
		return mManualMoveFocusPos;
	}
	else if (mbMoveFocusNoticeTarget)
	{
		if (const ACSKit_AIController* AIController = Cast<ACSKit_AIController>(GetOwner()))
		{
			if (const AActor* NoticeTarget = AIController->GetNoticeTarget())
			{
				return NoticeTarget->GetActorLocation();
			}
		}
	}

	FVector FocusPos = Super::GetMoveFocus(bAllowStrafe);

	if(mPossibleToFocusYawAngleList.Num() != 0)
	{
		float SelectYaw = 0.f;
		ReviseFocusPos(FocusPos, SelectYaw);
	}

	return FocusPos;
}

/* ------------------------------------------------------------
   !指定Actorへの直接移動中かどうか
------------------------------------------------------------ */
bool UCSKit_PathFollowingComponent::IsPlayingDirectMoveToActor(const AActor* InTarget) const
{
	if (mbDirectMoveToActor
		&& GetStatus() == EPathFollowingStatus::Moving
		&& GetMoveGoal() == InTarget)
	{
		return true;
	}
	return false;
}

/* ------------------------------------------------------------
   !外部から直接移動先を変更
------------------------------------------------------------ */
void UCSKit_PathFollowingComponent::SetDirectMoveControlTarget(const FVector& InTarget)
{
	if (!IsPlayingDirectMoveControl())
	{
		return;
	}

	const ACSKit_AIController* AIController = Cast<ACSKit_AIController>(GetOwner());
	FNavigationPath* PathInstance = Path.Get();
	if (AIController == nullptr
		|| PathInstance == nullptr)
	{
		return;
	}
	const APawn* OwnerPawn = AIController->GetPawn();
	const int32 PathPointNum = PathInstance->GetPathPoints().Num();
	if (OwnerPawn == nullptr
		|| PathPointNum != 2)
	{
		return;
	}

	const FVector BasePos = OwnerPawn->GetActorLocation();
	const FVector TargetPos = InTarget;
	PathInstance->GetPathPoints()[0].Location = BasePos;
	PathInstance->GetPathPoints()[PathPointNum - 1].Location = TargetPos;
	CurrentDestination.Position = TargetPos;
	MoveSegmentDirection = (TargetPos - BasePos).GetSafeNormal();
	mbBreakManualMoveFocusRotationSpeed = false;
}

/* ------------------------------------------------------------
   !外部から直接移動制御するモードかどうか
------------------------------------------------------------ */
bool UCSKit_PathFollowingComponent::IsPlayingDirectMoveControl() const
{
	if (mbDirectMoveControl
		&& GetStatus() == EPathFollowingStatus::Moving)
	{
		return true;
	}
	return false;
}

/* ------------------------------------------------------------
   !最終ゴール座標取得
------------------------------------------------------------ */
FVector UCSKit_PathFollowingComponent::GetGoalPos() const
{
	if (GetStatus() != EPathFollowingStatus::Moving)
	{
		return FAISystem::InvalidLocation;
	}
	const FNavigationPath* PathInstance = Path.Get();
	if (PathInstance == nullptr)
	{
		return FAISystem::InvalidLocation;
	}

	const int32 PathPointNum = PathInstance->GetPathPoints().Num();
	if (PathPointNum < 1)
	{
		return FAISystem::InvalidLocation;
	}

	return PathInstance->GetPathPoints()[PathPointNum - 1].Location;
}

/* ------------------------------------------------------------
   !直進するパスかどうか
------------------------------------------------------------ */
bool UCSKit_PathFollowingComponent::IsStraightPath() const
{
	if (GetStatus() != EPathFollowingStatus::Moving)
	{
		return false;
	}
	const FNavigationPath* PathInstance = Path.Get();
	if (PathInstance == nullptr)
	{
		return false;
	}

	const int32 PathPointNum = PathInstance->GetPathPoints().Num();
	return (PathPointNum <= 2);
}

/**
 * @brief 
 */
void UCSKit_PathFollowingComponent::OverrideInertial(const bool bInOwnInertial, const float InInertialBreak)
{
	mbOwnInertial = bInOwnInertial;
	mInertialBreak = InInertialBreak;
}

/**
 * @brief 
 */
void UCSKit_PathFollowingComponent::FollowPathSegment(float DeltaTime)
{
	if(mbFocusBasedMoveMode)
	{
		FollowPathSegmentFocusBasedMoveMode(DeltaTime);
		return;
	}
	
	Super::FollowPathSegment(DeltaTime);
}

/**
 * @brief 
 */
void UCSKit_PathFollowingComponent::SetMoveSegment(int32 SegmentStartIndex)
{
	Super::SetMoveSegment(SegmentStartIndex);

	//mManualMoveFocusRotationSpeed = 0.f;
	mbBreakManualMoveFocusRotationSpeed = false;
	if(mJointPathNodeAcceptanceRadius > 0.f)
	{
		CheckOverrideJointPathNodeAcceptanceRadius();
	}
}

/**
 * @brief 
 */
#if ENGINE_MAJOR_VERSION == 5
bool UCSKit_PathFollowingComponent::ShouldStopMovementOnPathFinished() const
{
	if(mbOwnInertial
		&& !mInertialSpeedV.IsZero())
	{
		return false;
	}
	return Super::ShouldStopMovementOnPathFinished();
}
#else
bool UCSKit_PathFollowingComponent::ShouldCheckPathOnResume() const
{
	if(mbOwnInertial
		&& !mInertialSpeedV.IsZero())
	{
		return false;
	}
	return Super::ShouldCheckPathOnResume();
}
#endif

/* ------------------------------------------------------------
   !指定のActorへ直接移動するための更新処理
------------------------------------------------------------ */
void UCSKit_PathFollowingComponent::UpdateDirectMoveToActor()
{
	const AActor* TargetActor = GetMoveGoal();
	const ACSKit_AIController* AIController = Cast<ACSKit_AIController>(GetOwner());
	FNavigationPath* PathInstance = Path.Get();
	if (TargetActor == nullptr
		|| AIController == nullptr
		|| PathInstance == nullptr)
	{
		return;
	}
	const APawn* OwnerPawn = AIController->GetPawn();
	const int32 PathPointNum = PathInstance->GetPathPoints().Num();
	if (OwnerPawn == nullptr
		|| PathPointNum != 2)
	{
		return;
	}

	const FVector BasePos = OwnerPawn->GetActorLocation();
	const FVector TargetPos = TargetActor->GetActorLocation();
	PathInstance->GetPathPoints()[0].Location = BasePos;
	PathInstance->GetPathPoints()[PathPointNum - 1].Location = TargetPos;
	CurrentDestination.Position = TargetPos;
	MoveSegmentDirection = (TargetPos - BasePos).GetSafeNormal();
}

/**
 * @brief 
 */
void UCSKit_PathFollowingComponent::FollowPathSegmentFocusBasedMoveMode(const float InDeltaTime)
{
#if ENGINE_MAJOR_VERSION == 5
	INavMovementInterface* NavMovement = NavMovementInterface.Get();
	if (!Path.IsValid()
		|| NavMovement == nullptr
		|| InDeltaTime <= 0.f)
	{
		return;
	}
#else
	UNavMovementComponent* NavMovement = MovementComp;
	if (!Path.IsValid()
		|| NavMovement == nullptr
		|| InDeltaTime <= 0.f)
	{
		return;
	}
#endif

	float SelectReviseYaw = 0.f;
	UpdateManualMoveFocusPos(SelectReviseYaw, InDeltaTime);

#if ENGINE_MAJOR_VERSION == 5
	const FVector CurrentLocation = NavMovement->GetFeetLocation();
#else
	const FVector CurrentLocation = NavMovement->GetActorFeetLocation();
#endif
	//const FVector CurrentTarget = GetCurrentTargetLocation();
	const FVector CurrentTarget = mManualMoveFocusPos;
	
	// set to false by default, we will set this back to true if appropriate
	bIsDecelerating = false;

	FVector MoveVelocity = (CurrentTarget - CurrentLocation) / InDeltaTime;
	if(SelectReviseYaw != 0.f)
	{
		MoveVelocity = FRotator(0.f, SelectReviseYaw, 0.f).RotateVector(MoveVelocity);
	}
	if(mbFocusBasedMoveVelocityXY)
	{
		MoveVelocity.Z = 0.f;
	}
	ReviseMoveVelocityOnFocusBasedMoveMode(MoveVelocity, InDeltaTime);

	const int32 LastSegmentStartIndex = Path->GetPathPoints().Num() - 2;
	const bool bNotFollowingLastSegment = (MoveSegmentStartIndex < LastSegmentStartIndex);

	PostProcessMove.ExecuteIfBound(this, MoveVelocity);
	NavMovement->RequestDirectMove(MoveVelocity, bNotFollowingLastSegment);
}

/**
 * @brief 
 */
void UCSKit_PathFollowingComponent::UpdateManualMoveFocusPos(float& OutYaw, const float InDeltaTime)
{
	if (Status != EPathFollowingStatus::Moving
		|| !Path.IsValid())
	{
		return;
	}
	
	ACSKit_AIController* AIController = Cast<ACSKit_AIController>(GetOwner());
	if(AIController == nullptr)
	{
		return;
	}
	const APawn* OwnerPawn = AIController->GetPawn();
	if(OwnerPawn == nullptr)
	{
		return;
	}
	const FVector CurrentTarget = GetCurrentTargetLocation();
	const float RotationSpeedMax = GetManualMoveFocusRotationSpeed();
	const float RotationSpeedMin = RotationSpeedMax*0.2f;
	const float RotationAccel = RotationSpeedMax;
	if(mbBreakManualMoveFocusRotationSpeed)
	{
		mManualMoveFocusRotationSpeed -= RotationAccel*InDeltaTime;
	}
	else
	{
		mManualMoveFocusRotationSpeed += RotationAccel*InDeltaTime;
	}
	mManualMoveFocusRotationSpeed = FMath::Clamp(mManualMoveFocusRotationSpeed, RotationSpeedMin, RotationSpeedMax);

	FVector TargetPos = CurrentTarget;
	if(mPossibleToFocusYawAngleList.Num() != 0)
	{
		ReviseFocusPos(TargetPos, OutYaw);
	}
	const FTransform BaseTransform = OwnerPawn->GetActorTransform();
	const FVector LocalTargetPos = BaseTransform.InverseTransformPosition(TargetPos);
	const float LocalTargetYaw = LocalTargetPos.Rotation().Yaw;
	const float RotateValue = mManualMoveFocusRotationSpeed * InDeltaTime;

	constexpr float BeginBreakTime = 0.2f;
	if(mManualMoveFocusRotationSpeed > RotationSpeedMin
		&& FMath::Abs(LocalTargetYaw) < mManualMoveFocusRotationSpeed*BeginBreakTime)
	{
		mbBreakManualMoveFocusRotationSpeed = true;
	}
	
	if(FMath::Abs(LocalTargetYaw) > RotateValue)
	{
		float LocalGoalYaw = RotateValue;
		if(LocalTargetYaw < 0.f)
		{
			LocalGoalYaw = -RotateValue;
		}
		const FVector LocalGoalPos = FRotator(0.f,LocalGoalYaw,0.f).RotateVector(FVector::ForwardVector*500.f);
		TargetPos = BaseTransform.TransformPosition(LocalGoalPos);
	}
	else
	{
		mManualMoveFocusRotationSpeed = 0.f;
	}
	mManualMoveFocusPos = TargetPos;
	mManualMoveFocusPos.Z = CurrentTarget.Z;

	//基底のUpdateMoveFocus()は毎フレームは実行されないので
	AIController->SetFocalPoint(mManualMoveFocusPos, EAIFocusPriority::Move);
}

/**
 * @brief 
 */
void UCSKit_PathFollowingComponent::CheckOverrideJointPathNodeAcceptanceRadius()
{
	const FNavigationPath* PathInstance = Path.Get();
	if (PathInstance == nullptr)
	{
		return;
	}
	if (MoveSegmentEndIndex < PathInstance->GetPathPoints().Num())
	{
		CurrentAcceptanceRadius = mJointPathNodeAcceptanceRadius;
	}
}

/**
 * @brief 
 */
void UCSKit_PathFollowingComponent::UpdateInertialBreak(const float InDeltaTime)
{
	if(mInertialSpeedV.IsNearlyZero())
	{
		mInertialSpeedV = FVector::ZeroVector;
		return;
	}
	const FVector InertialAccelNV = -mInertialSpeedV.GetSafeNormal();
	mInertialSpeedV += InertialAccelNV * mInertialBreak * InDeltaTime;
	if(FVector::DotProduct(InertialAccelNV, mInertialSpeedV.GetSafeNormal()) >= 0.f)
	{
		mInertialSpeedV = FVector::ZeroVector;
		return;
	}
	
	//PostProcessMove.ExecuteIfBound(this, mInertialSpeedV);
	
#if ENGINE_MAJOR_VERSION == 5
	if(INavMovementInterface* NavMovement = NavMovementInterface.Get())
	{
		NavMovement->RequestDirectMove(mInertialSpeedV, false);
	}
#else
	MovementComp->RequestDirectMove(mInertialSpeedV, false);
#endif
}

/**
 * @brief 
 */
void UCSKit_PathFollowingComponent::ReviseFocusPos(FVector& OutPos, float& OutYaw) const
{
	const ACSKit_AIController* AIController = Cast<ACSKit_AIController>(GetOwner());
	if (AIController == nullptr)
	{
		return;
	}
	const APawn* OwnerPawn = AIController->GetPawn();
	if(OwnerPawn == nullptr)
	{
		return;
	}
	const FTransform BaseTransform = OwnerPawn->GetActorTransform();
	const FVector LocalFocusPos = BaseTransform.InverseTransformPosition(OutPos);
	const float LocalYaw = LocalFocusPos.Rotation().Yaw;
	float SelectYaw = 0.f;
	float MinDiffAngle = 180.f;
	for(const float YawAngle : mPossibleToFocusYawAngleList)
	{
		FRotator DiffRot(0.f, LocalYaw - YawAngle, 0.f);
		DiffRot.Normalize();
		const float DiffAngle = FMath::Abs(DiffRot.Yaw);
		if(DiffAngle < MinDiffAngle)
		{
			MinDiffAngle = DiffAngle;
			SelectYaw = YawAngle;
		}
	}
	FVector ReviseFocusPos = FRotator(0.f,SelectYaw,0.f).RotateVector(LocalFocusPos);
	ReviseFocusPos = BaseTransform.TransformPosition(ReviseFocusPos);
	OutPos = ReviseFocusPos;
	OutYaw = SelectYaw;
}

#if USE_CSKIT_DEBUG
/* ------------------------------------------------------------
  !ActorWatcher選択時
------------------------------------------------------------ */
FString UCSKit_PathFollowingComponent::DebugDrawSelectedActorWatcher(UCanvas* InCanvas) const
{
	FString DebugInfo;

	DebugInfo += FString::Printf(TEXT("[PathFollowing]\n"));
	FString StatusString;
	switch (GetStatus())
	{
	case EPathFollowingStatus::Idle:
		StatusString = FString(TEXT("Idle"));
		break;
	case EPathFollowingStatus::Waiting:
		StatusString = FString(TEXT("Waiting"));
		break;
	case EPathFollowingStatus::Paused:
		StatusString = FString(TEXT("Paused"));
		break;
	case EPathFollowingStatus::Moving:
		StatusString = FString(TEXT("Moving"));
		break;
	default:
		break;
	}
	DebugInfo += FString::Printf(TEXT("   Status : %s\n"), *StatusString);
	DebugInfo += FString::Printf(TEXT("   mbDirectMoveToActor : %d\n"), mbDirectMoveToActor);
	DebugInfo += FString::Printf(TEXT("   mbDirectMoveControl : %d\n"), mbDirectMoveControl);
	DebugInfo += FString::Printf(TEXT("   mbMoveFocusNoticeTarget : %d\n"), mbMoveFocusNoticeTarget);

	if(mbFocusBasedMoveMode)
	{
		DebugInfo += FString::Printf(TEXT("   mbFocusBasedMoveMode : %d\n"), mbFocusBasedMoveMode);
		DebugInfo += FString::Printf(TEXT("      mManualMoveFocusRotationSpeed : %.2f\n"), mManualMoveFocusRotationSpeed);
		DebugInfo += FString::Printf(TEXT("      mbBreakManualMoveFocusRotationSpeed : %d\n"), mbBreakManualMoveFocusRotationSpeed);
#if ENGINE_MAJOR_VERSION == 5
		if(const INavMovementInterface* NavMovement = NavMovementInterface.Get())
		{
			DrawDebugLine(GetWorld(), NavMovement->GetLocation(), mManualMoveFocusPos, FColor::Orange, false, -1.f, 0, 3);
		}
#else
		DrawDebugLine(GetWorld(), MovementComp->GetActorLocation(), mManualMoveFocusPos, FColor::Orange, false, -1.f, 0, 3);
#endif
	}

	if(mbOwnInertial)
	{
		DebugInfo += FString::Printf(TEXT("   mbOwnInertial : %d\n"), mbOwnInertial);
		DebugInfo += FString::Printf(TEXT("      mInertialSpeedV : %s(%.2f)\n"), *mInertialSpeedV.ToString(), mInertialSpeedV.Size());
	}

	DebugDrawPath(InCanvas);

	return DebugInfo;
}

/* ------------------------------------------------------------
   !詳細ログ取得
------------------------------------------------------------ */
FString UCSKit_PathFollowingComponent::DebugGetDetailLog() const
{
	FString LogString;
	if(GetStatus() == EPathFollowingStatus::Moving)
	{
		LogString += FString::Printf(TEXT("   IsMoving\n"));
		LogString += FString::Printf(TEXT("   GoalPos : %s\n"), *GetGoalPos().ToString());
		LogString += FString::Printf(TEXT("   CurrentTargetPos : %s\n"), *GetCurrentTargetLocation().ToString());
	}
	return LogString;
}

/* ------------------------------------------------------------
  !パスデバッグ表示
------------------------------------------------------------ */
void UCSKit_PathFollowingComponent::DebugDrawPath(UCanvas* InCanvas) const
{
	if (GetStatus() != EPathFollowingStatus::Moving)
	{
		return;
	}
	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	if (NavSys == nullptr)
	{
		return;
	}

	const FNavigationPath* PathInstance = GetPath().Get();
	if (PathInstance == nullptr)
	{
		return;
	}

	constexpr bool bShowDetail = true;
	constexpr FColor PassagePathColor(20, 20, 255);
	constexpr FColor PlanPathColor(255, 165, 20);
	const int32 PathPointNum = PathInstance->GetPathPoints().Num();
	constexpr int32 NeedPointNum = 2;
	const int32 CurrentPathIndex = static_cast<int32>(GetCurrentPathIndex());
	const int32 NextPathIndex = static_cast<int32>(GetNextPathIndex());
	if (PathPointNum < NeedPointNum
		|| CurrentPathIndex >= PathPointNum
		|| NextPathIndex >= PathPointNum
		)
	{
		return;
	}


	FVector BeginLocation = PathInstance->GetPathPoints()[0].Location;
	DrawDebugLine(GetWorld(), BeginLocation, BeginLocation + FVector(0.f, 0.f, 100.f), PassagePathColor);

	//const float OctahedronRadius = 10.f;
	const FVector OctahedronBasePosOffset(0.f, 0.f, 80.f);
	UCSKitDebug_Draw::OctahedronArrow DrawCurrentNode;
	DrawCurrentNode.mBasePos = PathInstance->GetPathPoints()[CurrentPathIndex].Location + OctahedronBasePosOffset;
	DrawCurrentNode.mTargetPos = PathInstance->GetPathPoints()[CurrentPathIndex].Location;
	DrawCurrentNode.Draw(GetWorld(), PassagePathColor);

	UCSKitDebug_Draw::OctahedronArrow DrawNextNode;
	DrawNextNode.mBasePos = PathInstance->GetPathPoints()[NextPathIndex].Location + OctahedronBasePosOffset;
	DrawNextNode.mTargetPos = PathInstance->GetPathPoints()[NextPathIndex].Location;
	DrawNextNode.Draw(GetWorld(), PlanPathColor);

	if (bShowDetail)
	{
		FCSKitDebug_ScreenWindowText ScreenWindowText;
		ScreenWindowText.AddText(FString::Printf(TEXT("Index : %d"), 0));
		ScreenWindowText.AddText(FString::Printf(TEXT("pos(%s)"), *BeginLocation.ToString()));
		ScreenWindowText.SetWindowFrameColor(FColor(150, 200, 200));
		ScreenWindowText.Draw(InCanvas, BeginLocation);
	}

	for (int32 i = 0; i + 1 < PathPointNum; ++i)
	{
		const FNavPathPoint& PathPoint = PathInstance->GetPathPoints()[i];
		const FNavPathPoint& NextPathPoint = PathInstance->GetPathPoints()[i + 1];
		const FVector EndLocation = NextPathPoint.Location;
		FColor LineColor = PlanPathColor;
		if (i < CurrentPathIndex)
		{
			LineColor = PassagePathColor;
		}

#if WITH_EDITOR
		if (i == CurrentPathIndex)
		{
			TArray<FVector> NavMeshEdgePosList;
			//RecastNavMesh->DebugGetNavNodeVertex(NavMeshEdgePosList, NextPathPoint.NodeRef);
			for (int32 Index = 0; Index < NavMeshEdgePosList.Num(); Index += 2)
			{
				const FVector BeginPos = NavMeshEdgePosList[Index];
				const FVector EndPos = NavMeshEdgePosList[Index + 1];
				DrawDebugLine(GetWorld(), BeginPos, EndPos, FColor::Red, false, -1.f, 255, 5.f);
			}
		}
#endif

#if ENGINE_MAJOR_VERSION == 5
		if (PathPoint.CustomNavLinkId != FNavLinkId::Invalid)
		{
			if (const INavLinkCustomInterface* CustomNavLink = NavSys->GetCustomLink(PathPoint.CustomNavLinkId))
#else
		if (PathPoint.CustomLinkId != 0)
		{
			if (const INavLinkCustomInterface* CustomNavLink = NavSys->GetCustomLink(PathPoint.CustomLinkId))
#endif
			{
				if (const UNavLinkCustomComponent* CustomComponent = Cast<UNavLinkCustomComponent>(CustomNavLink))
				{
					FVector LeftPos;
					FVector RightPos;
					ENavLinkDirection::Type Direction;
					CustomComponent->GetLinkData(LeftPos, RightPos, Direction);
					DrawDebugLine(GetWorld(), LeftPos, RightPos, FColor::White);
				}
			}
		}
		else
		{
			DrawDebugLine(GetWorld(), BeginLocation, EndLocation, LineColor);
		}
		DrawDebugLine(GetWorld(), EndLocation, EndLocation + FVector(0.f, 0.f, 100.f), LineColor);

		if (bShowDetail)
		{
			FCSKitDebug_ScreenWindowText ScreenWindowText;
			ScreenWindowText.AddText(FString::Printf(TEXT("Index : %d"), i + 1));
			ScreenWindowText.AddText(FString::Printf(TEXT("pos(%s)"), *EndLocation.ToString()));
			ScreenWindowText.SetWindowFrameColor(FColor(150, 200, 200));
			ScreenWindowText.Draw(InCanvas, EndLocation);
		}

		BeginLocation = EndLocation;
	}
}
#endif