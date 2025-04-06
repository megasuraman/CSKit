// Copyright 2020 megasuraman.
/**
 * @file CSKit_RecastNavMesh.h
 * @brief 
 * @author megasuraman
 * @date 2025/04/06
 */
#pragma once

#include "CoreMinimal.h"
#include "NavMesh/RecastNavMesh.h"
#include "CSKit_RecastNavMesh.generated.h"


#if WITH_EDITOR
enum class ECSKit_GenerateNavLinkPhase : uint8
{
	Invalid,
	Init,
	WaitBuildNavmesh,
	GenerateNavLinkDoor,
	SetupNavMeshEdge,
	WaitBuildNavmeshFinal,
	Finish,
};
/* ------------------------------------------------------------
   !NavMeshEdgeを定義しとく(TArray<FVector>の対で扱うのはやりにくいので)
------------------------------------------------------------ */
struct FCSKit_NavMeshEdge
{
	FVector	mBegin = FVector::ZeroVector;
	FVector	mEnd = FVector::ZeroVector;
	FVector mDirectionNV = FVector(1.f, 0.f, 0.f);
	FVector mOutsideNV = FVector(0.f, 1.f, 0.f);
	int32	mTileIndex = 0;
	uint32	mUID = UINT32_MAX;

	FORCEINLINE bool operator==(const FCSKit_NavMeshEdge& InRef) const
	{
		return mUID == InRef.mUID;
	}

	FCSKit_NavMeshEdge() {}
	FCSKit_NavMeshEdge(const uint32 InUID)
		:mUID(InUID)
	{}
	FCSKit_NavMeshEdge(const FVector& InBegin, const FVector& InEnd, const int32 InTileIndex, const uint32 InUID)
		:mBegin(InBegin)
		,mEnd(InEnd)
		,mTileIndex(InTileIndex)
		,mUID(InUID)
	{
		mDirectionNV = FVector(mEnd - mBegin).GetSafeNormal();

		// GetPerpendicularClockwiseで"エッジからエッジの外側"に向くベクトルを取得する.
		mOutsideNV = FVector(-mDirectionNV.Y, mDirectionNV.X, mDirectionNV.Z);
		mOutsideNV.Z = 0.f;
		mOutsideNV = mOutsideNV.GetSafeNormal();
	}
	FVector	GetCenter() const
	{
		return (mBegin + mEnd)*0.5f;
	}
	float	GetHalfLength() const
	{
		return FVector::Distance(mBegin, mEnd)*0.5f;
	}
};
#endif

UCLASS()
class CSKIT_API ACSKit_RecastNavMesh : public ARecastNavMesh
{
	GENERATED_BODY()

public:
	ACSKit_RecastNavMesh(const FObjectInitializer& ObjectInitializer);
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void RequestRebuild();
	
#if USE_CSKIT_DEBUG
public:
	bool DebugCollectPolyEdgePoint(TArray<FVector>& OutList, const float InInterval) const;
	void DebugRequestDrawSafeNavMesh(const bool bInDraw);
protected:
	void DebugRequestDraw(const bool bInActive);
	void DebugDraw(UCanvas* InCanvas, class APlayerController* InPlayerController);
	void DebugDrawNavmeshTriangle(UCanvas* InCanvas) const;
	void DebugUpdateSafeNavMeshTriangle();
	void DebugDrawSafeNavMesh();
private:
	TArray<FVector> mDebugSafeNavMeshTrianglePointList;
	TArray<FVector> mDebugSafePosList;
	FDelegateHandle mDebugDrawHandle;
	int32 mDebugNextCollectSafeNavMeshTileIndex = 0;
	bool mbDebugDrawSafeNavMesh = false;
	bool mbDebugFinishCollectSafeNavMesh = false;
#endif
	
#if WITH_EDITOR
public:
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;

	UFUNCTION(BlueprintCallable, Category = "CSKit_GenerateNavLink", meta = (CallInEditor = "true", DisplayName = "NavLink生成"))
	void EditorGenerateNavLink();
	UFUNCTION(BlueprintCallable, Category = "CSKit_GenerateNavLink")
	bool EditorIsGeneratingNavLink() const;

protected:
	bool EditorTick(float InDeltaSec);
	void EditorSetGenerateNavLinkPhase(ECSKit_GenerateNavLinkPhase InPhase);
	bool EditorGenerateNavLink_Init();
	void EditorSetupSpawnLevel();
	void EditorSetupFolderPath();
	void EditorSetupSafePosList();
	void EditorDeleteNavLinkProxyDoor() const;
	bool EditorSetupNavMeshEdge();
	bool EditorIsSafePath(const FVector& InTargetPos) const;
	bool EditorGenerateNavLink_Door() const;
	bool EditorGenerateNavLink_Finish() const;
	
	void EditorDrawNavMeshEdge(UCanvas* InCanvas) const;
#endif
	
#if WITH_EDITORONLY_DATA
protected:
	UPROPERTY(EditInstanceOnly, Transient, Category = "CSKit_RecastNavMesh")
	TSubclassOf<AActor> mDoorClass;
	UPROPERTY(EditInstanceOnly, Transient, Category = "CSKit_RecastNavMesh")
	uint32 mEditorFixNavEdgeUID = 0;
	UPROPERTY(EditInstanceOnly, Transient, Category = "CSKit_RecastNavMesh")
	bool mbEditorPossibleToDraw = false;
	UPROPERTY(EditInstanceOnly, Transient, Category = "CSKit_RecastNavMesh")
	bool mbEditorDrawTriangle = false;
	UPROPERTY(EditInstanceOnly, Transient, Category = "CSKit_RecastNavMesh")
	bool mbEditorDrawPlyEdgePoint = false;
	UPROPERTY(EditInstanceOnly, Transient, Category = "CSKit_RecastNavMesh")
	bool mbEditorDrawNavMeshEdge = false;
	UPROPERTY(EditInstanceOnly, Transient, Category = "CSKit_RecastNavMesh")
	bool mbEditorFixGenerateNavLinkFall = false;

#if ENGINE_MAJOR_VERSION == 4
	FDelegateHandle mEditorTickDelegateHandle;
#else
	FTSTicker::FDelegateHandle mEditorTickDelegateHandle;
#endif
	TMap<int32, TArray<FCSKit_NavMeshEdge>> mEditorTileNavMeshEdgeMap;
	TArray<FVector> mEditorSafePosList;
	FString mEditorGenerateNavLinkMessage;
	FString	mEditorSpawnFolderPath;
	TWeakPtr<SNotificationItem> mEditorNotificationGenerateNavLink;
	TWeakObjectPtr<ULevel> mEditorSpawnLevel = nullptr;
	double mEditorGenerateNavLinkStartTime = 0.0;
	int32 mEditorNavMeshEdgeNextTileIndex = 0;
	uint32 mEditorNavMeshEdgeUIDSource = 0;
	ECSKit_GenerateNavLinkPhase mEditorGenerateNavLinkPhase = ECSKit_GenerateNavLinkPhase::Invalid;
#endif
};
