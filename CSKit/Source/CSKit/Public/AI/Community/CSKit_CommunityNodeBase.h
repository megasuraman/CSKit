// Copyright 2020 megasuraman.
/**
 * @file CSKit_CommunityComponent.h
 * @brief AIのチーム制御ノード
 * @author megasuraman
 * @date 2025/05/06
 */
#pragma once

#include "CoreMinimal.h"
#include "CSKit_CommunityNodeBase.generated.h"

class UCSKit_CommunityComponent;
class ACSKit_AIController;

UCLASS()
class CSKIT_API UCSKit_CommunityNodeBase : public UObject
{
	GENERATED_BODY()

public:
	UCSKit_CommunityNodeBase(const FObjectInitializer& ObjectInitializer);
	virtual void BeginPlay(){}
	virtual void Tick(const float InDeltaSec);
	virtual bool IsLeader(const UCSKit_CommunityComponent* InCommunityComponent) const
	{
		return (mLeader.Get() == InCommunityComponent);
	}

	bool Entry(UCSKit_CommunityComponent* InComponent);
	bool Exit(UCSKit_CommunityComponent* InComponent);
	void ExitAll();
	bool IsOwn(const UCSKit_CommunityComponent* InComponent) const;
	int32 GetMemberNum() const { return mMemberList.Num(); }
	int32 GetLogMemberNumMax() const{return mLogMemberNumMax;}
	void RequestChangeLeader();
	void RequestDelete() { mbRequestDelete = true; }
	bool IsRequestDelete() const { return mbRequestDelete; }
	const FVector& GetCenterPos() const{return mCenterPos;}
	float GetCenterRadius() const{return mCenterRadius;}
	UCSKit_CommunityComponent* GetLeader() const;
	const TArray<TWeakObjectPtr<UCSKit_CommunityComponent>>&	GetMemberList() const { return mMemberList; }
	void GetMemberAIControllerList(TArray<ACSKit_AIController*>& OutList) const;
	void GetMemberPawnList(TArray<APawn*>& OutList) const;
	void SetIgnoreCollisionMember(const bool bInIgnore) const;
	bool IsOwnNoticeTarget() const {return mbOwnNoticeTarget;}

protected:
	virtual	void OnEntry(UCSKit_CommunityComponent* InNewMember){}
	virtual	void OnExit(UCSKit_CommunityComponent* InExitMember){}
	virtual	void OnChangeOwnNoticeTarget(const bool bInOwnNoticeTarget){}
	virtual void OnPreExitAll(){}
	FVector	CalcCenterPos() const;
	float CalcCenterRadius() const;
	void CheckOwnNoticeTarget();

private:
	FVector mCenterPos = FVector::ZeroVector;
	TArray<TWeakObjectPtr<UCSKit_CommunityComponent>> mMemberList;
	TWeakObjectPtr<UCSKit_CommunityComponent> mLeader;
	float mCenterRadius = 100.f;
	int32 mLogMemberNumMax = 0;
	uint8 mbRequestDelete : 1;
	uint8 mbOwnNoticeTarget : 1;

#if USE_CSKIT_DEBUG
public:
	void DebugDraw(class UCanvas* InCanvas) const;
protected:
	virtual	void DebugAddDrawInfo(struct FCSKitDebug_ScreenWindowText& InInfo) const{}
#endif
};
