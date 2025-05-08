// Copyright 2020 megasuraman.
/**
 * @file CSKit_CommunityComponent.h
 * @brief AIのチーム制御用Component
 * @author megasuraman
 * @date 2025/05/06
 */
#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CSKit_CommunityComponent.generated.h"

class UCSKit_CommunityNodeBase;

DECLARE_MULTICAST_DELEGATE(FCSKit_OnEntryCommunity);
DECLARE_MULTICAST_DELEGATE(FCSKit_OnExitCommunity);

UCLASS(ClassGroup = (AI), HideCategories = (Variable, Sockets, ComponentReplication, Activation, Cooking), meta = (BlueprintSpawnableComponent))
class CSKIT_API UCSKit_CommunityComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCSKit_CommunityComponent();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	void Update(const float InDeltaSec);

	void RequestEntryCommunity(UCSKit_CommunityComponent* InPartner);
	void RequestExitCommunity();

	UCSKit_CommunityNodeBase*	GetEntryNode() const { return mEntryCommunityNode.Get(); }
	bool IsOwnCommunity() const
	{
		return (mEntryCommunityNode.Get() != nullptr);
	}
	bool IsOwnActiveCommunity() const
	{
		return (GetMemberNum() >= 2);
	}
	int32 GetMemberNum() const;
	int32 GetLogMemberNumMax() const;
	TSubclassOf<UCSKit_CommunityNodeBase> GetNodeClass() const { return mNodeClass; }
	bool IsLeader() const;
	bool IsMember(const UCSKit_CommunityComponent* InTarget) const;
	bool IsMemberActor(const AActor* InTarget) const;
	void RequestChangeLeader() const;
	FVector	GetOwnerPos() const;
	APawn* GetLeaderPawn() const;
	void AddDelegateOnEntryCommunity(const FCSKit_OnEntryCommunity::FDelegate& InDelegate);
	void AddDelegateOnExitCommunity(const FCSKit_OnExitCommunity::FDelegate& InDelegate);

	//UCSKit_CommunityNodeBaseから操作用
	bool EntryCommunityByNode(UCSKit_CommunityNodeBase* InNode);
	bool ExitCommunityByNode(const UCSKit_CommunityNodeBase* InNode);

protected:
	void OnEntryCommunity() const;
	void OnExitCommunity() const;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CSKit_Community")
	TSubclassOf<UCSKit_CommunityNodeBase> mNodeClass;

private:
	TWeakObjectPtr<UCSKit_CommunityNodeBase> mEntryCommunityNode;
	FCSKit_OnEntryCommunity mDelegateOnEntryCommunity;
	FCSKit_OnExitCommunity mDelegateOnExitCommunity;
};
