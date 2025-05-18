// Copyright 2020 megasuraman.
/**
 * @file CSKit_NoticeComponent.cpp
 * @brief 注目対象管理Component
 * @author megasuraman
 * @date 2025/05/05
 */
#include "AI/Notice/CSKit_NoticeComponent.h"

#include "AI/CSKit_AIController.h"
#include "AI/Notice/CSKit_NoticeTargetManager.h"
#include "CSKit_Subsystem.h"
#include "AI/Recognition/CSKit_RecognitionComponent.h"
#include "DrawDebugHelpers.h"
#include "AI/Territory/CSKit_TerritoryComponent.h"

#if USE_CSKIT_DEBUG
#include "ScreenWindow/CSKitDebug_ScreenWindowText.h"
#endif

UCSKit_NoticeComponent::UCSKit_NoticeComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


void UCSKit_NoticeComponent::BeginPlay()
{
	Super::BeginPlay();

	if (const ACSKit_AIController* Owner = Cast<ACSKit_AIController>(GetOwner()))
	{
		mRecognitionComponent = Owner->GetCSKitRecognition();
	}
}

/**
 * @brief 
 */
void UCSKit_NoticeComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if(GetSelectTarget() != nullptr)
	{
		SetSelectTarget(nullptr);
	}
}


/**
 * @brief 
 */
void UCSKit_NoticeComponent::Setup(const FCSKit_NoticeTableRow& InData)
{
	SetScoreWeight(InData.mScoreWeight);
	SetScoreBaseValue(InData.mScoreBaseValue);
	mInsideTerritoryBorderTime = InData.mInsideTerritoryBorderTime;
	mbOnlyInsideTerritory = InData.mbOnlyInsideTerritory;
}

/**
 * @brief 
 */
void UCSKit_NoticeComponent::SetTerritoryComponent(const UCSKit_TerritoryComponent* InComponent)
{
	mTerritoryComponent = InComponent;
}

/* ------------------------------------------------------------
   !更新処理(Tickは使わず、CSKit_AIControllerから呼ぶ)
------------------------------------------------------------ */
void UCSKit_NoticeComponent::Update(const float InDeltaSec)
{
#if USE_CSKIT_DEBUG
	if (mbDebugSleep)
	{
		return;
	}
#endif
	UpdateNoticeDataList();
	UpdateSelectTarget(InDeltaSec);
}

/**
 * @brief 
 */
AActor* UCSKit_NoticeComponent::GetSelectTarget() const
{
	return mSelectTarget.Get();
}

/**
 * @brief 
 */
float UCSKit_NoticeComponent::GetSameTargetPriorityScore() const
{
	if (mNoticeDataList.Num() > 0)
	{
		return mNoticeDataList[0].mScore;
	}
	return 0.f;
}

/**
 * @brief 
 */
void UCSKit_NoticeComponent::UpdateNoticeDataList()
{
	mNoticeDataList.Empty();
	if(mbNoSelectTarget)
	{
		return;
	}
	const ACSKit_AIController* Owner = Cast<ACSKit_AIController>(GetOwner());
	const UCSKit_RecognitionComponent* RecognitionComponent = mRecognitionComponent.Get();
	const UCSKit_Subsystem* CSKitSubsystem = GetWorld()->GetSubsystem<UCSKit_Subsystem>();
	if (Owner == nullptr
		|| RecognitionComponent == nullptr
		|| CSKitSubsystem == nullptr
		)
	{
		return;
	}
	const APawn* OwnerPawn = Owner->GetPawn();
	const UCSKit_NoticeTargetManager* NoticeTargetManager = CSKitSubsystem->GetNoticeTargetManager();
	if (OwnerPawn == nullptr
		|| NoticeTargetManager == nullptr)
	{
		return;
	}

	const AActor* SelectedTarget = GetSelectTarget();
	const FVector OwnerPawnPos = OwnerPawn->GetActorLocation();
	const FTransform OwnerTransform(OwnerPawn->GetActorTransform());
	const float BaseValueDistanceRcp = (mScoreBaseValue.mDistanceRangeMax - mScoreBaseValue.mDistanceRangeMin > 0.f)
		? 1.f / (mScoreBaseValue.mDistanceRangeMax - mScoreBaseValue.mDistanceRangeMin)
		: 0.f;
	const float BaseValueAngleRcp = (mScoreBaseValue.mAngleRangeMax - mScoreBaseValue.mAngleRangeMin > 0.f)
		? 1.f / (mScoreBaseValue.mAngleRangeMax - mScoreBaseValue.mAngleRangeMin)
		: 0.f;
	const float BaseValueLostSecRcp = (mScoreBaseValue.mLostSecRangeMax - mScoreBaseValue.mLostSecRangeMin > 0.f)
		? 1.f / (mScoreBaseValue.mLostSecRangeMax - mScoreBaseValue.mLostSecRangeMin)
		: 0.f;
	const float BaseValueDamageRcp = (mScoreBaseValue.mDamageRangeMax - mScoreBaseValue.mDamageRangeMin > 0.f)
		? 1.f / (mScoreBaseValue.mDamageRangeMax - mScoreBaseValue.mDamageRangeMin)
		: 0.f;
	const float BaseValueRepeatSelectRcp = (mScoreBaseValue.mRepeatSelectSecRangeMax - mScoreBaseValue.mRepeatSelectSecRangeMin > 0.f)
		? 1.f / (mScoreBaseValue.mRepeatSelectSecRangeMax - mScoreBaseValue.mRepeatSelectSecRangeMin)
		: 0.f;
	const float BaseValueSameTargetNumRcp = (mScoreBaseValue.mSameTargetNumMax - mScoreBaseValue.mSameTargetNumMin > 0.f)
		? 1.f / (mScoreBaseValue.mSameTargetNumMax - mScoreBaseValue.mSameTargetNumMin)
		: 0.f;
	const TArray<FCSKitRecognitionTarget>& RecognitionTargetList = RecognitionComponent->GetRecognitionTargetList();
	for (const FCSKitRecognitionTarget& RecognitionTarget : RecognitionTargetList)
	{
		if (RecognitionTarget.mbCompletelyLost)
		{
			continue;
		}
		AActor* TargetActor = RecognitionTarget.mActor.Get();
		if (TargetActor == nullptr
			|| IsIgnoreTarget(TargetActor))
		{
			continue;
		}

		if(mbOnlyInsideTerritory)
		{
			if(const UCSKit_TerritoryComponent* TerritoryComponent = mTerritoryComponent.Get())
			{//Territoryがあるのに、自分か対象がその外なら認識しない
				if(TerritoryComponent->IsOwnTerritoryVolume()
					&& (!TerritoryComponent->IsInside() || RecognitionTarget.mTouchTerritoryGauge < mInsideTerritoryBorderTime)
					)
				{
					continue;
				}
			}
		}

		const FVector TargetPos = RecognitionTarget.mLastRecognitionPos;

		float ScoreDistance = 0.f;
		if (mScoreWeight.mScoreDistanceRatio > 0.f
			&& BaseValueDistanceRcp > 0.f)
		{
			const float Distance = FVector::Distance(OwnerPawnPos, TargetPos);
			const float DistanceRatio = FMath::Clamp(
				(Distance - mScoreBaseValue.mDistanceRangeMin) * BaseValueDistanceRcp, 0.f, 1.f);
			ScoreDistance = 1.f - DistanceRatio;
			ScoreDistance *= mScoreWeight.mScoreDistanceRatio;
		}

		float ScoreAngle = 0.f;
		if (mScoreWeight.mScoreAngleRatio > 0.f
			&& BaseValueAngleRcp > 0.f)
		{
			const FVector TargetV = OwnerTransform.InverseTransformPosition(TargetPos);
			const float Angle = FMath::Abs(TargetV.Rotation().Yaw);
			const float AngleRatio = FMath::Clamp((Angle - mScoreBaseValue.mAngleRangeMin) * BaseValueAngleRcp, 0.f,
			                                      1.f);
			ScoreAngle = 1.f - AngleRatio;
			ScoreAngle *= mScoreWeight.mScoreAngleRatio;
		}

		float ScoreVisible = 0.f;
		if (mScoreWeight.mScoreVisibleRatio > 0.f
			&& BaseValueLostSecRcp > 0.f
			&& ScoreDistance > 0.f//めちゃ遠くなら見えてないことに
			)
		{
			const float LostRatio = FMath::Clamp(
				(RecognitionTarget.mLostSec - mScoreBaseValue.mLostSecRangeMin) * BaseValueLostSecRcp, 0.f, 1.f);
			ScoreVisible = 1.f - LostRatio;
			ScoreVisible *= mScoreWeight.mScoreVisibleRatio;
		}

		float ScoreDamage = 0.f;
		if (mScoreWeight.mScoreDamageRatio > 0.f
			&& BaseValueDamageRcp > 0.f)
		{
			const float DamageRatio = FMath::Clamp(
				(RecognitionTarget.mRecentDamage - mScoreBaseValue.mDamageRangeMin) * BaseValueDamageRcp, 0.f, 1.f);
			ScoreDamage = DamageRatio;
			ScoreDamage *= mScoreWeight.mScoreDamageRatio;
		}

		float ScoreRepeatSelect = 0.f;
		if (mScoreWeight.mScoreRepeatSelectRatio > 0.f
			&& BaseValueRepeatSelectRcp > 0.f
			&& SelectedTarget == TargetActor)
		{
			const float RepeatSelectRatio = FMath::Clamp(
				(mRepeatSelectSec - mScoreBaseValue.mRepeatSelectSecRangeMin) * BaseValueRepeatSelectRcp, 0.f, 1.f);
			ScoreRepeatSelect = 1.f - RepeatSelectRatio;
			ScoreRepeatSelect *= mScoreWeight.mScoreRepeatSelectRatio;
		}
		float ScoreSameTargetNum = 0.f;
		if(mScoreWeight.mScoreSameTargetNumRatio > 0.f
			&& BaseValueSameTargetNumRcp > 0.f)
		{
			const float SameTargetNum = static_cast<float>(NoticeTargetManager->CalcMemberNum(TargetActor, Owner));
			ScoreSameTargetNum = (SameTargetNum - mScoreBaseValue.mSameTargetNumMin) * BaseValueSameTargetNumRcp;
			ScoreSameTargetNum = FMath::Clamp(ScoreSameTargetNum, 0.f, 1.f);
			ScoreSameTargetNum = 1.f - ScoreSameTargetNum;
			ScoreSameTargetNum *= mScoreWeight.mScoreSameTargetNumRatio;
		}

		const float ScoreTargetPriority = GetTargetPriority(TargetActor) * mScoreWeight.mScoreTargetPriorityRatio;

		const float ScoreTotal = ScoreDistance + ScoreAngle + ScoreVisible + ScoreDamage + ScoreRepeatSelect + ScoreSameTargetNum + ScoreTargetPriority;
		if(ScoreTotal <= 0.f)
		{
			continue;
		}

		FCSKit_NoticeData NoticeData;
		NoticeData.mActor = TargetActor;
		NoticeData.mScore = ScoreTotal;
#if USE_CSKIT_DEBUG
		NoticeData.mDebugScoreDistance = ScoreDistance;
		NoticeData.mDebugScoreAngle = ScoreAngle;
		NoticeData.mDebugScoreVisible = ScoreVisible;
		NoticeData.mDebugScoreDamage = ScoreDamage;
		NoticeData.mDebugScoreRepeatSelect = ScoreRepeatSelect;
		NoticeData.mDebugScoreSameTargetNum = ScoreSameTargetNum;
		NoticeData.mDebugScoreTargetPriority = ScoreTargetPriority;
#endif
		mNoticeDataList.Add(NoticeData);
	}
}

/**
 * @brief 
 */
void UCSKit_NoticeComponent::UpdateSelectTarget(const float InDeltaTime)
{
	if (mNoticeDataList.Num() == 0)
	{
		SetSelectTarget(nullptr);
		return;
	}

	SortNoticeDataList();

	AActor* SelectActor = mNoticeDataList[0].mActor.Get();
	if (SelectActor == mSelectTarget.Get())
	{
		mRepeatSelectSec += InDeltaTime;
	}
	else
	{
		SetSelectTarget(SelectActor);
	}
}

/**
 * @brief 
 */
void UCSKit_NoticeComponent::SortNoticeDataList()
{
	mNoticeDataList.Sort([](const FCSKit_NoticeData& Base, const FCSKit_NoticeData& Target)
	{
		return (Base.mScore > Target.mScore);
	}
	);
}

/**
 * @brief 
 */
void UCSKit_NoticeComponent::SetSelectTarget(AActor* InTarget)
{
	AActor* OldTarget = mSelectTarget.Get();
	if(OldTarget == InTarget)
	{
		return;
	}
	mSelectTarget = InTarget;
	mRepeatSelectSec = 0.f;
	OnChangeSelectTarget(OldTarget, InTarget);
}

/**
 * @brief 
 */
void UCSKit_NoticeComponent::OnChangeSelectTarget(AActor* InOldTarget, AActor* InNewTarget) const
{
	ACSKit_AIController* AIController = Cast<ACSKit_AIController>(GetOwner());
	if (AIController == nullptr)
	{
		return;
	}
	
	AIController->OnChangeNoticeTarget(InOldTarget, InNewTarget);
	
	if (const UCSKit_Subsystem* CSKitSubsystem = GetWorld()->GetSubsystem<UCSKit_Subsystem>())
	{
		if (UCSKit_NoticeTargetManager* NoticeTargetManager = CSKitSubsystem->GetNoticeTargetManager())
		{
			NoticeTargetManager->Exit(InOldTarget, AIController);
			NoticeTargetManager->Entry(InNewTarget, AIController);
		}
	}
}

#if USE_CSKIT_DEBUG
/* ------------------------------------------------------------
  !ActorWatcher選択時
------------------------------------------------------------ */
FString UCSKit_NoticeComponent::DebugDrawSelectedActorWatcher(UCanvas* InCanvas) const
{
	FString DebugInfo;

	DebugInfo += FString::Printf(TEXT("[Notice]\n"));
	DebugInfo += FString::Printf(TEXT("   mRepeatSelectSec : %.2f\n"), mRepeatSelectSec);

	DebugDrawNoticeDataList(InCanvas);

	return DebugInfo;
}

/* ------------------------------------------------------------
   !対象選択情報デバッグ表示
------------------------------------------------------------ */
void UCSKit_NoticeComponent::DebugDrawNoticeDataList(UCanvas* InCanvas) const
{
	for (const FCSKit_NoticeData& Data : mNoticeDataList)
	{
		DebugDrawNoticeDataList(InCanvas, Data);
	}
}
void UCSKit_NoticeComponent::DebugDrawNoticeDataList(UCanvas* InCanvas, const FCSKit_NoticeData& InData) const
{
	const AActor* TargetActor = InData.mActor.Get();
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

	FColor Color = FColor::Green;
	if (TargetActor == GetSelectTarget())
	{
		Color = FColor::Red;
	}

	DrawDebugLine(GetWorld(), OwnerPawn->GetActorLocation(), TargetActor->GetActorLocation(), Color);

	FCSKitDebug_ScreenWindowText DebugWindow;
	DebugWindow.SetWindowFrameColor(Color);
	DebugWindow.SetWindowName(TEXT("NoticeData"));
	DebugWindow.AddText(FString::Printf(TEXT("mScore : %.2f"), InData.mScore));
	DebugWindow.AddText(FString::Printf(TEXT("  Distance : %.2f"), InData.mDebugScoreDistance));
	DebugWindow.AddText(FString::Printf(TEXT("  Angle : %.2f"), InData.mDebugScoreAngle));
	DebugWindow.AddText(FString::Printf(TEXT("  Visible : %.2f"), InData.mDebugScoreVisible));
	DebugWindow.AddText(FString::Printf(TEXT("  Damage : %.2f"), InData.mDebugScoreDamage));
	DebugWindow.AddText(FString::Printf(TEXT("  Repeat : %.2f"), InData.mDebugScoreRepeatSelect));
	DebugWindow.AddText(FString::Printf(TEXT("  SameTarget : %.2f"), InData.mDebugScoreSameTargetNum));
	DebugWindow.AddText(FString::Printf(TEXT("  TargetPriority : %.2f"), InData.mDebugScoreTargetPriority));
	DebugWindow.Draw(InCanvas, TargetActor->GetActorLocation());
}

#endif