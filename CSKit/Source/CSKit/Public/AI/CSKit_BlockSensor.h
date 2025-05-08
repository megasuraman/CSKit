// Copyright 2020 megasuraman.
/**
 * @file CSKit_BlockSensor.h
 * @brief 移動引っ掛かりチェック
 * @author megasuraman
 * @date 2025/05/06
 */
#pragma once

#include "CoreMinimal.h"
#include "CSKit_BlockSensor.generated.h"


USTRUCT(BlueprintType)
struct CSKIT_API FCSKit_BlockSensor
{
	GENERATED_USTRUCT_BODY()

	FCSKit_BlockSensor();

public:
	bool Update(float InDeltaTime, const FVector& InPos);
	void Reset();
	bool IsBlock() const { return mbBlock; }
	void SetActive(const bool InActive) { mbActive = InActive; }
	void ChangeLogSize(const uint32 InSize)
	{
		mLocationLog.Empty();
		mLocationLog.Reserve(InSize);
	}
	void SetLocationLogInterval(const float InInterval) { mLocationLogInterval = InInterval; }
	void SetCheckBlockDistanceBorder(const float InDistance) { mCheckBlockDistanceBorder = InDistance; }

protected:
	bool UpdateLocationLog(float InDeltaTime, const FVector& InPos);
	bool UpdateBlock();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "CSKit_BlockSensor")
	float mLocationLogInterval = 0.1f;
	UPROPERTY(EditDefaultsOnly, Category = "CSKit_BlockSensor")
	float mCheckBlockDistanceBorder = 10.f;

private:
	TArray<FVector>	mLocationLog;
	float mLocationLogTime = 0.f;
	uint8 mNextLocationLogIndex = 0;
	uint8 mbActive : 1;
	uint8 mbBlock : 1;
	uint8 mbCompleteLocationLog : 1;

#ifdef USE_CSKIT_DEBUG
public:
	void DebugDraw(const class UWorld* InWorld) const;
#endif
};