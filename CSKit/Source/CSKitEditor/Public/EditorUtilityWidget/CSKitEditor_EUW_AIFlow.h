// Copyright 2020 megasuraman.
/**
 * @file CSKitEditor_EUW_AIFlow.h
 * @brief AIFlow操作
 * @author megasuraman
 * @date 2025/05/06
 */ 
#pragma once

#include "CoreMinimal.h"
#include "CSKitEditor_EUW_Base.h"
#include "CSKitEditor_EUW_AIFlow.generated.h"

class ACSKit_AIFlowNode;

UCLASS(BlueprintType, Blueprintable)
class CSKITEDITOR_API UCSKitEditor_EUW_AIFlow : public UCSKitEditor_EUW_Base
{
	GENERATED_BODY()

public:
	UCSKitEditor_EUW_AIFlow();
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

protected:
	virtual void NativeConstruct() override;
	virtual void FakeTick() override;
	virtual void Draw(UCanvas* InCanvas, APlayerController* InPlayerController) override;
	
	UFUNCTION(BlueprintCallable, Category = "EUW_AIFlow")
	void RequestSetupAllAIFlowData();
	UFUNCTION(BlueprintCallable, Category = "EUW_AIFlow")
	void RequestSetLinkSelectNodeStraight();

	void DrawAllAIFlow(UCanvas* InCanvas) const;
	void UpdateSelectNodeList();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EUW_AIFlow", Meta = (DisplayName = "全AIFlow表示"))
	bool mbDrawAllAIFlow = false;
private:
	TArray<TSoftObjectPtr<ACSKit_AIFlowNode>> mSelectNodeList;
};
