// Copyright 2020 megasuraman.
/**
 * @file CSKit_BTDecorator_OwnComponent.cpp
 * @brief BTDecorator Component所持判定
 * @author megasuraman
 * @date 2025/05/19
 */
#include "AI/BehaviorTree/Decorators/CSKit_BTDecorator_OwnComponent.h"

#include "AIController.h"
#include "BehaviorTree/BTCompositeNode.h"


UCSKit_BTDecorator_OwnComponent::UCSKit_BTDecorator_OwnComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	NodeName = "CSKit Component";
}

bool	UCSKit_BTDecorator_OwnComponent::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	const AAIController* AIController = Cast<AAIController>(OwnerComp.GetOwner());
	if(AIController == nullptr)
	{
		return false;
	}
	if (AIController->GetComponentByClass(mClass) == nullptr)
	{
		return false;
	}
	return true;
}

#if WITH_EDITOR
void UCSKit_BTDecorator_OwnComponent::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	FString ClassName;
	if (const UClass* Class = mClass.Get())
	{
		ClassName = Class->GetName();
	}
	NodeName = FString::Printf(TEXT("IsOwn %s"), *ClassName);
}
#endif