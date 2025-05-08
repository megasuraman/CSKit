// Copyright 2020 megasuraman.
/**
 * @file CSKit_CommunityManager.h
 * @brief AIのチーム制御管理
 * @author megasuraman
 * @date 2025/05/06
 */
#pragma once

#include "CoreMinimal.h"
#include "CSKit_CommunityManager.generated.h"

class UCSKit_CommunityNodeBase;
class UCSKit_CommunityComponent;
class UCanvas;
class APlayerController;

UCLASS()
class CSKIT_API UCSKit_CommunityManager : public UObject
{
	GENERATED_BODY()
	
public:
	void Update(const float InDeltaSec);

public:
	UCSKit_CommunityNodeBase*	CreateCommunity(UCSKit_CommunityComponent* InMember);
	void	DeleteCommunity(UCSKit_CommunityNodeBase* InCommunityNode);

protected:

private:
	UPROPERTY()
	TArray<UCSKit_CommunityNodeBase*> mNodeList;

#if USE_CSKIT_DEBUG
public:
	void DebugDraw(UCanvas* InCanvas);

	void DebugSetDrawNodeList(const bool bInDraw){mbDebugDrawNodeList = bInDraw;}
private:
	bool mbDebugDrawNodeList = false;
#endif
};
