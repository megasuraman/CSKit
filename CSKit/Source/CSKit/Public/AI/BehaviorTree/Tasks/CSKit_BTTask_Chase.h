// Copyright 2020 megasuraman.
/**
 * @file CSKit_BTTask_StopMove.h
 * @brief 追跡Task(単純に対象にMoveToだと機能不足なので)
 * @author megasuraman
 * @date 2025/05/24
 */
#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "CSKit_BTTask_Chase.generated.h"

class ACSKit_AIController;

//実行タスク単位の情報
struct FCSKit_ChaseTaskMemory
{
	FVector mOldNavigateGoalPos = FVector::ZeroVector;
	FAIRequestID mMoveRequestID;
	float mRevengeIntervalSec = 0.f;
	int32 mRevengeCount = 0;
	uint8 mbOriginalGoalInNavNullSpace : 1;
	uint8 mbDirectMove : 1;
	uint8 mbResultPathDirect : 1;//経路探索の結果直接移動可能だった

	FCSKit_ChaseTaskMemory()
		: mbOriginalGoalInNavNullSpace(false)
		, mbDirectMove(false)
		, mbResultPathDirect(false)
	{}
};


UCLASS(DisplayName = "CSKit Chase")
class CSKIT_API UCSKit_BTTask_Chase : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UCSKit_BTTask_Chase(const FObjectInitializer& ObjectInitializer);
	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual uint16 GetInstanceMemorySize() const override
	{
		return sizeof(FCSKit_ChaseTaskMemory);
	}
	virtual void DescribeRuntimeValues(const UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTDescriptionVerbosity::Type Verbosity, TArray<FString>& Values) const override;
	virtual FString GetStaticDescription() const override;

protected:
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	virtual EBTNodeResult::Type	RequestChase(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);
	virtual EBTNodeResult::Type	RequestChaseDirect(ACSKit_AIController& InAIController, const AActor& InTargetActor, FCSKit_ChaseTaskMemory* InTaskMemory);
	virtual EBTNodeResult::Type	RequestChaseNavigate(ACSKit_AIController& InAIController, const AActor& InTargetActor, FCSKit_ChaseTaskMemory* InTaskMemory);
	virtual void UpdateChase(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds);
	virtual void UpdateChaseDirect(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds);
	virtual EBTNodeResult::Type UpdateChaseNavigate(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds);
	virtual FVector FindBestTargetGoalPos(const AActor& InTarget) const;
	virtual bool CheckNearTarget(ACSKit_AIController& InAIController, const AActor& InTargetActor) const;
	virtual void OnRequestMove(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory){}
	virtual float GetCheckNearTargetRadius(const ACSKit_AIController& InAIController) const
	{
		return GetTaskFinishRadius(InAIController);
	}

	float GetTaskFinishRadius(const ACSKit_AIController& InAIController) const;
	static bool IsTargetSameGroundSpace(const ACSKit_AIController& InAIController);

protected:
	UPROPERTY(EditAnywhere, Category = "CSKit_BTTask_Chase", meta = (DisplayName = "タスク終了距離"))
	float mTaskFinishDistance = 100.f;
	UPROPERTY(VisibleAnywhere, Category = "CSKit_BTTask_Chase", meta = (DisplayName = "上書きTask終了半径", EditCondition = "mbOverrideTaskFinishRadius"))
	FBlackboardKeySelector mBBKey_OverrideFinishTaskRadius;
	UPROPERTY(EditAnywhere, Category = "CSKit_BTTask_Chase", meta = (DisplayName = "対象の移動先予測時間", EditCondition = "mbUseTargetPredictMovePos"))
	FBlackboardKeySelector mBBKey_TargetPredictMovePosSec;
	UPROPERTY(EditAnywhere, Category = "CSKit_BTTask_Chase", meta = (DisplayName = "リベンジ間隔"))
	float mRevengeIntervalTime = 0.3f;
	UPROPERTY(EditAnywhere, Category = "CSKit_BTTask_Chase", meta = (DisplayName = "最大リベンジ回数"))
	int32 mMaxRevengeNum = 3;
	UPROPERTY(EditAnywhere, Category = "CSKit_BTTask_Chase", meta = (DisplayName = "辿れる所まで辿る"))
	uint8 mbAllowPartialPath : 1;
	UPROPERTY(EditAnywhere, Category = "CSKit_BTTask_Chase", meta = (DisplayName = "Navmesh使用"))
	uint8 mbUseNavmesh : 1;
	UPROPERTY(EditAnywhere, Category = "CSKit_BTTask_Chase", meta = (InlineEditConditionToggle))
	uint8 mbOverrideTaskFinishRadius : 1;
	UPROPERTY(EditAnywhere, Category = "CSKit_BTTask_Chase", meta = (InlineEditConditionToggle))
	uint8 mbUseTargetPredictMovePos : 1;
};
