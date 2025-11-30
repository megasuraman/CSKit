// Copyright 2020 megasuraman.
/**
 * @file CSKit_BTDecorator_OwnComponent.h
 * @brief BTDecorator Component所持判定
 * @author megasuraman
 * @date 2025/05/19
 */
#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "CSKit_BTDecorator_OwnComponent.generated.h"

UCLASS(DisplayName = "CSKit OwnComponent")
class CSKIT_API UCSKit_BTDecorator_OwnComponent : public UBTDecorator
{
	GENERATED_BODY()
public:
	UCSKit_BTDecorator_OwnComponent(const FObjectInitializer& ObjectInitializer);

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OwnComponent")
	TSubclassOf<UActorComponent> mClass;
	
#if WITH_EDITOR
public:
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};
