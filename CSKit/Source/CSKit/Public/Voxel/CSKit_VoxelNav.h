// Copyright 2020 megasuraman.
/**
 * @file CSKit_VoxelNav.h
 * @brief Voxelによる経路探索機能提供
 * @author megasuraman
 * @date 2026/08/11
*/
#pragma once

#include "CoreMinimal.h"
#include "CSKit_Voxel.h"

struct FCSKit_VoxelNav : public FCSKit_Voxel
{
	static const TArray<FCSKit_VoxelIndex> sAroundOffsetVoxelIndexList;
	uint32 mAroundLinkBitFlag = 0; //周囲26個のVoxelでLink通ってるかどうかのBitFlag
	uint8 mbHitCollision : 1;

	FCSKit_VoxelNav()
		:mbHitCollision(false)
	{}
	
	virtual uint8 GetLastDataVersion() const override
	{
		return 1;
	};
	virtual void Serializable(FArchive& Ar) override
	{
		uint8 TempBitFlag = 0; //BitFieldの変数はそのままだとArchiveできないので、BitFlagを経由

		if (Ar.IsSaving())
		{
			if (mbHitCollision) TempBitFlag |= (1 << 0);
		}
		Ar << mAroundLinkBitFlag;
		Ar << TempBitFlag;
		if (Ar.IsLoading())
		{
			mbHitCollision = (TempBitFlag & (1 << 0)) != 0;
		}
	}
	
	void SetLinkBitFlag(const FCSKit_VoxelIndex& InIndex, const bool bInSet);
	TArray<FCSKit_VoxelIndex> GetLinkIndexList() const;
};

struct FCSKit_VoxelNavFindPathOption
{
	bool mbUseAllVoxel = false;
	bool mbReviseStartVoxelIndex = false;
	bool mbReviseGoalVoxelIndex = false;
	bool mbSmoothing = true;
};

//
struct CSKIT_API FCSKit_VoxelNavChunk : public FCSKit_VoxelChunk<FCSKit_VoxelNav>
{
public:
	virtual uint8 GetLastVersion() const override
	{
		return 1;
	};
	virtual FString GetFilePath() const override
	{
		return mFilePath;
	};
	
	FCSKit_VoxelIndex FindNearestSafeLinkVoxelIndex(const FCSKit_VoxelIndex& InIndex) const;

	void SetupFilePath(const UWorld* InWorld);
	
	bool FindPath(TArray<FVector>& OutResult, const FVector& InStart, const FVector& InGoal, const FCSKit_VoxelNavFindPathOption& InOption) const;
	void SmoothingPath(TArray<FVector>& OutResult, const TArray<FCSKit_VoxelIndex>& InPath, const FVector& InStart, const FVector& InGoal)
		const;
	bool LineTrace(const FVector& InBasePos, const FVector& InTargetPos) const;
	bool LineTrace(const FCSKit_VoxelIndex& InBaseVoxelIndex, const FCSKit_VoxelIndex& InTargetVoxelIndex) const;

	/***FGraphAStarによる経路探索用***/
	typedef FCSKit_VoxelIndex FNodeRef;
	// 隣接するノード（移動可能な隣のマス）を取得する
	void GetNeighbors(FNodeRef NodeRef, TArray<FNodeRef>& OutNeighbors) const;
	// A点からB点への実際の移動コスト（G値）
	float GetCost(FNodeRef From, FNodeRef To) const;
	// ゴールまでの予測コスト（H値 / ヒューリスティック）
	// ReSharper disable once CppMemberFunctionMayBeStatic
	float GetHeuristicCost(FNodeRef From, FNodeRef To) const
	{
		return From.Distance(To);
	}
	bool IsValidRef(FNodeRef NodeRef) const
	{
		return NodeRef.IsValid() && IsOwnVoxel(NodeRef);
	}
	// ReSharper disable once CppMemberFunctionMayBeStatic
	int32 GetNeighbourCount(FNodeRef NodeRef) const
	{
		return 26;
	} //3*3*3 - 1
	int32 GetNeighbourCountV2(FNodeRef NodeRef) const
	{
		return GetNeighbourCount(NodeRef);
	}
	FNodeRef GetNeighbour(FNodeRef Node, int32 NeighbourIndex) const;
	/******/
private:
	FString mFilePath;

#if USE_CSKIT_DEBUG
	void Draw(const UWorld* InWorld, UCanvas* InCanvas, const FVector& InBasePos, const int32 InDrawAroundNum) const;
	void DrawFindPathResult(
		const UWorld* InWorld, UCanvas* InCanvas, const FVector& InStart, const FVector& InGoal, const FCSKit_VoxelNavFindPathOption& InOption
	) const;
#endif
};

//FGraphAStarによる経路探索用のFilter
struct FCSKit_VoxelNavGraphQueryFilter
{
	typedef FCSKit_VoxelNavChunk FGraph;
	const FGraph& mGraph;
	const FGraph::FNodeRef mGoalNode;

	FCSKit_VoxelNavGraphQueryFilter(const FGraph& InGraph, const FGraph::FNodeRef& InGoal)
		: mGraph(InGraph),
		  mGoalNode(InGoal)
	{
	}

	// 隣接する2点間の実際の移動コスト (G)
	// ReSharper disable once CppMemberFunctionMayBeStatic
	float GetTraversalCost(const FGraph::FNodeRef From, const FGraph::FNodeRef To) const
	{
		return 1.0f;
	}

	// 実際の移動コスト
	float GetCost(const FGraph::FNodeRef From, const FGraph::FNodeRef To) const
	{
		return mGraph.GetCost(From, To);
	}

	// 予測コスト（ヒューリスティック）
	float GetHeuristicCost(const FGraph::FNodeRef Node, const FGraph::FNodeRef Goal) const
	{
		return mGraph.GetHeuristicCost(Node, Goal);
	}

	// ゴールに到達したか
	bool IsGoal(const FGraph::FNodeRef Node) const
	{
		return Node == mGoalNode;
	}

	// 特定のノードを除外したい場合（壁など）
	// ReSharper disable once CppMemberFunctionMayBeStatic
	bool IsExcluded(const FGraph::FNodeRef Node) const
	{
		return false;
	}

	// ReSharper disable once CppMemberFunctionMayBeStatic
	float GetHeuristicScale() const
	{
		return 1.f;
	}
	// ReSharper disable once CppMemberFunctionMayBeStatic
	bool WantsPartialSolution() const
	{
		return true;
	}
	// ReSharper disable once CppMemberFunctionMayBeStatic
	bool IsTraversalAllowed(FGraph::FNodeRef NodeRefA, FGraph::FNodeRef NodeRefB) const
	{ //移動の可否
		return true;
	}
};