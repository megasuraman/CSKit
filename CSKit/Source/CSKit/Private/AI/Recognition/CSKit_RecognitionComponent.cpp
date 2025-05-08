// Copyright 2020 megasuraman.
/**
 * @file CSKit_RecognitionComponent.cpp
 * @brief Actor認識管理Component(AIPerceptionComponentの代わり)
 * @author megasuraman
 * @date 2025/05/05
 */
#include "AI/Recognition/CSKit_RecognitionComponent.h"

#include "AI/CSKit_AIController.h"
#include "CSKit_Subsystem.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"
#include "AI/Recognition/CSKit_RecognitionDataTable.h"
#include "AI/Territory/CSKit_TerritoryComponent.h"

#if USE_CSKIT_DEBUG
#include "CSKitDebug_Draw.h"
#include "ScreenWindow/CSKitDebug_ScreenWindowText.h"
#endif

UCSKit_RecognitionComponent::UCSKit_RecognitionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	mRecognitionTargetList.Reserve(mMaxRecognitionTargetListSize);
}

void UCSKit_RecognitionComponent::BeginPlay()
{
	Super::BeginPlay();
	mRecognitionTargetList.Reserve(mMaxRecognitionTargetListSize);
}

/* ------------------------------------------------------------
   !
------------------------------------------------------------ */
void UCSKit_RecognitionComponent::Setup(const FCSKit_RecognitionTableRow& InData)
{
	SetRecognizeAbility(InData.mRecognizeAbility);
}

void UCSKit_RecognitionComponent::SetTerritoryComponent(const UCSKit_TerritoryComponent* InComponent)
{
	mTerritoryComponent = InComponent;
}

/* ------------------------------------------------------------
   !更新処理(Tickは使わず、CSKit_AIControllerから呼ぶ)
------------------------------------------------------------ */
void UCSKit_RecognitionComponent::Update(const float InDeltaSec)
{
	UpdateRecognizeTarget(InDeltaSec);
}

/* ------------------------------------------------------------
   !ダメージ受けたとき
------------------------------------------------------------ */
void UCSKit_RecognitionComponent::OnTakeDamage(AActor* InDamageCauser, const float InDamage)
{
	AActor* DamageCauser = ReplaceTarget(InDamageCauser);
	if(DamageCauser == nullptr)
	{
		return;
	}
	if(IsFriend(DamageCauser))
	{
		return;
	}
	if (FCSKitRecognitionTarget* RecognitionTarget = FindRecognitionTarget(DamageCauser))
	{
		RecognitionTarget->mTotalDamage += InDamage;
		RecognitionTarget->mRecentDamage += InDamage;
		RecognitionTarget->mRecentDamageDownInterval = mRecognizeAbility.mRecentDamageDownIntervalTime;
		RecognitionTarget->mbLastRecognitionTakeDamage = true;
		return;
	}

	AddRecognitionTarget(DamageCauser);
	if (FCSKitRecognitionTarget* RecognitionTarget = FindRecognitionTarget(DamageCauser))
	{
		RecognitionTarget->mTotalDamage = InDamage;
		RecognitionTarget->mRecentDamage = InDamage;
		RecognitionTarget->mRecentDamageDownInterval = mRecognizeAbility.mRecentDamageDownIntervalTime;
		RecognitionTarget->mLastRecognitionFlags = 0;
		RecognitionTarget->mbLastRecognitionTakeDamage = true;
	}
}

/* ------------------------------------------------------------
   !他者からの共有を受けた
------------------------------------------------------------ */
void UCSKit_RecognitionComponent::OnTakeMessage(AActor* InTarget)
{
	if(mbIgnoreMessage)
	{
		return;
	}
	AActor* DamageCauser = ReplaceTarget(InTarget);
	if(DamageCauser == nullptr)
	{
		return;
	}
	if(IsFriend(DamageCauser))
	{
		return;
	}
	
	if (FCSKitRecognitionTarget* RecognitionTarget = FindRecognitionTarget(DamageCauser))
	{
		RecognitionTarget->mbLastRecognitionTakeMessage = true;
		return;
	}

	AddRecognitionTarget(DamageCauser);
	if (FCSKitRecognitionTarget* RecognitionTarget = FindRecognitionTarget(DamageCauser))
	{
		RecognitionTarget->mLastRecognitionFlags = 0;
		RecognitionTarget->mbLastRecognitionTakeMessage = true;
	}
}

/* ------------------------------------------------------------
   !リストから認識対象情報を取得
------------------------------------------------------------ */
const FCSKitRecognitionTarget* UCSKit_RecognitionComponent::FindRecognitionTarget(const AActor* InTarget) const
{
	for (const FCSKitRecognitionTarget& Target : mRecognitionTargetList)
	{
		if (Target.mActor.Get() == InTarget)
		{
			return &Target;
		}
	}
	return nullptr;
}

/* ------------------------------------------------------------
  !
------------------------------------------------------------ */
void UCSKit_RecognitionComponent::SendMessage(AActor* InTarget, const TArray<ACSKit_AIController*>& InReceiverList)
{
	if(InTarget == nullptr)
	{
		return;
	}
	for(const ACSKit_AIController* AIController : InReceiverList)
	{
		if(UCSKit_RecognitionComponent* RecognitionComponent = AIController->GetCSKitRecognition())
		{
			RecognitionComponent->OnTakeMessage(InTarget);
		}
	}
}

/* ------------------------------------------------------------
   !Actorを認識対象のActorに変換(ダメージ与えてきた弾本体じゃなくて所有者にするとか)
------------------------------------------------------------ */
AActor* UCSKit_RecognitionComponent::ReplaceTarget(AActor* InTarget) const
{
	return InTarget;
}

/* ------------------------------------------------------------
   !対象が仲間かどうか
------------------------------------------------------------ */
bool UCSKit_RecognitionComponent::IsFriend(const AActor* InTarget) const
{
	if(const ACSKit_AIController* Owner = Cast<ACSKit_AIController>(GetOwner()))
	{
		return Owner->IsFriend(InTarget);
	}
	return false;
}

/* ------------------------------------------------------------
   !認識対象更新
------------------------------------------------------------ */
void UCSKit_RecognitionComponent::UpdateRecognizeTarget(const float InDeltaTime)
{
	const ACSKit_AIController* Owner = Cast<ACSKit_AIController>(GetOwner());
	const UCSKit_Subsystem* CSKitSubsystem = GetWorld()->GetSubsystem<UCSKit_Subsystem>();
	if (Owner == nullptr
		|| CSKitSubsystem == nullptr)
	{
		return;
	}

	const APawn* OwnerPawn = Owner->GetPawn();
	if (OwnerPawn == nullptr)
	{
		return;
	}

	const FVector OwnerPos = OwnerPawn->GetPawnViewLocation();
	FTransform OwnerTransform(OwnerPawn->GetActorTransform());
	if(mOffsetFrontYaw != 0.f)
	{
		FRotator OffsetRot = OwnerTransform.GetRotation().Rotator();
		OffsetRot.Yaw += mOffsetFrontYaw;
		OffsetRot.Normalize();
		OwnerTransform.SetRotation(OffsetRot.Quaternion());
	}
	const float SightRadiusSq = FMath::Square(mRecognizeAbility.mSightRadius);
	const float SightLostRadiusSq = FMath::Square(mRecognizeAbility.mSightLostRadius);
	const float PresenceRadiusSq = FMath::Square(mRecognizeAbility.mPresenceRadius);
	const float PresenceLostRadiusSq = FMath::Square(mRecognizeAbility.mPresenceLostRadius);
	const float CheckDistanceSq = SightLostRadiusSq > PresenceLostRadiusSq ? SightLostRadiusSq : PresenceLostRadiusSq;
	const TArray<TWeakObjectPtr<AActor>>& TargetList = CSKitSubsystem->GetRecognitionTargetList();
	const bool bCylinderSight = mRecognizeAbility.mSightHeight > 0.f;
	for (const auto& WeakPtr : TargetList)
	{
		AActor* Target = WeakPtr.Get();
		if (Target == nullptr
			|| Target == OwnerPawn
			|| IsFriend(Target)
			)
		{
			continue;
		}

		FCSKitRecognitionTarget* RecognitionTarget = FindRecognitionTarget(Target);
		bool bNoRecognitionTarget = true;
		if (RecognitionTarget
			&& !RecognitionTarget->mbCompletelyLost)
		{
			bNoRecognitionTarget = false;
		}
		bool bOldRecognitionTarget = false;
		if(RecognitionTarget
			&& RecognitionTarget->mbRecognize)
		{
			bOldRecognitionTarget = true;
		}

		const FVector TargetPos = Target->GetActorLocation();
		float TargetDistanceSq = FVector::DistSquared(OwnerPos, TargetPos);
		if(bCylinderSight)
		{
			TargetDistanceSq = FVector::DistSquared2D(OwnerPos, TargetPos);
		}
		const bool bOutsideCheck = (TargetDistanceSq > CheckDistanceSq);
		if (bNoRecognitionTarget
			&& bOutsideCheck)
		{//認識対象ではなく、めちゃ離れてるなら無視
			continue;
		}

		const bool bIgnoreTarget = IsIgnoreTarget(Target);
		const bool bHideTarget = IsHideTarget(Target);
		bool bRecognition = false;
		bool bInsidePresence = false;
		float TouchTerritoryGauge = 0.f;
		bool bTouchTerritory = false;
		if(RecognitionTarget)
		{
			TouchTerritoryGauge = RecognitionTarget->mTouchTerritoryGauge;
		}
		
		if(bIgnoreTarget)
		{
			bRecognition = false;
		}
		else if(!bOldRecognitionTarget
			&& bHideTarget)
		{
			if (TargetDistanceSq < PresenceRadiusSq
				&& CheckCollisionVisible(OwnerPos, Target, bOldRecognitionTarget))
			{
				bRecognition = true;
			}
		}
		else
		{
			const float CheckSightRadiusSq = bNoRecognitionTarget ? SightRadiusSq : SightLostRadiusSq;
			if (TargetDistanceSq < CheckSightRadiusSq
				&& (!bCylinderSight || FMath::Abs(OwnerPos.Z - TargetPos.Z) < mRecognizeAbility.mSightHeight)
				)
			{
				const float CheckPresenceRadiusSq = bNoRecognitionTarget ? PresenceRadiusSq : PresenceLostRadiusSq;
				bInsidePresence = (TargetDistanceSq < CheckPresenceRadiusSq);
				bool bWishCheckCollision = false;
				if (bInsidePresence)
				{
					bWishCheckCollision = true;
				}
				else
				{
					const float CheckSightAngle = bNoRecognitionTarget ? mRecognizeAbility.mSightAngle : mRecognizeAbility.mSightLostAngle;
					const FVector TargetV = OwnerTransform.InverseTransformPosition(TargetPos);
					if (FMath::Abs(TargetV.Rotation().Yaw) < CheckSightAngle)
					{
						bWishCheckCollision = true;
					}
				}

				if(bWishCheckCollision)
				{
					if (CheckCollisionVisible(OwnerPos, Target, bOldRecognitionTarget))
					{
						bRecognition = true;
					}
				}
			}

			bool bNeedCheckTerritory = false;
			if(mRecognizeAbility.mbOnlyInsideTerritory)
			{
				bNeedCheckTerritory = true;
			}
			else if(mRecognizeAbility.mbOnlyInsideTerritoryBeginRecognition && RecognitionTarget == nullptr)
			{
				bNeedCheckTerritory = true;
			}
		
			if(const UCSKit_TerritoryComponent* TerritoryComponent = mTerritoryComponent.Get())
			{
				if(TerritoryComponent->IsOwnTerritoryVolume()
					&& TerritoryComponent->IsTouchTerritoryVolume(Target->GetActorLocation(),10.f))
				{
					TouchTerritoryGauge += mRecognizeAbility.mTouchTerritoryGaugeUpSpeed * InDeltaTime;
					bTouchTerritory = true;
				}
				else
				{
					TouchTerritoryGauge -= mRecognizeAbility.mTouchTerritoryGaugeDownSpeed * InDeltaTime;
				}
				TouchTerritoryGauge = FMath::Clamp(TouchTerritoryGauge, 0.f, 1.f);
				
				if(bRecognition
					&& bNeedCheckTerritory)
				{//Territoryがあるのに、自分か対象がその外なら認識しない
					if(TerritoryComponent->IsOwnTerritoryVolume()
						&& (!TerritoryComponent->IsInside() || TouchTerritoryGauge <= 0.f)
						)
					{
						bRecognition = false;
					}
				}
			}
		}

		if (RecognitionTarget)
		{
			if (bRecognition)
			{
				RecognitionTarget->mbRecognize = true;
				RecognitionTarget->mbCompletelyLost = false;
				RecognitionTarget->mbHideTarget = false;
				RecognitionTarget->mLastRecognitionPos = TargetPos;
				RecognitionTarget->mLostSec = 0.f;

				RecognitionTarget->mLastRecognitionFlags = 0;
				RecognitionTarget->mbLastRecognitionInsideSight = !bInsidePresence;
				RecognitionTarget->mbLastRecognitionInsidePresence = bInsidePresence;
			}
			else
			{
				RecognitionTarget->mbRecognize = false;
				RecognitionTarget->mbHideTarget = bHideTarget;
				RecognitionTarget->mLostSec += InDeltaTime;
				if (bIgnoreTarget
					|| RecognitionTarget->mLostSec > mRecognizeAbility.mCompletelyLostBorderTime)
				{
					RecognitionTarget->mbCompletelyLost = true;
					RecognitionTarget->mActor = nullptr;
				}
			}

			if(RecognitionTarget->mRecentDamageDownInterval > 0.f)
			{
				RecognitionTarget->mRecentDamageDownInterval -= InDeltaTime;
			}
			else if(RecognitionTarget->mRecentDamage > 0.f)
			{
				RecognitionTarget->mRecentDamage -= mRecognizeAbility.mRecentDamageDownSpeed*InDeltaTime;
				RecognitionTarget->mRecentDamage = FMath::Max(RecognitionTarget->mRecentDamage, 0.f);
			}

			RecognitionTarget->mTouchTerritoryGauge = TouchTerritoryGauge;
			RecognitionTarget->mbTouchTerritory = bTouchTerritory;
		}
		else if(bRecognition)
		{
			AddRecognitionTarget(Target);
			if (FCSKitRecognitionTarget* NewRecognitionTarget = FindRecognitionTarget(Target))
			{
				NewRecognitionTarget->mLastRecognitionFlags = 0;
				NewRecognitionTarget->mbLastRecognitionInsideSight = !bInsidePresence;
				NewRecognitionTarget->mbLastRecognitionInsidePresence = bInsidePresence;
				
				NewRecognitionTarget->mTouchTerritoryGauge = TouchTerritoryGauge;
				NewRecognitionTarget->mbTouchTerritory = bTouchTerritory;
			}
		}
	}
}

/* ------------------------------------------------------------
   !リストから認識対象情報を取得
------------------------------------------------------------ */
FCSKitRecognitionTarget* UCSKit_RecognitionComponent::FindRecognitionTarget(const AActor* InTarget)
{
	for (FCSKitRecognitionTarget& Target : mRecognitionTargetList)
	{
		if (Target.mActor.Get() == InTarget)
		{
			return &Target;
		}
	}
	return nullptr;
}

/* ------------------------------------------------------------
   !リストに認識対象を追加
------------------------------------------------------------ */
void UCSKit_RecognitionComponent::AddRecognitionTarget(AActor* InTarget)
{
	if (InTarget == nullptr)
	{
		return;
	}
	FCSKitRecognitionTarget NewRecognitionTarget;
	NewRecognitionTarget.mActor = InTarget;
	NewRecognitionTarget.mLastRecognitionPos = InTarget->GetActorLocation();
	NewRecognitionTarget.mbRecognize = true;
	AddRecognitionTarget(NewRecognitionTarget);
}
void UCSKit_RecognitionComponent::AddRecognitionTarget(const FCSKitRecognitionTarget& InTarget)
{
	if(!IsOwnRecognitionTarget())
	{
		OnFirstRecognition(InTarget.mActor.Get());
	}
	if (mRecognitionTargetList.Num() < mMaxRecognitionTargetListSize)
	{
		mRecognitionTargetList.Add(InTarget);
		return;
	}

	//不要になってるのがあれば上書き
	for (FCSKitRecognitionTarget& Target : mRecognitionTargetList)
	{
		if (Target.mbCompletelyLost)
		{
			Target = InTarget;
			return;
		}
	}

	//新しいのより価値が低いのがあれば上書き
	int32 MinPriceIndex = INDEX_NONE;
	float MinPrice = CalcPrice(InTarget);
	for (int32 i=0; i<mRecognitionTargetList.Num(); ++i)
	{
		const FCSKitRecognitionTarget& Target = mRecognitionTargetList[i];
		const float Price = CalcPrice(Target);
		if (Price < MinPrice)
		{
			MinPrice = Price;
			MinPriceIndex = i;
		}
	}
	if (MinPriceIndex != INDEX_NONE)
	{
		mRecognitionTargetList[MinPriceIndex] = InTarget;
	}
}

/* ------------------------------------------------------------
   !誰も認識してない状態から認識対象追加された際の処理
------------------------------------------------------------ */
void UCSKit_RecognitionComponent::OnFirstRecognition(AActor* InTarget)
{
}

/* ------------------------------------------------------------
   !価値計算(上書き用に)
------------------------------------------------------------ */
float UCSKit_RecognitionComponent::CalcPrice(const FCSKitRecognitionTarget& InTarget) const
{
	const AAIController* Owner = Cast<AAIController>(GetOwner());
	if (Owner == nullptr)
	{
		return 0.f;
	}
	const APawn* OwnerPawn = Owner->GetPawn();
	if (OwnerPawn == nullptr)
	{
		return 0.f;
	}
	const FVector BasePos = OwnerPawn->GetActorLocation();

	float DistanceScore = 0.f;
	{
		const float Distance = FVector::Distance(BasePos, InTarget.mLastRecognitionPos);
		const float MaxDistance =
			(mRecognizeAbility.mSightLostRadius > mRecognizeAbility.mPresenceLostRadius) ?
			mRecognizeAbility.mSightLostRadius :
			mRecognizeAbility.mPresenceLostRadius;
		if (MaxDistance > 0.f)
		{
			DistanceScore = 1.f - FMath::Clamp(Distance / MaxDistance, 0.f, 1.f);
		}
	}

// 	float AngleScore = 0.f;
// 	if(mRecognizeAbility.mSightLostAngle > 0.f)
// 	{
// 		const FTransform OwnerTransform(OwnerPawn->GetActorTransform());
// 		const FVector TargetV = OwnerTransform.InverseTransformPosition(InTarget.mLastRecognitionPos);
// 		AngleScore = FMath::Abs(TargetV.Rotation().Yaw) / mRecognizeAbility.mSightLostAngle;
// 		AngleScore = 1.f - FMath::Clamp(AngleScore, 0.f, 1.f);
// 	}

	float LostSecScore = 0.f;
	if (mRecognizeAbility.mCompletelyLostBorderTime > 0.f)
	{
		LostSecScore = 1.f - FMath::Clamp(InTarget.mLostSec / mRecognizeAbility.mCompletelyLostBorderTime, 0.f, 1.f);
	}

	return DistanceScore + /*AngleScore +*/ LostSecScore;
}

/* ------------------------------------------------------------
   !コリジョン的に見えるかどうか
------------------------------------------------------------ */
bool UCSKit_RecognitionComponent::CheckCollisionVisible(const FVector& InBasePos, AActor* InTarget, const bool bInOldRecognitionTarget) const
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
  !ActorWatcher選択時
------------------------------------------------------------ */
bool UCSKit_RecognitionComponent::IsOwnRecognitionTarget() const
{
	for(const FCSKitRecognitionTarget& RecognitionTarget : mRecognitionTargetList)
	{
		if(RecognitionTarget.mbRecognize)
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
FString UCSKit_RecognitionComponent::DebugDrawSelectedActorWatcher(UCanvas* InCanvas) const
{
	FString DebugInfo;

	DebugDrawAbilityRange();
	DebugDrawRecognitionTarget(InCanvas);

	return DebugInfo;
}
/* ------------------------------------------------------------
   !能力範囲デバッグ表示
------------------------------------------------------------ */
void	UCSKit_RecognitionComponent::DebugDrawAbilityRange() const
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
	const FRotator BaseRot = OwnerPawn->GetActorRotation();
	UCSKitDebug_Draw::FanShape VisibleRange;
	VisibleRange.mPos = BasePos;
	VisibleRange.mRot = BaseRot;
	if(mOffsetFrontYaw != 0.f)
	{
		VisibleRange.mRot.Yaw += mOffsetFrontYaw;
		VisibleRange.mRot.Normalize();
	}
	VisibleRange.mAngle = mRecognizeAbility.mSightAngle*2.f;
	VisibleRange.mRadius = mRecognizeAbility.mSightRadius;
	VisibleRange.Draw(GetWorld(), FColor::Green, 0, 3.f);

	VisibleRange.mAngle = mRecognizeAbility.mSightLostAngle*2.f;
	VisibleRange.mRadius = mRecognizeAbility.mSightLostRadius;
	VisibleRange.Draw(GetWorld(), FColor::Yellow, 0, 3.f);

	const FMatrix CircleMatrix(FQuatRotationTranslationMatrix(FQuat(FVector(0.f, 1.f, 0.f), PI*0.5f), BasePos));
	DrawDebugCircle(
		GetWorld(),
		CircleMatrix,
		mRecognizeAbility.mPresenceRadius,
		32,
		FColor::Green,
		false,
		-1.f,
		0,
		3.f
		);
	DrawDebugCircle(
		GetWorld(),
		CircleMatrix,
		mRecognizeAbility.mPresenceLostRadius,
		32,
		FColor::Yellow,
		false,
		-1.f,
		0,
		3.f
	);
}

/* ------------------------------------------------------------
   !認識対象デバッグ表示
------------------------------------------------------------ */
void UCSKit_RecognitionComponent::DebugDrawRecognitionTarget(UCanvas* InCanvas) const
{
	for (const FCSKitRecognitionTarget& Target : mRecognitionTargetList)
	{
		DebugDrawRecognitionTarget(InCanvas, Target);
	}
}
void UCSKit_RecognitionComponent::DebugDrawRecognitionTarget(UCanvas* InCanvas, const FCSKitRecognitionTarget& InTarget) const
{
	const AActor* TargetActor = InTarget.mActor.Get();
	const AAIController* Owner = Cast<AAIController>(GetOwner());
	if (TargetActor == nullptr
		|| Owner == nullptr)
	{
		return;
	}
	const APawn* OwnerPawn = Owner->GetPawn();
	if (OwnerPawn == nullptr)
	{
		return;
	}

	const FVector BasePos = OwnerPawn->GetActorLocation();

	FColor Color = FColor::Blue;
	if (InTarget.mbCompletelyLost)
	{
		Color = FColor::Red;
	}
	else if (!InTarget.mbRecognize)
	{
		Color = FColor::Yellow;
	}

	DrawDebugLine(GetWorld(), BasePos, InTarget.mLastRecognitionPos, Color);
	if (!InTarget.mbRecognize)
	{
		DrawDebugLine(GetWorld(), InTarget.mLastRecognitionPos, TargetActor->GetActorLocation(), Color);
	}

	FCSKitDebug_ScreenWindowText DebugWindow;
	DebugWindow.SetWindowFrameColor(Color);
	DebugWindow.SetWindowName(TEXT("RecognitionTarget"));
	DebugWindow.AddText(FString::Printf(TEXT("%.s"), *TargetActor->GetName()));
	DebugWindow.AddText(FString::Printf(TEXT("mLostSec : %.1f"), InTarget.mLostSec));
	DebugWindow.AddText(FString::Printf(TEXT("mTotalDamage : %.1f"), InTarget.mTotalDamage));
	DebugWindow.AddText(FString::Printf(TEXT("mRecentDamage : %.1f(%.1f)"), InTarget.mRecentDamage, InTarget.mRecentDamageDownInterval));
	DebugWindow.AddText(FString::Printf(TEXT("mTouchTerritorySec : %.1f"), InTarget.mTouchTerritoryGauge));
	DebugWindow.AddText(FString::Printf(TEXT("mPrice : %.2f"), CalcPrice(InTarget)));
	DebugWindow.AddText(FString::Printf(TEXT("mbRecognize : %d"), InTarget.mbRecognize));
	DebugWindow.AddText(FString::Printf(TEXT("mbCompletelyLost : %d"), InTarget.mbCompletelyLost));
	DebugWindow.AddText(FString::Printf(TEXT("mbLastRecognitionInsideSight : %d"), InTarget.mbLastRecognitionInsideSight));
	DebugWindow.AddText(FString::Printf(TEXT("mbLastRecognitionInsidePresence : %d"), InTarget.mbLastRecognitionInsidePresence));
	DebugWindow.AddText(FString::Printf(TEXT("mbLastRecognitionTakeDamage : %d"), InTarget.mbLastRecognitionTakeDamage));
	DebugWindow.AddText(FString::Printf(TEXT("mbLastRecognitionTakeMessage : %d"), InTarget.mbLastRecognitionTakeMessage));
	DebugWindow.Draw(InCanvas, InTarget.mLastRecognitionPos);
}

#endif