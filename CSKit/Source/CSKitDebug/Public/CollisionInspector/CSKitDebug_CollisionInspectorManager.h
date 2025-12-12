// Copyright 2020 megasuraman
/**
 * @file CSKitDebug_CollisionInspectorManager.h
 * @brief コリジョンチェック管理クラス
 * @author megasuraman
 * @date 2025/12/12
 */
#pragma once

#include "CoreMinimal.h"
#include "CSKitDebug_CollisionInspectorManager.generated.h"

class UCSKitDebug_CollisionInspectorComponent;

UCLASS()
class UCSKitDebug_CollisionInspectorManager : public UObject
{
	GENERATED_BODY()

public:
	static UCSKitDebug_CollisionInspectorManager* sGet(const UWorld* InWorld);
	void Init();
	void Entry(UCSKitDebug_CollisionInspectorComponent* InComponent);
	void Exit(UCSKitDebug_CollisionInspectorComponent* InComponent);
	void DebugTick(const float InDeltaSec);
	void DebugDraw(UCanvas* InCanvas);
	void AttachComponentToPlayer() const;
	void DetachComponentToPlayer() const;
	void OutputErrorFile() const;

private:
	TArray<TWeakObjectPtr<UCSKitDebug_CollisionInspectorComponent>> mComponentList;
	bool mbDrawLastSafePoint = false;
	bool mbDrawVoxel = false;
	bool mbDrawError = false;
};
