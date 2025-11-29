// Copyright 2020 megasuraman.
/**
 * @file CSKit_BTDecorator_BrainQuery.h
 * @brief BTDecorator BrainQueryの結果判定用
 * @author megasuraman
 * @date 2025/05/05
 */
#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType.h"
#include "AI/BrainQuery/CSKit_BrainQueryTestSelector.h"
#include "CSKit_BTDecorator_BrainQuery.generated.h"

/**
 * 
 */
UCLASS(DisplayName = "CSKit BrainQuery")
class CSKIT_API UCSKit_BTDecorator_BrainQuery : public UBTDecorator
{
	GENERATED_BODY()
public:
	UCSKit_BTDecorator_BrainQuery(const FObjectInitializer& ObjectInitializer);

	/** initialize any asset related data */
	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
	virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual EBlackboardNotificationResult OnBlackboardKeyValueChange(const UBlackboardComponent& Blackboard, FBlackboard::FKey ChangedKeyID);

protected:
	UPROPERTY(EditAnywhere, Category = "Condition", meta = (DisplayName = "Test", DisplayPriority = 1))
	FCSKit_BrainQueryTestSelector mTestSelector;
	UPROPERTY(VisibleAnywhere, Category = "Condition", meta = (DisplayName = "判定対象", DisplayPriority = 1))
	FBlackboardKeySelector mBBKey;

#if WITH_EDITOR
public:	
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
protected:
	void EditorCheckBBKey();
#endif
};
