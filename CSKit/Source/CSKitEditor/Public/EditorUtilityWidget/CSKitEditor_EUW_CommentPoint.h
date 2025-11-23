// Copyright 2020 megasuraman
/**
 * @file CSKitEditor_EUW_CommentPoint.h
 * @brief CommentPoint操作EUW
 * @author megasuraman
 * @date 2025/03/29
 */
#pragma once

#include "CoreMinimal.h"
#include "CSKitEditor_EUW_Base.h"
#include "CSKitDebug_CommentPoint.h"
#include "CSKitEditor_EUW_CommentPoint.generated.h"

UCLASS()
class CSKITEDITOR_API UCSKitEditor_EUW_CommentPoint : public UCSKitEditor_EUW_Base
{
	GENERATED_BODY()

public:
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
	
protected:
	UFUNCTION(BlueprintCallable, Category = "CommentPoint")
	void DeleteAlreadyReadAllPoint();
	UFUNCTION(BlueprintCallable, Category = "CommentPoint")
	void CollectCommentPointList();
	UFUNCTION(BlueprintCallable, Category = "CommentPoint")
	void GetCommentPointDisplayStringList(TArray<FString>& OutCommentPointTitleList) const;
	UFUNCTION(BlueprintCallable, Category = "CommentPoint")
	ACSKitDebug_CommentPoint* GetCommentPoint(const int32 InIndex) const;
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "EnemyWeakPoint")
	void OnChangeCollectCommentPointListBP();

	virtual void Draw(UCanvas* InCanvas, APlayerController* InPlayerController) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Filter", Meta = (DisplayName = "重要度", DisplayPriority = 2))
	ECSKitDebug_CommentPriority mFilterPriority = ECSKitDebug_CommentPriority::Invalid;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Filter", Meta = (DisplayName = "最終コメントが自分以外", DisplayPriority = 2))
	bool mbLastMessageByOther = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Draw", Meta = (DisplayName = "全非表示", DisplayPriority = 1))
	bool mbNoDraw = false;

	TArray<TSoftObjectPtr<ACSKitDebug_CommentPoint>> mCommentPointList;
};
