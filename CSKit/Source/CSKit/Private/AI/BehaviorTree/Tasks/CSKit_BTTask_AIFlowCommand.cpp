// Copyright 2020 megasuraman.
/**
 * @file CSKit_BTTask_AIFlowCommand.cpp
 * @brief AIFlow操作
 * @author megasuraman
 * @date 2025/06/03
 */ 
#include "AI/BehaviorTree/Tasks/CSKit_BTTask_AIFlowCommand.h"

#include "CSKitDebug_Utility.h"
#include "CSKit_Config.h"
#include "AI/CSKit_AIController.h"
#include "AI/AIFlow/CSKit_AIFlowComponent.h"
#include "AI/AIFlow/CSKit_AIFlowNode.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Bool.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Float.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"


UCSKit_BTTask_AIFlowCommand::UCSKit_BTTask_AIFlowCommand(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	NodeName = TEXT("CSKit_AIFlowCommand");
}

/**
 * @brief Task開始
 */
EBTNodeResult::Type UCSKit_BTTask_AIFlowCommand::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
   switch (mCommandType)
   {
   case ECSKit_BTTaskAIFlowCommandType::Setup:
      return Setup(OwnerComp);
   case ECSKit_BTTaskAIFlowCommandType::StepNext:
      return StepNext(OwnerComp);
   case ECSKit_BTTaskAIFlowCommandType::SetBlackboard:
      return SetBlackboard(OwnerComp);
   default:
   break;
   }
   return EBTNodeResult::Failed;
}

/**
 * @brief 
 */
FString UCSKit_BTTask_AIFlowCommand::GetStaticDescription() const
{
	const FString KeyDesc("invalid");

	return FString::Printf(TEXT("%s: %s"), *Super::GetStaticDescription(), *KeyDesc);
}

/**
 * @brief AIFlowNode準備
 */
EBTNodeResult::Type UCSKit_BTTask_AIFlowCommand::Setup(UBehaviorTreeComponent& OwnerComp)
{
   const ACSKit_AIController* AIController = Cast<ACSKit_AIController>(OwnerComp.GetOwner());
   if(AIController == nullptr)
   {
      return EBTNodeResult::Failed;
   }

   UCSKit_AIFlowComponent* AIFlowComponent = AIController->GetCSKitAIFlowComponent();
   if(AIFlowComponent == nullptr)
   {
      return EBTNodeResult::Failed;
   }

   AIFlowComponent->SetupAIFlowNodeIndex();
   const UCSKit_Config* CSKitConfig = GetDefault<UCSKit_Config>();
   OwnerComp.SetDynamicSubtree(CSKitConfig->mAIFlowNodeTag, AIFlowComponent->GetNextNodeAction());

   return EBTNodeResult::Succeeded;
}

/**
 * @brief AIFlowNodeを次へ進める
 */
EBTNodeResult::Type UCSKit_BTTask_AIFlowCommand::StepNext(UBehaviorTreeComponent& OwnerComp)
{
   const ACSKit_AIController* AIController = Cast<ACSKit_AIController>(OwnerComp.GetOwner());
   if(AIController == nullptr)
   {
      return EBTNodeResult::Failed;
   }

   UCSKit_AIFlowComponent* AIFlowComponent = AIController->GetCSKitAIFlowComponent();
   if(AIFlowComponent == nullptr)
   {
      return EBTNodeResult::Failed;
   }

   AIFlowComponent->StepNextAIFlowNodeIndex();
   if (AIFlowComponent->IsFinished())
   {
      return EBTNodeResult::Failed;
   }
   const UCSKit_Config* CSKitConfig = GetDefault<UCSKit_Config>();
   OwnerComp.SetDynamicSubtree(CSKitConfig->mAIFlowNodeTag, AIFlowComponent->GetNextNodeAction());

   return EBTNodeResult::Succeeded;
}

/**
 * @brief AIFlowの値をBlackboardにセット
 */
EBTNodeResult::Type UCSKit_BTTask_AIFlowCommand::SetBlackboard(UBehaviorTreeComponent& OwnerComp) const
{
   UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
   const ACSKit_AIController* AIController = Cast<ACSKit_AIController>(OwnerComp.GetOwner());
   if(BlackboardComponent == nullptr
      || AIController == nullptr)
   {
      return EBTNodeResult::Failed;
   }

   const UCSKit_AIFlowComponent* AIFlowComponent = AIController->GetCSKitAIFlowComponent();
   if(AIFlowComponent == nullptr)
   {
      return EBTNodeResult::Failed;
   }
   const FCSKit_AIFlowNodeData* AIFlowNodeData = AIFlowComponent->GetNextNodeData();
   if(AIFlowNodeData == nullptr)
   {
      return EBTNodeResult::Failed;
   }

   if (BlackboardKey.SelectedKeyType == UBlackboardKeyType_Vector::StaticClass())
   {
      BlackboardComponent->SetValue<UBlackboardKeyType_Vector>(BlackboardKey.GetSelectedKeyID(), AIFlowNodeData->mPos);
   }
   else if (BlackboardKey.SelectedKeyType == UBlackboardKeyType_Float::StaticClass())
   {
      BlackboardComponent->SetValue<UBlackboardKeyType_Float>(BlackboardKey.GetSelectedKeyID(), AIFlowNodeData->mWaitTimeAfterAction);
   }
	
   return EBTNodeResult::Succeeded;
}

#if WITH_EDITOR
/**
 * @brief 
 */
void UCSKit_BTTask_AIFlowCommand::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
   Super::PostEditChangeProperty(PropertyChangedEvent);
   
   NodeName = FString::Printf(TEXT("AIFlowCommand %s"), *UCSKitDebug_Utility::GetUEnumString(mCommandType));
}
#endif