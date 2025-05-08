// Copyright 2020 megasuraman.
/**
 * @file CSKit_WorryComponent.cpp
 * @brief Actor心配管理Component(AIPerceptionComponentのHearing代わり)
 * @author megasuraman
 * @date 2025/05/06
 */
#include "AI/Worry/CSKit_WorryComponent.h"

#include "AI/CSKit_AIController.h"
#include "CSKit_Subsystem.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/Character.h"
#include "AI/Territory/CSKit_TerritoryComponent.h"
#include "AI/Worry/CSKit_WorryDataTable.h"
#include "AI/Worry/CSKit_WorrySourceManager.h"

#if USE_CSKIT_DEBUG
#include "CSKitDebug_Draw.h"
#include "ScreenWindow/CSKitDebug_ScreenWindowText.h"
#endif

UCSKit_WorryComponent::UCSKit_WorryComponent()
	:mGiveUpWorryPos(FAISystem::InvalidLocation)
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UCSKit_WorryComponent::BeginPlay()
{
	Super::BeginPlay();
}

/* ------------------------------------------------------------
   !
------------------------------------------------------------ */
void UCSKit_WorryComponent::Setup(const FCSKit_WorryAbilityTableRow& InData)
{
	mAbility = InData;;
}

/* ------------------------------------------------------------
   !
------------------------------------------------------------ */
void UCSKit_WorryComponent::SetTerritoryComponent(const UCSKit_TerritoryComponent* InComponent)
{
	mTerritoryComponent = InComponent;
}

/* ------------------------------------------------------------
   !更新処理(Tickは使わず、CSKit_AIControllerから呼ぶ)
------------------------------------------------------------ */
void UCSKit_WorryComponent::Update(const float InDeltaSec)
{
	if(IsSleep())
	{
		return;
	}
	const ACSKit_AIController* AIController = Cast<ACSKit_AIController>(GetOwner());
	if(AIController == nullptr)
	{
		return;
	}
	if(AIController->GetNoticeTarget() != nullptr)
	{
		SetWishCheckWorryPoint(false);
		return;
	}

	const APawn* OwnerPawn = AIController->GetPawn();
	const UCSKit_WorrySourceManager* WorrySourceManager = UCSKit_WorrySourceManager::sGet(GetWorld());
	if( OwnerPawn == nullptr
		|| WorrySourceManager == nullptr)
	{
		return;
	}

	UpdateGiveUpSec(InDeltaSec);
	
	const FVector OwnerPos = OwnerPawn->GetActorLocation();
	if(mbWishCheckWorryPoint
		&& FVector::DistSquared(OwnerPos,mWorryPoint.mPos) < FMath::Square(mWorryPoint.mRadius))
	{
		SetWishCheckWorryPoint(false);
	}
	
	TArray<FCSKit_WorrySource> WorrySourceList;
	WorrySourceManager->CollectTouchSource(WorrySourceList, OwnerPos, mAbility.mCatchRadius, mAbility.mCatchHeight);
	float DistanceMinSq = FLT_MAX;
	if(mbWishCheckWorryPoint)
	{
		DistanceMinSq = FVector::Distance(mWorryPoint.mPos,OwnerPos);
		DistanceMinSq += -100.f;
		DistanceMinSq = FMath::Square(DistanceMinSq);
	}
	bool bSelectedWorrySource = false;
	FVector WorrySourcePos = FVector::ZeroVector;
	float WorryRadius = 0.f;
	TWeakObjectPtr<const AActor> SourceActor = nullptr;
	for(const FCSKit_WorrySource& WorrySource : WorrySourceList)
	{
		const float DistanceSq = FVector::DistSquared(OwnerPos,WorrySource.mPos);
		if(DistanceSq < DistanceMinSq
			&& !IsNearGiveUpPos(WorrySource.mPos)
			&& !IsOutsideTerritory(WorrySource.mPos))
		{
			DistanceMinSq = DistanceSq;
			WorrySourcePos = WorrySource.mPos;
			WorryRadius = WorrySource.mRadius;
			SourceActor = WorrySource.mSourceActor;
			bSelectedWorrySource = true;
		}
	}
	if(mAbility.mVisibilityDistance > 0.f
		&& mAbility.mVisibilityAngle > 0.f)
	{
		mCheckVisibleInterval += InDeltaSec;
		if(mCheckVisibleInterval > mCheckVisibleIntervalTime)
		{
			mCheckVisibleInterval = 0.f;
			const FVector VisibleWorryPos = CheckVisibleWorrySource(DistanceMinSq);
			if(VisibleWorryPos != FAISystem::InvalidLocation)
			{
				WorrySourcePos = VisibleWorryPos;
				bSelectedWorrySource = true;
			}
		}
	}

	if(!bSelectedWorrySource)
	{
		return;
	}

	mWorryPoint.mPos = WorrySourcePos;
	mWorryPoint.mRadius = FMath::Max(WorryRadius, 100.f);
	mWorryPoint.mSourceActor = SourceActor;
	SetWishCheckWorryPoint(true);
}

/* ------------------------------------------------------------
  !気になってる座標
------------------------------------------------------------ */
FVector UCSKit_WorryComponent::GetWishCheckPos() const
{
	if(IsWishCheckWorryPoint())
	{
		return mWorryPoint.mPos;
	}
	return FAISystem::InvalidLocation;
}

void UCSKit_WorryComponent::SetSleep(const bool bInSleep)
{
	if(mbSleep == bInSleep)
	{
		return;
	}
	mbSleep = bInSleep;
	if(mbSleep)
	{
		SetWishCheckWorryPoint(false);
	}
}

/* ------------------------------------------------------------
  !
------------------------------------------------------------ */
void UCSKit_WorryComponent::GiveUpWorry()
{
	if(!IsWishCheckWorryPoint())
	{
		return;
	}

	mGiveUpWorryPos = mWorryPoint.mPos;
	mGiveUpInterval = 5.f;
	SetWishCheckWorryPoint(false);
}

/* ------------------------------------------------------------
  !
------------------------------------------------------------ */
void UCSKit_WorryComponent::SetWishCheckWorryPoint(const bool bInWish)
{
	if(mbWishCheckWorryPoint == bInWish)
	{
		return;
	}
	mbWishCheckWorryPoint = bInWish;
	if(mbWishCheckWorryPoint)
	{
		OnBeginWishCheckWorryPoint();
	}
	else
	{
		OnEndWishCheckWorryPoint();
	}
}

/* ------------------------------------------------------------
  !視界での心配要素チェック(UCSKit_RecognitionComponentの範囲外)
------------------------------------------------------------ */
FVector UCSKit_WorryComponent::CheckVisibleWorrySource(const float InBorderDistanceSq) const
{
	const ACSKit_AIController* Owner = Cast<ACSKit_AIController>(GetOwner());
	const UCSKit_Subsystem* CSKitSubsystem = GetWorld()->GetSubsystem<UCSKit_Subsystem>();
	if (Owner == nullptr
		|| CSKitSubsystem == nullptr)
	{
		return FAISystem::InvalidLocation;
	}

	const APawn* OwnerPawn = Owner->GetPawn();
	if (OwnerPawn == nullptr)
	{
		return FAISystem::InvalidLocation;
	}

	const FTransform OwnerTransform = OwnerPawn->GetActorTransform();
	const FVector OwnerPos = OwnerPawn->GetActorLocation();
	//const FTransform OwnerTransform(OwnerPawn->GetActorTransform());
	const float VisibilityDistanceSq = FMath::Square(mAbility.mVisibilityDistance);
	float MinDistanceSq = FMath::Min(VisibilityDistanceSq, InBorderDistanceSq);
	FVector NearestTargetPos = FAISystem::InvalidLocation;
	const TArray<TWeakObjectPtr<AActor>>& TargetList = CSKitSubsystem->GetRecognitionTargetList();
	for (const auto& WeakPtr : TargetList)
	{
		AActor* Target = WeakPtr.Get();
		if (Target == nullptr
			|| Target == OwnerPawn
			|| Owner->IsFriend(Target)
			)
		{
			continue;
		}

		const FVector TargetPos = Target->GetActorLocation();
		const float TargetDistanceSq = FVector::DistSquared(OwnerPos, TargetPos);
		if (TargetDistanceSq > MinDistanceSq
			|| IsNearGiveUpPos(TargetPos))
		{
			continue;
		}
		
		const FVector TargetV = OwnerTransform.InverseTransformPosition(TargetPos);
		if (FMath::Abs(TargetV.Rotation().Yaw) > mAbility.mVisibilityAngle)
		{
			continue;
		}

		if(IsHideTarget(Target)
			|| !CheckCollisionVisible(OwnerPos, Target))
		{
			continue;
		}

		MinDistanceSq = TargetDistanceSq;
		NearestTargetPos = TargetPos;
	}
	return NearestTargetPos;
}
/* ------------------------------------------------------------
  !コリジョン的に見えるかどうか
------------------------------------------------------------ */
bool UCSKit_WorryComponent::CheckCollisionVisible(const FVector& InBasePos, AActor* InTarget) const
{
	FVector TargetPos = InTarget->GetActorLocation();
	if(const ACharacter* TargetCharacter = Cast<ACharacter>(InTarget))
	{
		TargetPos = TargetCharacter->GetPawnViewLocation();
	}
	if (!GetWorld()->LineTraceTestByChannel(InBasePos, TargetPos, ECC_Visibility))
	{
		return true;
	}
	return false;
}

/* ------------------------------------------------------------
  !
------------------------------------------------------------ */
bool UCSKit_WorryComponent::IsNearGiveUpPos(const FVector& InTargetPos) const
{
	if(mGiveUpWorryPos == FAISystem::InvalidLocation)
	{
		return false;
	}
	return (FVector::DistSquared(mGiveUpWorryPos,InTargetPos) < FMath::Square(1000.f));
}

/* ------------------------------------------------------------
  !
------------------------------------------------------------ */
void UCSKit_WorryComponent::UpdateGiveUpSec(const float InDeltaSec)
{
	if(mGiveUpInterval > 0.f)
	{
		mGiveUpInterval -= InDeltaSec;
		if(mGiveUpInterval <= 0.f)
		{
			mGiveUpWorryPos = FAISystem::InvalidLocation;	
		}
	}
}

/* ------------------------------------------------------------
   !
------------------------------------------------------------ */
bool UCSKit_WorryComponent::IsOutsideTerritory(const FVector& InTargetPos) const
{
	if(const UCSKit_TerritoryComponent* TerritoryComponent = mTerritoryComponent.Get())
	{
		if(TerritoryComponent->IsOwnTerritoryVolume()
			&& !TerritoryComponent->IsTouchTerritoryVolume(InTargetPos, 1.f))
		{
			return true;
		}
	}
	return false;
}

#if USE_CSKIT_DEBUG
/* ------------------------------------------------------------
  !ActorWatcher選択時
------------------------------------------------------------ */
FString UCSKit_WorryComponent::DebugDrawSelectedActorWatcher(UCanvas* InCanvas) const
{
	FString DebugInfo;

	DebugDrawAbilityRange();
	DebugDrawWorryPoint(InCanvas);
	if(mGiveUpWorryPos != FAISystem::InvalidLocation)
	{
		DebugDrawGiveUpWorryPos(InCanvas);
	}

	return DebugInfo;
}
/* ------------------------------------------------------------
   !能力範囲デバッグ表示
------------------------------------------------------------ */
void	UCSKit_WorryComponent::DebugDrawAbilityRange() const
{
	const AAIController* Owner = Cast<AAIController>(GetOwner());
	if (Owner == nullptr)
	{
		return;
	}
	const APawn* OwnerPawn = Owner->GetPawn();
	if (OwnerPawn == nullptr)
	{
		return;
	}

	const FVector BasePos = OwnerPawn->GetActorLocation();
	const FMatrix CircleMatrix(FQuatRotationTranslationMatrix(FQuat(FVector(0.f, 1.f, 0.f), PI*0.5f), BasePos));
	DrawDebugCircle(
		GetWorld(),
		CircleMatrix,
		mAbility.mCatchRadius,
		32,
		FColor::Green,
		false,
		-1.f,
		0,
		3.f
	);
}

/* ------------------------------------------------------------
   !認識対象デバッグ表示
------------------------------------------------------------ */
void UCSKit_WorryComponent::DebugDrawWorryPoint(UCanvas* InCanvas) const
{
	const AAIController* Owner = Cast<AAIController>(GetOwner());
	if (Owner == nullptr)
	{
		return;
	}
	const APawn* OwnerPawn = Owner->GetPawn();
	if (OwnerPawn == nullptr)
	{
		return;
	}

	const FVector BasePos = OwnerPawn->GetActorLocation();
	FColor LineColor = FColor::Blue;
	float Thickness = 1.f;
	if(mbWishCheckWorryPoint)
	{
		LineColor = FColor::Orange;
		Thickness = 3.f;
	}

	DrawDebugLine(GetWorld(), BasePos, mWorryPoint.mPos, LineColor, false, -1.f, 0, Thickness);
	
	const FMatrix CircleMatrix(FQuatRotationTranslationMatrix(FQuat(FVector(0.f, 1.f, 0.f), PI*0.5f), mWorryPoint.mPos));
	DrawDebugCircle(
		GetWorld(),
		CircleMatrix,
		mWorryPoint.mRadius,
		32,
		LineColor,
		false,
		-1.f,
		0,
		Thickness
		);
}

/* ------------------------------------------------------------
   !
------------------------------------------------------------ */
void UCSKit_WorryComponent::DebugDrawGiveUpWorryPos(UCanvas* InCanvas) const
{
	UCSKitDebug_Draw::OctahedronArrow OctahedronArrow;
	OctahedronArrow.mBasePos = mGiveUpWorryPos + FVector(0.f,0.f,100.f);
	OctahedronArrow.mTargetPos = mGiveUpWorryPos;
	OctahedronArrow.Draw(GetWorld(), FColor::Blue);

	FCSKitDebug_ScreenWindowText ScreenWindowText;
	ScreenWindowText.SetWindowName(FString::Printf(TEXT("GiveUpWorryPos")));
	ScreenWindowText.AddText(FString::Printf(TEXT("%s"), *mGiveUpWorryPos.ToString()));
	ScreenWindowText.AddText(FString::Printf(TEXT("mGiveUpInterval : %.1f"), mGiveUpInterval));
	ScreenWindowText.SetWindowFrameColor(FColor::Blue);
	ScreenWindowText.Draw(InCanvas, OctahedronArrow.mBasePos);
}
#endif
