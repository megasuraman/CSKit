// Copyright 2020 megasuraman.
/**
 * @file CSKit_WorryDataTable.h
 * @brief Worry用DataTable
 * @author megasuraman
 * @date 2025/05/06
 */
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "CSKit_WorryDataTable.generated.h"


// 気になる対象のDataTable(音の発生源等)
USTRUCT(BlueprintType)
struct CSKIT_API FCSKit_WorrySourceTableRow : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, Category = "WorrySource", meta = (DisplayName = "効果範囲(min)"))
	float mEffectRadiusMin = 100.f;
	UPROPERTY(EditDefaultsOnly, Category = "WorrySource", meta = (DisplayName = "効果範囲(max)"))
	float mEffectRadiusMax = 300.f;
	UPROPERTY(EditDefaultsOnly, Category = "WorrySource", meta = (DisplayName = "効果範囲拡大時間"))
	float mEffectScaleTime = 0.1f;
	UPROPERTY(EditDefaultsOnly, Category = "WorrySource", meta = (DisplayName = "効果時間"))
	float mEffectTime = 1.f;
};

// 気にする側の能力DataTable
USTRUCT(BlueprintType)
struct CSKIT_API FCSKit_WorryAbilityTableRow : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, Category = "WorryAbility", meta = (DisplayName = "気にする範囲"))
	float mCatchRadius = 500.f;
	UPROPERTY(EditDefaultsOnly, Category = "WorryAbility", meta = (DisplayName = "気にする範囲(高低差)"))
	float mCatchHeight = 300.f;
	UPROPERTY(EditDefaultsOnly, Category = "WorryAbility", meta = (DisplayName = "気にする視界距離"))
	float mVisibilityDistance = 0.f;
	UPROPERTY(EditDefaultsOnly, Category = "WorryAbility", meta = (DisplayName = "気にする視界角度"))
	float mVisibilityAngle = 0.f;
};