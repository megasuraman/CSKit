// Copyright 2020 megasuraman.
/**
 * @file CSKit_PathFollowingComponent.h
 * @brief 慣性ブレーキや方向基準での移動機能等を追加
 * @author megasuraman
 * @date 2025/05/06
 */
#pragma once

#include "CoreMinimal.h"
#include "Navigation/PathFollowingComponent.h"
#include "CSKit_PathFollowingComponent.generated.h"

UCLASS()
class CSKIT_API UCSKit_PathFollowingComponent : public UPathFollowingComponent
{
	GENERATED_BODY()

public:
	UCSKit_PathFollowingComponent(const FObjectInitializer& ObjectInitializer);

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
	virtual FAIRequestID RequestMove(const FAIMoveRequest& RequestData, FNavPathSharedPtr InPath) override;
	virtual void OnPathFinished(const FPathFollowingResult& Result) override;
	virtual FVector GetMoveFocus(bool bAllowStrafe) const override;

	float GetNavPathGoalActorTetherDistance() const { return mNavPathGoalActorTetherDistance; }
	void RequestDirectMoveToActor() { mbDirectMoveToActor = true; }
	bool IsPlayingDirectMoveToActor(const AActor* InTarget) const;
	void RequestDirectMoveControl() { mbDirectMoveControl = true; }
	void SetDirectMoveControlTarget(const FVector& InTarget);
	bool IsPlayingDirectMoveControl() const;
	void RequestMoveFocusNoticeTarget() { mbMoveFocusNoticeTarget = true; }
	FVector GetGoalPos() const;
	bool IsStraightPath() const;
	void OverrideInertial(const bool bInOwnInertial, const float InInertialBreak);

protected:
	virtual void FollowPathSegment(float DeltaTime) override;
	virtual void SetMoveSegment(int32 SegmentStartIndex) override;
#if ENGINE_MAJOR_VERSION == 5
	virtual bool ShouldStopMovementOnPathFinished() const override;
#else
	virtual bool ShouldCheckPathOnResume() const override;
#endif
	
	void UpdateDirectMoveToActor();
	void FollowPathSegmentFocusBasedMoveMode(const float InDeltaTime);
	void UpdateManualMoveFocusPos(float& OutYaw, const float InDeltaTime);
	virtual float GetManualMoveFocusRotationSpeed() const {return 180.f;}
	void CheckOverrideJointPathNodeAcceptanceRadius();
	void UpdateInertialBreak(const float InDeltaTime);
	void ReviseFocusPos(FVector& OutPos, float& OutYaw) const;
	virtual void ReviseMoveVelocityOnFocusBasedMoveMode(FVector& OutMoveVelocity, const float InDeltaTime){}

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CSKit_PathFollowingComponent", Meta = (DisplayName = "指定角度へのFocus補正"))
	TArray<float> mPossibleToFocusYawAngleList;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CSKit_PathFollowingComponent")
	float mNavPathGoalActorTetherDistance = 100.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CSKit_PathFollowingComponent", Meta = (DisplayName = "中継点の到着半径"))
	float mJointPathNodeAcceptanceRadius = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CSKit_PathFollowingComponent", Meta = (DisplayName = "慣性ブレーキ", EditCondition = "mbOwnInertial"))
	float mInertialBreak = 1000.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CSKit_PathFollowingComponent", Meta = (DisplayName = "Focus先(キャラの向き方向)基準で移動"))
	uint8 mbFocusBasedMoveMode : 1;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CSKit_PathFollowingComponent", Meta = (DisplayName = "Focus先の移動ベクトルのZを消す", EditCondition = "mbFocusBasedMoveMode"))
	uint8 mbFocusBasedMoveVelocityXY : 1;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CSKit_PathFollowingComponent", Meta = (DisplayName = "慣性アリ"))
	uint8 mbOwnInertial : 1;

private:
	FVector mManualMoveFocusPos = FVector::ZeroVector;
	FVector mInertialSpeedV = FVector::ZeroVector;
	float mManualMoveFocusRotationSpeed = 0.f;
	uint8 mbDirectMoveToActor : 1;
	uint8 mbDirectMoveControl : 1;
	uint8 mbMoveFocusNoticeTarget : 1;
	uint8 mbBreakManualMoveFocusRotationSpeed : 1;

#if USE_CSKIT_DEBUG
public:
	virtual FString DebugDrawSelectedActorWatcher(UCanvas* InCanvas) const;
	virtual FString DebugGetDetailLog() const;

	void DebugDrawPath(UCanvas* InCanvas) const;
	void DebugSetFindPathPos(const FVector& InStartPos, const FVector& InGoalPos)
	{
		mDebugFindPathStartPos = InStartPos;
		mDebugFindPathGoalPos = InGoalPos;
	}
private:
	FVector mDebugFindPathStartPos = FVector::ZeroVector;
	FVector mDebugFindPathGoalPos = FVector::ZeroVector;
	FVector mDebugBlockedPathStartPos = FVector::ZeroVector;
	FVector mDebugBlockedPathGoalPos = FVector::ZeroVector;
#endif
};
