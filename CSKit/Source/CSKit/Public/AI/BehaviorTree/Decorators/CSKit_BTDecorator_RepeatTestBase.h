// Copyright 2020 megasuraman.
/**
 * @file CSKit_BTDecorator_RepeatTestBase.cpp
 * @brief BTDecorator ノード変化時以外も繰り返し条件チェック
 * @author megasuraman
 * @date 2025/05/19
 */
#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "CSKit_BTDecorator_RepeatTestBase.generated.h"

UCLASS(abstract)
class CSKIT_API UCSKit_BTDecorator_RepeatTestBase : public UBTDecorator
{
	GENERATED_BODY()

public:
	UCSKit_BTDecorator_RepeatTestBase(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditAnywhere, Category = "RepeatTestBase", meta = (DisplayName = "テスト間隔(秒)"))
	float	mTestIntervalSec = 1.f;
	UPROPERTY(EditAnywhere, Category = "RepeatTestBase", meta = (DisplayName = "OneShot化"))
	uint8	mbForceOneShot : 1;//ちょっと矛盾してるけど、OneShotとRepeatの両方用意するのが大変なので
	
protected:
	virtual void OnNodeActivation(FBehaviorTreeSearchData& SearchData) override;
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
