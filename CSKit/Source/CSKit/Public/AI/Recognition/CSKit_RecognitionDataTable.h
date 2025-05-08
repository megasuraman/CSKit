// Copyright 2020 megasuraman.
/**
 * @file CSKit_RecognitionDataTable.h
 * @brief Recognition用DataTable
 * @author megasuraman
 * @date 2025/05/05
 */
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "CSKit_RecognitionDataTable.generated.h"


/* ------------------------------------------------------------
   !能力値
------------------------------------------------------------ */
USTRUCT(BlueprintType)
struct CSKIT_API FCSKit_RecognizeAbility
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, Category = "RecognizeAbility", Meta = (DisplayName = "視野距離"))
	float	mSightRadius = 1000.f;
	UPROPERTY(EditDefaultsOnly, Category = "RecognizeAbility", Meta = (DisplayName = "視野距離(見失い)"))
	float	mSightLostRadius = 1500.f;
	UPROPERTY(EditDefaultsOnly, Category = "RecognizeAbility", Meta = (DisplayName = "視野高さ"))
	float	mSightHeight = -1.f;
	UPROPERTY(EditDefaultsOnly, Category = "RecognizeAbility", Meta = (DisplayName = "正面からの視野角"))
	float	mSightAngle = 90.f;
	UPROPERTY(EditDefaultsOnly, Category = "RecognizeAbility", Meta = (DisplayName = "正面からの視野角(見失い)"))
	float	mSightLostAngle = 120.f;
	UPROPERTY(EditDefaultsOnly, Category = "RecognizeAbility", Meta = (DisplayName = "気配距離"))
	float	mPresenceRadius = 200.f;
	UPROPERTY(EditDefaultsOnly, Category = "RecognizeAbility", Meta = (DisplayName = "気配距離(見失い)"))
	float	mPresenceLostRadius = 500.f;
	UPROPERTY(EditDefaultsOnly, Category = "RecognizeAbility", Meta = (DisplayName = "直近ダメージ量キープ時間"))
	float	mRecentDamageDownIntervalTime = 3.f;
	UPROPERTY(EditDefaultsOnly, Category = "RecognizeAbility", Meta = (DisplayName = "直近ダメージ量減少速度"))
	float	mRecentDamageDownSpeed = 100.f;
	UPROPERTY(EditDefaultsOnly, Category = "RecognizeAbility", Meta = (DisplayName = "完全に認識無くなるまでの時間"))
	float	mCompletelyLostBorderTime = 10.f;
	UPROPERTY(EditDefaultsOnly, Category = "RecognizeAbility", Meta = (DisplayName = "TerritoryGauge上昇速度"))
	float	mTouchTerritoryGaugeUpSpeed = 1.f;
	UPROPERTY(EditDefaultsOnly, Category = "RecognizeAbility", Meta = (DisplayName = "TerritoryGauge下降速度"))
	float	mTouchTerritoryGaugeDownSpeed = 5.f;
	UPROPERTY(EditDefaultsOnly, Category = "RecognizeAbility", Meta = (DisplayName = "Territory内のみ認識"))
	bool	mbOnlyInsideTerritory = false;
	UPROPERTY(EditDefaultsOnly, Category = "RecognizeAbility", Meta = (DisplayName = "認識開始のみTerritory内のみに"))
	bool	mbOnlyInsideTerritoryBeginRecognition = false;
};


// DataTable
USTRUCT(BlueprintType)
struct CSKIT_API FCSKit_RecognitionTableRow : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, Category = "Recognition", meta = (DisplayName = "能力値"))
	FCSKit_RecognizeAbility		mRecognizeAbility;
};