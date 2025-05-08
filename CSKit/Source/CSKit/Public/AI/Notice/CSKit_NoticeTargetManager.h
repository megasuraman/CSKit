// Copyright 2020 megasuraman.
/**
 * @file CSKit_NoticeComponent.h
 * @brief 全AIのNoticeTarget情報の取りまとめ
 * @author megasuraman
 * @date 2025/05/05
 */
#pragma once

#include "CoreMinimal.h"
#include "AI/CSKit_AIController.h"
#include "CSKit_NoticeTargetManager.generated.h"

class ACSKit_AIController;

struct FCSKit_SameNoticeMember
{
	TWeakObjectPtr<ACSKit_AIController> mAIController;
	float mScore = 0.f;
	
	void Update();
};
struct FCSKit_SameNoticeGroup
{
	TArray<FCSKit_SameNoticeMember> mMemberList;

	void Update();
};

UCLASS()
class CSKIT_API UCSKit_NoticeTargetManager : public UObject
{
	GENERATED_BODY()
	
public:
	UCSKit_NoticeTargetManager();

	void Update(const float InDeltaSec);

	void Entry(AActor* InTarget, ACSKit_AIController* InAIController);
	void Exit(AActor* InTarget, ACSKit_AIController* InAIController);
	int32 CalcMemberNum(AActor* InTarget, const ACSKit_AIController* InIgnoreAIController) const;
	const FCSKit_SameNoticeGroup* FindSameNoticeGroup(AActor* InTarget) const;

protected:

private:
	TMap<TWeakObjectPtr<AActor>, FCSKit_SameNoticeGroup> mSameNoticeGroupMap;

#if USE_CSKIT_DEBUG
public:
	void DebugDraw(UCanvas* InCanvas) const;

	void DebugSetDrawInfo(const bool bInDraw){mbDebugDrawInfo=bInDraw;}
protected:
	void DebugDrawInfo(UCanvas* InCanvas) const;
	void DebugDrawSameNoticeGroupInfo(UCanvas* InCanvas) const;
	void DebugDrawSameNoticeGroupInfo(UCanvas* InCanvas, const AActor& InTarget, const FCSKit_SameNoticeGroup& InGroup, const FColor InColor) const;
private:
	bool mbDebugDrawInfo = false;
#endif
};
