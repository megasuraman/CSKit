// Copyright 2020 megasuraman
/**
 * @file CSKitDebug_CollisionInspectorComponent.h
 * @brief コリジョンチェック用Component
 * @author megasuraman
 * @date 2025/12/12
 */

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CSKitDebug_CollisionInspectorVoxel.h"
#include "CSKitDebug_CollisionInspectorError.h"
#include "CSKitDebug_CollisionInspectorComponent.generated.h"

UCLASS(meta = (BlueprintSpawnableComponent))
class CSKITDEBUG_API UCSKitDebug_CollisionInspectorComponent : public UActorComponent
{
	GENERATED_BODY()
	
	UCSKitDebug_CollisionInspectorComponent();

public:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
	
	void DebugDraw(UCanvas* InCanvas) const;
	const FCollisionInspectorError& GetError() const{return mError;}
	
protected:
	void UpdateLastSafeTransform();
	bool CheckPassThrough(const AActor& InOwnerActor);
	bool CheckStuck(const AActor& InOwnerActor);
	void UpdateVoxel(FCollisionInspectorVoxelChunk& InVoxelChunk) const;
	void CheckTooTight(const FCollisionInspectorVoxelChunk& InVoxelChunk);
	void CheckTooTight(const FCollisionInspectorVoxelChunk& InVoxelChunk, const FCollisionInspectorVoxel& InVoxel);
	void CheckNoGround(const FCollisionInspectorVoxelChunk& InVoxelChunk);
	void CheckNoGround(const FCollisionInspectorVoxelChunk& InVoxelChunk, const FCollisionInspectorVoxel& InVoxel);
	bool CheckHitCollision_Line(FHitResult& OutResult, const FVector& InBasePos, const FVector& InTargetPos) const;
	bool CheckHitCollision_Sphere(FHitResult& OutResult, const FVector& InBasePos, const FVector& InTargetPos, const float InRadius, const bool bInIgnoreTouches) const;
	
	void OnError_PassThrough(const FHitResult& InHitResult, const FTransform& InTransform);
	void OnError_Stuck(const FHitResult& InHitResult, const FTransform& InTransform, const float InRadius);
	void OnError_Penetrating(const FHitResult& InHitResult, const FTransform& InTransform, const float InRadius);
	
	void DrawLastSafeTransform(UCanvas* InCanvas) const;
	void DrawVoxel(UCanvas* InCanvas, const FCollisionInspectorVoxelChunk& InVoxelChunk) const;
	
private:
	FCollisionInspectorVoxelChunk mVoxelChunk;
	FCollisionInspectorError mError;
	FTransform mLastSafeTransform;
	float mTooTightLengthBorder = 84.f;
	bool mbGetLastSafeTransform = false;
};