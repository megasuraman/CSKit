// Copyright 2020 megasuraman.
/**
 * @file CSKit_BTComposite_SequenceLoop.h
 * @brief Sequenceで実行し終わったら頭に戻ってループ
 * @author megasuraman
 * @date 2025/05/19
 */
#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "BehaviorTree/BTCompositeNode.h"
#include "CSKit_BTComposite_SequenceLoop.generated.h"


UCLASS(DisplayName = "CSKit Sequence Loop")
class CSKIT_API UCSKit_BTComposite_SequenceLoop : public UBTCompositeNode
{
	GENERATED_UCLASS_BODY()

	virtual int32 GetNextChildHandler(struct FBehaviorTreeSearchData& SearchData, int32 PrevChild, EBTNodeResult::Type LastResult) const override;

#if WITH_EDITOR
	virtual bool CanAbortLowerPriority() const override;
	virtual FName GetNodeIconName() const override;
#endif
};
