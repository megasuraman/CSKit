// Copyright 2020 megasuraman
/**
 * @file CSKit_NavigationSystem.h
 * @brief 
 * @author megasuraman
 * @date 2025/04/06
 */
#pragma once

#include "CoreMinimal.h"
#include "NavigationSystem.h"
#include "CSKit_NavigationSystem.generated.h"

class ACSKit_GameState;

UCLASS()
class CSKIT_API UCSKit_NavigationSystem : public UNavigationSystemV1
{
	GENERATED_BODY()
	
public:
	UCSKit_NavigationSystem(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void Tick(float DeltaSeconds) override;

	void RequestRebuild();
	void RequestCheckFirstCreate();
	bool IsFinishFirstCreate() const{return mbFinishedFirstCreate;}

protected:
	void UpdateCheckFirstCreate(float DeltaSeconds);
	void OnLoadedStage();

private:
	float mFirstCreateNoTaskSec = 0.f;//一定時間タスクない状態が続いてるか判定するため
	uint8 mbRequestCheckFirstCreate : 1;
	uint8 mbFinishedFirstCreate : 1;

#if USE_CSKIT_DEBUG
public:
	void DebugRequestDrawSafeNavMesh(const bool bInDraw);
protected:
	void DebugResetCheckFirstCreate();
private:
	float mDebugFirstCreateCheckTotalSec = 0.f;
#endif
	
#if WITH_EDITOR
public:
	void EditorRequestGenerateNavLink();
	bool EditorIsGeneratingNavLink() const;
#endif
};
