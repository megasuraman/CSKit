// Copyright 2020 megasuraman.
/**
 * @file CSKit_NavModifierComponent.h
 * @brief NavModiferComponent 範囲指定を上書き
 * @author megasuraman
 * @date 2025/11/29
 */
#pragma once

#include "CoreMinimal.h"
#include "NavModifierComponent.h"
#include "CSKit_NavModifierComponent.generated.h"

UCLASS(meta = (BlueprintSpawnableComponent))
class CSKIT_API UCSKit_NavModifierComponent : public UNavModifierComponent
{
	GENERATED_BODY()
	
	virtual void CalcAndCacheBounds() const override;

protected:
	void SetupManualCacheBounds() const;
	void OffsetBoundsExtent() const;
	static void OffsetBoundsExtent(FBox& InBox, const FVector& InExtent);
	void OffsetBoundsPos() const;

protected:
	UPROPERTY(EditAnywhere, Category = "CSKit_NavModifier", meta = (DisplayName = "位置Offset"))
	FVector mOffsetBoundsPos = FVector::ZeroVector;
	UPROPERTY(EditAnywhere, Category = "CSKit_NavModifier", meta = (DisplayName = "範囲Offset"))
	FVector mOffsetBoundsExtent = FVector::ZeroVector;
	UPROPERTY(EditAnywhere, Category = "CSKit_NavModifier", meta = (DisplayName = "範囲手動指定", EditCondition = "mbUseManualBoundsExtent"))
	FVector mManualBoundsExtent = FVector(100.f,100.f,100.f);
	UPROPERTY(EditAnywhere, Category = "CSKit_NavModifier", meta = (InlineEditConditionToggle))
	bool mbUseManualBoundsExtent = false;
};