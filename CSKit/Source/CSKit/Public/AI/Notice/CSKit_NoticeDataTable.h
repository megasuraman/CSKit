// Copyright 2020 megasuraman.
/**
 * @file CSKit_NoticeDataTable.h
 * @brief Notice用DataTable
 * @author megasuraman
 * @date 2025/05/05
 */
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "CSKit_NoticeDataTable.generated.h"


//スコア計算時の各種重み値
USTRUCT(BlueprintType)
struct CSKIT_API FCSKit_NoticeScoreWeight
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, Category = "NoticeScoreWeight", Meta = (DisplayName = "距離"))
	float	mScoreDistanceRatio = 1.f;
	UPROPERTY(EditDefaultsOnly, Category = "NoticeScoreWeight", Meta = (DisplayName = "角度"))
	float	mScoreAngleRatio = 1.f;
	UPROPERTY(EditDefaultsOnly, Category = "NoticeScoreWeight", Meta = (DisplayName = "可視"))
	float	mScoreVisibleRatio = 1.f;
	UPROPERTY(EditDefaultsOnly, Category = "NoticeScoreWeight", Meta = (DisplayName = "ダメージ"))
	float	mScoreDamageRatio = 1.f;
	UPROPERTY(EditDefaultsOnly, Category = "NoticeScoreWeight", Meta = (DisplayName = "継続"))
	float	mScoreRepeatSelectRatio = 1.f;
	UPROPERTY(EditDefaultsOnly, Category = "NoticeScoreWeight", Meta = (DisplayName = "被り"))
	float	mScoreSameTargetNumRatio = 2.f;
	UPROPERTY(EditDefaultsOnly, Category = "NoticeScoreWeight", Meta = (DisplayName = "対象優先度"))
	float	mScoreTargetPriorityRatio = 1.f;
};

//スコア計算時の各種基準値
USTRUCT(BlueprintType)
struct CSKIT_API FCSKit_NoticeScoreBaseValue
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, Category = "NoticeScoreBaseValue", Meta = (DisplayName = "距離最小値"))
	float	mDistanceRangeMin = 300.f;
	UPROPERTY(EditDefaultsOnly, Category = "NoticeScoreBaseValue", Meta = (DisplayName = "距離最大値"))
	float	mDistanceRangeMax = 1500.f;
	UPROPERTY(EditDefaultsOnly, Category = "NoticeScoreBaseValue", Meta = (DisplayName = "角度最小値"))
	float	mAngleRangeMin = 50.f;
	UPROPERTY(EditDefaultsOnly, Category = "NoticeScoreBaseValue", Meta = (DisplayName = "角度最大値"))
	float	mAngleRangeMax = 90.f;
	UPROPERTY(EditDefaultsOnly, Category = "NoticeScoreBaseValue", Meta = (DisplayName = "見失い時間最小値"))
	float	mLostSecRangeMin = 3.f;
	UPROPERTY(EditDefaultsOnly, Category = "NoticeScoreBaseValue", Meta = (DisplayName = "見失い時間最大値"))
	float	mLostSecRangeMax = 8.f;
	UPROPERTY(EditDefaultsOnly, Category = "NoticeScoreBaseValue", Meta = (DisplayName = "ダメージ最小値"))
	float	mDamageRangeMin = 200.f;
	UPROPERTY(EditDefaultsOnly, Category = "NoticeScoreBaseValue", Meta = (DisplayName = "ダメージ最大値"))
	float	mDamageRangeMax = 800.f;
	UPROPERTY(EditDefaultsOnly, Category = "NoticeScoreBaseValue", Meta = (DisplayName = "選択時間最小値"))
	float	mRepeatSelectSecRangeMin = 2.f;
	UPROPERTY(EditDefaultsOnly, Category = "NoticeScoreBaseValue", Meta = (DisplayName = "選択時間最大値"))
	float	mRepeatSelectSecRangeMax = 5.f;
	UPROPERTY(EditDefaultsOnly, Category = "NoticeScoreBaseValue", Meta = (DisplayName = "同時に狙ってる人の最小数"))
	float	mSameTargetNumMin = 0.f;
	UPROPERTY(EditDefaultsOnly, Category = "NoticeScoreBaseValue", Meta = (DisplayName = "同時に狙ってる人の最大数"))
	float	mSameTargetNumMax = 3.f;
};


// DataTable
USTRUCT(BlueprintType)
struct FCSKit_NoticeTableRow : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, Category = "Notice", meta = (DisplayName = "基準値"))
	FCSKit_NoticeScoreBaseValue mScoreBaseValue;
	UPROPERTY(EditDefaultsOnly, Category = "Notice", meta = (DisplayName = "重み"))
	FCSKit_NoticeScoreWeight mScoreWeight;
	UPROPERTY(EditDefaultsOnly, Category = "Notice", Meta = (DisplayName = "Territory内と判断する時間"))
	float mInsideTerritoryBorderTime = 1.f;
	UPROPERTY(EditDefaultsOnly, Category = "Notice", Meta = (DisplayName = "Territory内のみ認識"))
	bool mbOnlyInsideTerritory = false;
};