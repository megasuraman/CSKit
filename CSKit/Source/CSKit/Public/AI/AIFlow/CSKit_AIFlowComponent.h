// Copyright 2020 megasuraman.
/**
 * @file CSKit_AIFlowComponent.h
 * @brief 配置での行動指定管理Component
 * @author megasuraman
 * @date 2025/05/05
 */
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CSKit_AIFlowComponent.generated.h"

class ACSKit_AIFlow;
struct FCSKit_AIFlowNodeData;
struct FCSKit_AIFlowActionTableRow;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CSKIT_API UCSKit_AIFlowComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCSKit_AIFlowComponent();
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void SetAIFlow(const ACSKit_AIFlow* InAIFlow);
	void SetupAIFlowNodeIndex();
	void StepNextAIFlowNodeIndex();
	const ACSKit_AIFlow* GetAIFlow() const;
	const FCSKit_AIFlowNodeData* GetNextNodeData() const;
	virtual class UBehaviorTree* GetNextNodeAction();
	bool IsFinished() const{return mbFinished;}

protected:
	virtual const FCSKit_AIFlowActionTableRow* GetAIFlowActionTableRow(const FName& InActionName) const;

private:
	TWeakObjectPtr<const ACSKit_AIFlow> mAIFlow;
	int32 mNextIndex = INDEX_NONE;
	bool mbFinished = false;

#if USE_CSKIT_DEBUG
public:
	virtual FString DebugDrawSelectedActorWatcher(UCanvas* InCanvas) const;
#endif
};
