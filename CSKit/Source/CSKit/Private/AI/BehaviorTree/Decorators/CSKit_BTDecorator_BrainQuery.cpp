// Copyright 2020 megasuraman.
/**
 * @file CSKit_BTDecorator_BrainQuery.cpp
 * @brief BTDecorator BrainQueryの結果判定用
 * @author megasuraman
 * @date 2025/05/05
 */
#include "AI/BehaviorTree/Decorators/CSKit_BTDecorator_BrainQuery.h"

#include "CSKit_Config.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Name.h"
#include "BehaviorTree/BTCompositeNode.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AI/BrainQuery/CSKit_BrainQueryDataTable.h"


UCSKit_BTDecorator_BrainQuery::UCSKit_BTDecorator_BrainQuery(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	NodeName = "CSKit BrainQuery";
	FlowAbortMode = EBTFlowAbortMode::Self;
#if ENGINE_MAJOR_VERSION == 5
	INIT_DECORATOR_NODE_NOTIFY_FLAGS();
#endif

	mBBKey.AddNameFilter(this, GET_MEMBER_NAME_CHECKED(UCSKit_BTDecorator_BrainQuery, mBBKey));

#if WITH_EDITOR
	const UCSKit_Config* CSKitConfig = GetDefault<UCSKit_Config>();
	mTestSelector.mEditorDataTablePathList.Add(CSKitConfig->mEditorBrainQuery_DataTable->GetPathName());
	mTestSelector.mEditorDisplayName = FString(TEXT("Test"));
#endif
}

/* ------------------------------------------------------------
   !FBlackboardKeySelectorの初期化のために
------------------------------------------------------------ */
void UCSKit_BTDecorator_BrainQuery::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);

	if (const UBlackboardData* BBAsset = GetBlackboardAsset())
	{
		mBBKey.ResolveSelectedKey(*BBAsset);
	}
	else
	{
		UE_LOG(LogBehaviorTree, Warning, TEXT("Can't initialize %s due to missing blackboard data."), *GetName());
		mBBKey.InvalidateResolvedKey();
	}
}

bool	UCSKit_BTDecorator_BrainQuery::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	const UBlackboardComponent* MyBlackboard = OwnerComp.GetBlackboardComponent();
	const FName TargetName = MyBlackboard->GetValue<UBlackboardKeyType_Name>(mBBKey.GetSelectedKeyID());
	return mTestSelector.mTestName == TargetName;
}

void UCSKit_BTDecorator_BrainQuery::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	//Super::OnBecomeRelevant(OwnerComp, NodeMemory);
	
	if (UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent())
	{
		const auto KeyID = mBBKey.GetSelectedKeyID();
		BlackboardComp->RegisterObserver(KeyID, this, FOnBlackboardChangeNotification::CreateUObject(this, &UCSKit_BTDecorator_BrainQuery::OnBlackboardKeyValueChange));
	}
}

EBlackboardNotificationResult UCSKit_BTDecorator_BrainQuery::OnBlackboardKeyValueChange(const UBlackboardComponent& Blackboard, FBlackboard::FKey ChangedKeyID)
{
	UBehaviorTreeComponent* BehaviorComp = Cast<UBehaviorTreeComponent>(Blackboard.GetBrainComponent());
	if (BehaviorComp == nullptr)
	{
		return EBlackboardNotificationResult::RemoveObserver;
	}

	if (mBBKey.GetSelectedKeyID() == ChangedKeyID)
	{
		BehaviorComp->RequestExecution(this);		
	}
	return EBlackboardNotificationResult::ContinueObserving;
}

#if WITH_EDITOR
void UCSKit_BTDecorator_BrainQuery::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	NodeName = mTestSelector.mTestName.ToString();
	EditorCheckBBKey();
}

void UCSKit_BTDecorator_BrainQuery::EditorCheckBBKey()
{
	const FString& DataTablePath = mTestSelector.mEditorDataTablePathList[0];
	const UDataTable* DataTable = LoadObject<UDataTable>(nullptr, *DataTablePath, nullptr, LOAD_None, nullptr);
	if (DataTable == nullptr)
	{
		return;
	}
	const FCSKit_BrainQueryTableRow* BrainQueryTableRow = DataTable->FindRow<FCSKit_BrainQueryTableRow>(mTestSelector.mEditorDataTableRowName,TEXT(""));
	if(BrainQueryTableRow == nullptr)
	{
		return;
	}
	for (const FCSKit_BrainQueryTest& BrainQueryTest : BrainQueryTableRow->mTest)
	{
		if(BrainQueryTest.mTestName != mTestSelector.mTestName)
		{
			continue;
		}

		mBBKey.SelectedKeyName = BrainQueryTest.mBlackboardKeyName;
		break;
	}
}
#endif
