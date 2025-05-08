// Copyright 2020 megasuraman.
/**
 * @file CSKit_ExperienceDataTable.h
 * @brief CSKit ExperienceのDataTable関連
 * @author megasuraman
 * @date 2025/05/05
 */
#pragma once
#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "CSKit_ExperienceDataTable.generated.h"

/* ------------------------------------------------------------
   !Score計算に使う値
------------------------------------------------------------ */
USTRUCT(BlueprintType)
struct CSKIT_API FCSKit_ExperienceScoreElement
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, Category = ExperienceScoreElement, Meta = (DisplayName = "良い経験値", ClampMin = "0.0", ClampMax = "1.0"))
	float mPositiveValue = 0.f;
	UPROPERTY(EditDefaultsOnly, Category = ExperienceScoreElement, Meta = (DisplayName = "悪い経験値", ClampMin = "0.0", ClampMax = "1.0"))
	float mNegativeValue = 0.f;
	UPROPERTY(EditDefaultsOnly, Category = ExperienceScoreElement, Meta = (DisplayName = "自動加算までの間隔(秒)"))
	float mAutoAddIntervalTime = 0.f;
	UPROPERTY(EditDefaultsOnly, Category = ExperienceScoreElement, Meta = (DisplayName = "自動加算速度"))
	float mAutoAddSpeed = 0.f;
	UPROPERTY(EditDefaultsOnly, Category = ExperienceScoreElement, Meta = (DisplayName = "補正割合の最小値"))
	float mReviseRatioMin = 0.5f;
	UPROPERTY(EditDefaultsOnly, Category = ExperienceScoreElement, Meta = (DisplayName = "補正割合の最大値"))
	float mReviseRatioMax = 1.5f;
	UPROPERTY(EditDefaultsOnly, Category = ExperienceScoreElement, Meta = (DisplayName = "補正割合を決める基準値"))
	float mReviseBaseValue = 0.5f;
};

/* ------------------------------------------------------------
   !Experience設定のDataTable
------------------------------------------------------------ */
USTRUCT(BlueprintType)
struct CSKIT_API FCSKit_ExperienceTableRow : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	FName mName;
	int32 mMasterDataIndex = 0;

	UPROPERTY(EditDefaultsOnly, Category = "Experience", meta = (DisplayName = "Score設定値"))
	TMap<FName, FCSKit_ExperienceScoreElement> mExperienceScoreElement;
};