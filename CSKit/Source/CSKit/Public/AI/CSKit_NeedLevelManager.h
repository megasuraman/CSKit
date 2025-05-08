// Copyright 2020 megasuraman.
/**
 * @file CSKit_NeedLevelManager.h
 * @brief AIの処理LOD管理
 * @author megasuraman
 * @date 2025/05/05
 */
#pragma once

#include "CoreMinimal.h"
#include "CSKit_NeedLevelManager.generated.h"

class ACSKit_AIController;

UENUM(BlueprintType)
enum class ECSKit_NeedLevel : uint8
{
	Invalid,
	High,
	Low,
	Zero,

	Num,
};

struct CSKIT_API FCSKit_NeedLevelTarget
{
public:
	FCSKit_NeedLevelTarget();
	FCSKit_NeedLevelTarget(ACSKit_AIController* InAIController);
	bool operator==(const FCSKit_NeedLevelTarget InTarget) const
	{
		return (mAIController == InTarget.mAIController);
	}

	ACSKit_AIController* GetAIController() const;
	ECSKit_NeedLevel GetNeedLevel() const { return mNeedLevel; }
	bool SetNeedLevel(const ECSKit_NeedLevel InLevel);
	bool IsUseFixedPos() const;
	FVector GetFixedPos() const{return mFixedPos;}
	float GetOffsetRadius() const{return mOffsetRadius;}
	void SetFixedPos(const FVector& InPos){mFixedPos = InPos;}
	void SetOffsetRadius(const float InRadius){mOffsetRadius=InRadius;}

protected:
	void OnChangeNeedLevel(const ECSKit_NeedLevel InOldLevel, const ECSKit_NeedLevel InNewLevel) const;
	static void SetSleepActor(AActor* InTarget, const bool bInSleep);

private:
	FVector mFixedPos;
	TWeakObjectPtr<ACSKit_AIController> mAIController;
	float mOffsetRadius = 0.f;
	ECSKit_NeedLevel mNeedLevel = ECSKit_NeedLevel::Invalid;
};

UCLASS()
class CSKIT_API UCSKit_NeedLevelManager : public UObject
{
	GENERATED_BODY()
	
public:
	UCSKit_NeedLevelManager();

	virtual void Update(const float InDeltaSec);

	void EntryFactor(const AActor* InActor);
	void ExitFactor(const AActor* InActor);

	void EntryTarget(ACSKit_AIController* InAIController);
	void ExitTarget(ACSKit_AIController* InAIController);
	FCSKit_NeedLevelTarget* FindNeedLevelTarget(const ACSKit_AIController* InAIController);

	void SetNeedLevelDistanceBorder(const ECSKit_NeedLevel InLevel, const float InDistance);
	float GetNeedLevelDistanceBorder(const ECSKit_NeedLevel InLevel) const;

protected:
	bool CollectFactorPosList(TArray<FVector>& OutList) const;
	void UpdateTarget(FCSKit_NeedLevelTarget& InTarget, const TArray<FVector>& InFactorPosList);
	ECSKit_NeedLevel CalcTargetNeedLevel(const FCSKit_NeedLevelTarget& InTarget, const TArray<FVector>& InFactorPosList) const;
	virtual void OnChangedNeedLevel(const FCSKit_NeedLevelTarget& InTarget, const ECSKit_NeedLevel InOldNeedLevel){}

private:
	TArray<TWeakObjectPtr<const AActor>> mFactorList;
	TArray<FCSKit_NeedLevelTarget> mTargetList;
	float mNeedLevelDistanceBorder[static_cast<uint8>(ECSKit_NeedLevel::Num)];
	float mNeedLevelDistanceOffset = 300.f;
	int32 mFrameCheckTargetMaxNum = 32;//1フレームにチェックする最大Target数
	int32 mNextTargetListIndex = 0;

#if USE_CSKIT_DEBUG
public:
	virtual void DebugDraw(UCanvas* InCanvas);
	void DebugSetFixNeedLevel(const ECSKit_NeedLevel InLevel) { mDebugFixNeedLevel = InLevel; }
	void DebugSetDrawInfo(const bool bInDraw) { mbDebugDrawInfo = bInDraw; }
	void DebugSetDrawTargetInfo(const bool bInDraw) { mbDebugDrawTargetInfo = bInDraw; }
protected:
	void DebugDrawInfo(UCanvas* InCanvas);
	void DebugDrawTargetInfo(UCanvas* InCanvas);
private:
	ECSKit_NeedLevel mDebugFixNeedLevel = ECSKit_NeedLevel::Invalid;
	bool mbDebugDrawInfo = false;
	bool mbDebugDrawTargetInfo = false;
#endif
};
