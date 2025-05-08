// Copyright 2020 megasuraman.
/**
 * @file CSKit_WorryComponent.h
 * @brief Actor心配管理Component(AIPerceptionComponentのHearing代わり)
 * @author megasuraman
 * @date 2025/05/06
 */
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AI/Worry/CSKit_WorryDataTable.h"
#include "CSKit_WorryComponent.generated.h"

class UCSKit_TerritoryComponent;

struct FCSKit_WorryPoint
{
	TWeakObjectPtr<const AActor> mSourceActor;
	FVector mPos = FVector::ZeroVector;
	float mRadius = 100.f;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CSKIT_API UCSKit_WorryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCSKit_WorryComponent();
	virtual void BeginPlay() override;

	virtual void Setup(const FCSKit_WorryAbilityTableRow& InData);
	void SetTerritoryComponent(const UCSKit_TerritoryComponent* InComponent);
	void Update(const float InDeltaSec);
	bool IsWishCheckWorryPoint() const{return mbWishCheckWorryPoint;}
	FVector GetWishCheckPos() const;
	void SetSleep(const bool bInSleep);
	void GiveUpWorry();
	const FCSKit_WorryPoint& GetWorryPoint() const{return mWorryPoint;}

protected:
	virtual bool IsSleep() const{return mbSleep;}
	virtual void OnBeginWishCheckWorryPoint(){}
	virtual void OnEndWishCheckWorryPoint(){}
	virtual bool IsHideTarget(const AActor* InTarget) const{return false;};

	void SetWishCheckWorryPoint(const bool bInWish);
	FVector CheckVisibleWorrySource(const float InBorderDistanceSq) const;
	virtual bool CheckCollisionVisible(const FVector& InBasePos, AActor* InTarget) const;
	bool IsNearGiveUpPos(const FVector& InTargetPos) const;
	void UpdateGiveUpSec(const float InDeltaSec);
	bool IsOutsideTerritory(const FVector& InTargetPos) const;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "CSKit_WorryComponent")
	float mCheckVisibleIntervalTime = 1.f;

	FCSKit_WorryAbilityTableRow mAbility;
	FCSKit_WorryPoint mWorryPoint;
	FVector mGiveUpWorryPos;
	TWeakObjectPtr<const UCSKit_TerritoryComponent> mTerritoryComponent;
	float mGiveUpInterval = 0.f;
	float mCheckVisibleInterval = 0.f;
	bool mbWishCheckWorryPoint = false;
	bool mbSleep = false;

#if USE_CSKIT_DEBUG
public:
	virtual FString DebugDrawSelectedActorWatcher(UCanvas* InCanvas) const;
protected:
	void DebugDrawAbilityRange() const;
	void DebugDrawWorryPoint(UCanvas* InCanvas) const;
	void DebugDrawGiveUpWorryPos(UCanvas* InCanvas) const;
#endif
};
