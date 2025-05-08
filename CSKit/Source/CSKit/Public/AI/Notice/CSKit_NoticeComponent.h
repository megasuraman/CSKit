// Copyright 2020 megasuraman.
/**
 * @file CSKit_NoticeComponent.h
 * @brief 注目対象管理Component
 * @author megasuraman
 * @date 2025/05/05
 */
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AI/Notice/CSKit_NoticeDataTable.h"
#include "CSKit_NoticeComponent.generated.h"

class UCSKit_RecognitionComponent;
class UCSKit_TerritoryComponent;
struct FCSKit_NoticeTableRow;

//対象選択用のデータ
struct FCSKit_NoticeData
{
	TWeakObjectPtr<AActor>	mActor;
	float mScore = 0.f;
#if USE_CSKIT_DEBUG
	float mDebugScoreDistance = 0.f;
	float mDebugScoreAngle = 0.f;
	float mDebugScoreVisible = 0.f;
	float mDebugScoreDamage = 0.f;
	float mDebugScoreRepeatSelect = 0.f;
	float mDebugScoreSameTargetNum = 0.f;
	float mDebugScoreTargetPriority = 0.f;
#endif
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CSKIT_API UCSKit_NoticeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCSKit_NoticeComponent();
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void Setup(const FCSKit_NoticeTableRow& InData);
	void SetTerritoryComponent(const UCSKit_TerritoryComponent* InComponent);
	virtual void Update(const float InDeltaTime);
	AActor* GetSelectTarget() const;
	virtual float GetSameTargetPriorityScore() const;
	void SetNoSelectTarget(const bool bInNoSelect){mbNoSelectTarget=bInNoSelect;}

protected:
	void SetScoreWeight(const FCSKit_NoticeScoreWeight& InData)
	{
		mScoreWeight = InData;
	}
	void SetScoreBaseValue(const FCSKit_NoticeScoreBaseValue& InData)
	{
		mScoreBaseValue = InData;
	}
	void UpdateNoticeDataList();
	void UpdateSelectTarget(const float InDeltaTime);
	void SortNoticeDataList();
	void SetSelectTarget(AActor* InTarget);
	void OnChangeSelectTarget(AActor* InOldTarget, AActor* InNewTarget) const;
	virtual bool IsIgnoreTarget(const AActor* InTarget) const{return false;}
	virtual float GetTargetPriority(const AActor* InTarget) const{return 0.f;}

private:
	FCSKit_NoticeScoreWeight mScoreWeight;
	FCSKit_NoticeScoreBaseValue mScoreBaseValue;
	TWeakObjectPtr<UCSKit_RecognitionComponent> mRecognitionComponent;
	TWeakObjectPtr<const UCSKit_TerritoryComponent> mTerritoryComponent;
	TWeakObjectPtr<AActor> mSelectTarget;
	TArray<FCSKit_NoticeData> mNoticeDataList;
	float mRepeatSelectSec = 0.f;
	float mInsideTerritoryBorderTime = 1.f;
	bool mbOnlyInsideTerritory = false;
	bool mbNoSelectTarget = false;

#if USE_CSKIT_DEBUG
public:
	virtual FString DebugDrawSelectedActorWatcher(UCanvas* InCanvas) const;

	void DebugSetSleep(const bool bInSleep) { mbDebugSleep = bInSleep; }
protected:
	void DebugDrawNoticeDataList(UCanvas* InCanvas) const;
	void DebugDrawNoticeDataList(UCanvas* InCanvas, const FCSKit_NoticeData& InData) const;
private:
	bool mbDebugSleep = false;
#endif
};
