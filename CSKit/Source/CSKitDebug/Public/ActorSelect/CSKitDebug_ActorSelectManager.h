// Copyright 2020 megasuraman
/**
 * @file CSKitDebug_ActorSelectManager.h
 * @brief デバッグ選択管理用Manager
 * @author megasuraman
 * @date 2020/05/27
 */
#pragma once

#include "CoreMinimal.h"
#include "CSKitDebug_ActorSelectManager.generated.h"

class UCanvas;
class UCSKitDebug_ActorSelectComponent;
class ADebugCameraController;
struct FCSKitDebugMenuNodeGetter;

UCLASS()
class CSKITDEBUG_API UCSKitDebug_ActorSelectManager : public UObject
{
	GENERATED_BODY()

#if USE_CSKIT_DEBUG
public:
	static UCSKitDebug_ActorSelectManager* sGet(const UWorld* InWorld);

	void	Init() const;
	bool	DebugTick(float InDeltaSecond);
	void	DebugDraw(class UCanvas* InCanvas);
	void	EntryDebugSelectComponent(UCSKitDebug_ActorSelectComponent* InComponent);
	void	ExitDebugSelectComponent(UCSKitDebug_ActorSelectComponent* InComponent);

	bool	IsShowPathFollow() const { return mbShowSelectPathFollow; }
	bool	IsShowLastEQS() const { return mbShowSelectLastEQS; }

protected:
	ADebugCameraController* GetActiveDebugCameraController() const;
	void	CheckDebugCameraController();
	void	CheckSelectTarget();
	void	OnSelect(const AActor* InActor);
	static void	SetActiveTickActor(AActor* InActor, const bool bInActive);

	void	DrawInfo(UCanvas* InCanvas) const;
	void	DrawSelectMarker(UCanvas* InCanvas) const;
	void	DrawMarkAllSelectList(UCanvas* InCanvas);

	void	SetOnlyUpdateSelectActor(const bool bInOnlyUpdate);

private:
	TWeakObjectPtr<ADebugCameraController>	mDebugCameraController;
	TArray<TWeakObjectPtr<UCSKitDebug_ActorSelectComponent>>	mAllSelectList;
	TArray<TWeakObjectPtr<UCSKitDebug_ActorSelectComponent>>	mSelectList;
	bool	mbActive = false;
	bool	mbOnlyUpdateSelectActor = false;
	bool	mbShowInfo = false;
	bool	mbShowMark = false;
	bool	mbShowSelectAxis = false;
	bool	mbShowSelectBone = false;
	bool	mbShowSelectPathFollow = false;
	bool	mbShowSelectLastEQS = false;
	bool	mbShowSelectBehaviorTree = false;
	bool	mbShowSelectPerception = false;

#endif//USE_CSKIT_DEBUG
};
