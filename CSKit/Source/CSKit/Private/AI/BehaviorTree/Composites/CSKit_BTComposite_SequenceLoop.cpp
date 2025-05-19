// Copyright 2020 megasuraman.
/**
 * @file CSKit_BTComposite_SequenceLoop.cpp
 * @brief Sequenceで実行し終わったら頭に戻ってループ
 * @author megasuraman
 * @date 2025/05/19
 */
#include "AI/BehaviorTree/Composites/CSKit_BTComposite_SequenceLoop.h"


UCSKit_BTComposite_SequenceLoop::UCSKit_BTComposite_SequenceLoop(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	NodeName = "CSKit_SequenceLoop";
}

int32 UCSKit_BTComposite_SequenceLoop::GetNextChildHandler(FBehaviorTreeSearchData& SearchData, int32 PrevChild, EBTNodeResult::Type LastResult) const
{
	// failure = quit
	int32 NextChildIdx = BTSpecialChild::ReturnToParent;

	if (PrevChild == BTSpecialChild::NotInitialized)
	{
		// newly activated: start from first
		NextChildIdx = 0;
	}
	else if (LastResult == EBTNodeResult::Succeeded)
	{
		if ((PrevChild + 1) < GetChildrenNum())
		{
			NextChildIdx = PrevChild + 1;
		}
		else
		{
			NextChildIdx = 0;
		}
	}

	return NextChildIdx;
}

#if WITH_EDITOR

bool UCSKit_BTComposite_SequenceLoop::CanAbortLowerPriority() const
{
	// don't allow aborting lower priorities, as it breaks sequence order and doesn't makes sense
	return false;
}

FName UCSKit_BTComposite_SequenceLoop::GetNodeIconName() const
{
	return FName("BTEditor.Graph.BTNode.Composite.Sequence.Icon");
}

#endif