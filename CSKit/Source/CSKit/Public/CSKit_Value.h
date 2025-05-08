// Copyright 2020 megasuraman.
/**
 * @file CSKit_Compress.h
 * @brief 値管理用の各種構造体
 * @author megasuraman
 * @date 2025/05/05
 */
#pragma once

#include "CoreMinimal.h"
#include "CSKit_Subsystem.h"
#include "CSKit_Value.generated.h"

/* ------------------------------------------------------------
   !Floatの最小値と最大値を定義
------------------------------------------------------------ */
USTRUCT(BlueprintType)
struct CSKIT_API FCSKit_FloatValueMinMax
{
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY(EditDefaultsOnly, Category = CSKit_FloatValueMinMax, DisplayName = "最低値")
	float mValueMin = 0.f;
	UPROPERTY(EditDefaultsOnly, Category = CSKit_FloatValueMinMax, DisplayName = "最高値")
	float mValueMax = 1.f;

	FCSKit_FloatValueMinMax(){}
	FCSKit_FloatValueMinMax(const float InMin, const float InMax)
		:mValueMin(InMin)
		,mValueMax(InMax)
	{}

	float CalcValue(const float InRatio) const
	{
		return mValueMin + (mValueMax - mValueMin)*InRatio;
	}
	float CalcRatio(const float InValue) const
	{
		if(mValueMin < mValueMax)
		{
			return FMath::Clamp((InValue - mValueMin) / (mValueMax-mValueMin), 0.f, 1.f);
		}
		return 0.f;
	}
	float Clamp(const float InValue) const
	{
		return FMath::Clamp(InValue, mValueMin, mValueMax);
	}
	float Rand() const
	{
		return UCSKit_Subsystem::msRand.FRandRange(mValueMin, mValueMax);
	}
};

/* ------------------------------------------------------------
   !自然回復する値(0.0～1.0)
------------------------------------------------------------ */
USTRUCT(BlueprintType)
struct CSKIT_API FCSKit_AutoRecoveryValue
{
	GENERATED_USTRUCT_BODY()

	void Update(const float InDeltaSecond)
	{
		if(mRecoveryIntervalSec > 0.f)
		{
			mRecoveryIntervalSec -= InDeltaSecond;
			return;
		}
		mValue += mRecoverySpeed*InDeltaSecond;
		mValue = FMath::Clamp(mValue, 0.f, 1.f);
	}
	void RequestAdd()
	{
		mValue += mAddValue;
		mValue = FMath::Clamp(mValue, 0.f, 1.f);
		mRecoveryIntervalSec = mRecoveryIntervalTime;
	}
	void RequestSub()
	{
		mValue -= mSubValue;
		mValue = FMath::Clamp(mValue, 0.f, 1.f);
		mRecoveryIntervalSec = mRecoveryIntervalTime;
	}
	float GetValue() const{return mValue;} 
	void InitRandom()
	{
		mValue = UCSKit_Subsystem::msRand.FRandRange();
		mRecoveryIntervalSec = UCSKit_Subsystem::msRand.FRandRange(0.f, mRecoveryIntervalTime);
	}

protected:
	UPROPERTY(EditDefaultsOnly, Category = CSKit_AutoRecoveryValue, Meta = (DisplayName = "加算量", DisplayPriority = 1))
	float mAddValue = 0.f;
	UPROPERTY(EditDefaultsOnly, Category = CSKit_AutoRecoveryValue, Meta = (DisplayName = "減算量", DisplayPriority = 1))
	float mSubValue = 0.f;
	UPROPERTY(EditDefaultsOnly, Category = CSKit_AutoRecoveryValue, Meta = (DisplayName = "自然回復速度", DisplayPriority = 1))
	float mRecoverySpeed = 0.f;
	UPROPERTY(EditDefaultsOnly, Category = CSKit_AutoRecoveryValue, Meta = (DisplayName = "自然回復開始までの待ち時間", DisplayPriority = 1))
	float mRecoveryIntervalTime = 0.f;
private:
	float mValue = 0.f;
	float mRecoveryIntervalSec = 0.f;
};