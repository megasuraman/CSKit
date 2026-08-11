// Copyright 2020 megasuraman.
/**
 * @file CSKit_VoxelNav.cpp
 * @brief Voxelによる経路探索機能提供
 * @author megasuraman
 * @date 2026/08/11
*/
#include "Voxel/CSKit_VoxelNav.h"

#include "DrawDebugHelpers.h"
#include "GraphAStar.h"
#include "Kismet/GameplayStatics.h"
#include "ScreenWindow/CSKitDebug_ScreenWindowText.h"

const TArray<FCSKit_VoxelIndex> FCSKit_VoxelNav::sAroundOffsetVoxelIndexList = {
	FCSKit_VoxelIndex(-1,-1,-1),
	FCSKit_VoxelIndex(-1,-1, 0),
	FCSKit_VoxelIndex(-1,-1, 1),
	FCSKit_VoxelIndex(-1, 0,-1),
	FCSKit_VoxelIndex(-1, 0, 0),
	FCSKit_VoxelIndex(-1, 0, 1),
	FCSKit_VoxelIndex(-1, 1,-1),
	FCSKit_VoxelIndex(-1, 1, 0),
	FCSKit_VoxelIndex(-1, 1, 1),
	
	FCSKit_VoxelIndex( 0,-1,-1),
	FCSKit_VoxelIndex( 0,-1, 0),
	FCSKit_VoxelIndex( 0,-1, 1),
	FCSKit_VoxelIndex( 0, 0,-1),
	//FCSKit_VoxelIndex( 0, 0, 0),
	FCSKit_VoxelIndex( 0, 0, 1),
	FCSKit_VoxelIndex( 0, 1,-1),
	FCSKit_VoxelIndex( 0, 1, 0),
	FCSKit_VoxelIndex( 0, 1, 1),
	
	FCSKit_VoxelIndex( 1,-1,-1),
	FCSKit_VoxelIndex( 1,-1, 0),
	FCSKit_VoxelIndex( 1,-1, 1),
	FCSKit_VoxelIndex( 1, 0,-1),
	FCSKit_VoxelIndex( 1, 0, 0),
	FCSKit_VoxelIndex( 1, 0, 1),
	FCSKit_VoxelIndex( 1, 1,-1),
	FCSKit_VoxelIndex( 1, 1, 0),
	FCSKit_VoxelIndex( 1, 1, 1),
};
/**
 * @brief	Linkしてる周囲のVoxelのBitFlag設定
 */
void FCSKit_VoxelNav::SetLinkBitFlag(const FCSKit_VoxelIndex& InIndex, const bool bInSet)
{
	const FCSKit_VoxelIndex OffsetIndex = InIndex - mVoxelIndex;
	const int32 ListIndex = sAroundOffsetVoxelIndexList.Find(OffsetIndex);
	if (ListIndex < 0
		|| ListIndex > 32)
	{
		ensure(false);
		return;
	}
	
	if (bInSet)
	{
		mAroundLinkBitFlag |= 1 << ListIndex;
	}
	else
	{
		mAroundLinkBitFlag &= ~(1 << ListIndex);
	}
}

/**
 * @brief	周囲のVoxelでLinkしてるのを取得
 */
TArray<FCSKit_VoxelIndex> FCSKit_VoxelNav::GetLinkIndexList() const
{
	TArray<FCSKit_VoxelIndex> LinkList;
	for (int32 i=0; i<sAroundOffsetVoxelIndexList.Num(); ++i)
	{
		if ( (mAroundLinkBitFlag & (1 << i)) != 0 )
		{
			LinkList.Add(mVoxelIndex + sAroundOffsetVoxelIndexList[i]);
		}
	}
	return LinkList;
}
/**
 * @brief	周囲にあるLink持ちのVoxelで一番近い奴
 */
FCSKit_VoxelIndex FCSKit_VoxelNavChunk::FindNearestSafeLinkVoxelIndex(const FCSKit_VoxelIndex& InIndex) const
{
	const FCSKit_VoxelNav* Voxel = FindVoxel(InIndex);
	if (Voxel == nullptr)
	{
		return FCSKit_VoxelIndex();
	}
	const TArray<FCSKit_VoxelIndex> LinkIndexList = Voxel->GetLinkIndexList();
	if (LinkIndexList.Num() != 0)
	{
		return InIndex;
	}
	TArray<FCSKit_VoxelIndex> AroundVoxelIndexList;
	InIndex.GetAroundVoxelList(AroundVoxelIndexList);
	FCSKit_VoxelIndex NearestSafeLinkVoxelIndex;
	int32 DistanceMin = INT_MAX;
	for (const FCSKit_VoxelIndex& VoxelIndex : AroundVoxelIndexList)
	{
		const FCSKit_VoxelNav* AroundVoxel = FindVoxel(VoxelIndex);
		if (AroundVoxel == nullptr
			|| AroundVoxel->GetLinkIndexList().Num() == 0)
		{
			continue;
		}
		const int32 Distance = InIndex.Distance(VoxelIndex);
		if (Distance < DistanceMin)
		{
			NearestSafeLinkVoxelIndex = VoxelIndex;
			DistanceMin = Distance;
		}
	}
	return NearestSafeLinkVoxelIndex;
}

/**
 * @brief	
 */
void FCSKit_VoxelNavChunk::SetupFilePath(const UWorld* InWorld)
{
	mFilePath = FPaths::ConvertRelativePathToFull(FPaths::ProjectContentDir());
	mFilePath += FString(TEXT("CSKit/VoxelNav"));
	const FString FileName = UGameplayStatics::GetCurrentLevelName(InWorld);
	mFilePath += FString::Printf(TEXT("/%s.bin"), *FileName);
}

/**
 * @brief	経路探索
 */
bool FCSKit_VoxelNavChunk::FindPath(
	TArray<FVector>& OutResult,
	const FVector& InStart,
	const FVector& InGoal,
	const FCSKit_VoxelNavFindPathOption& InOption
	) const
{
	FGraphAStar<FCSKit_VoxelNavChunk> AStarSolver(*this);

	FCSKit_VoxelIndex StartNode = GetVoxelIndex(InStart);
	if (InOption.mbReviseStartVoxelIndex)
	{
		StartNode = FindNearestSafeLinkVoxelIndex(StartNode);
	}
	if (!StartNode.IsValid())
	{
		return false;
	}
	
	FCSKit_VoxelIndex GoalNode = GetVoxelIndex(InGoal);
	if (InOption.mbReviseGoalVoxelIndex)
	{
		GoalNode = FindNearestSafeLinkVoxelIndex(GoalNode);
	}
	if (!GoalNode.IsValid())
	{
		return false;
	}
	
	if (!LineTrace(StartNode,GoalNode))
	{
		OutResult.Add(InStart);
		OutResult.Add(InGoal);
		return true;
	}
	
	const FCSKit_VoxelNavGraphQueryFilter Filter(*this, GoalNode);
	TArray<FCSKit_VoxelIndex> ResultPath;
	const EGraphAStarResult Result = AStarSolver.FindPath(StartNode, GoalNode, Filter, ResultPath);
	if (Result != EGraphAStarResult::SearchSuccess
		|| ResultPath.Num() <= 0)
	{
		return false;
	}
	
	if (InOption.mbUseAllVoxel)
	{
		for (const FCSKit_VoxelIndex& VoxelIndex : ResultPath)
		{
			OutResult.Add( GetVoxelPos(VoxelIndex) );
		}
		return true;
	}
	
	if (InOption.mbSmoothing)
	{
		SmoothingPath(OutResult, ResultPath, InStart, InGoal);
	}
	else
	{
		for (const FCSKit_VoxelIndex& VoxelIndex : ResultPath)
		{
			const FVector VoxelPos = GetVoxelPos(VoxelIndex);
			OutResult.Add(VoxelPos);
		}
		OutResult.Add(InGoal);
	}
	
	return true;
}

/**
 * @brief	経路探索結果から直線的な移動は間引く
 */
void FCSKit_VoxelNavChunk::SmoothingPath(
	TArray<FVector>& OutResult,
	const TArray<FCSKit_VoxelIndex>& InPath,
	const FVector& InStart,
	const FVector& InGoal
	) const
{
	FVector PreCheckPos = InStart;
	FVector BaseNV = FVector::ZeroVector;
	for (const FCSKit_VoxelIndex& VoxelIndex : InPath)
	{
		const FVector VoxelPos = GetVoxelPos(VoxelIndex);
		const FVector TargetNV = FVector(VoxelPos - PreCheckPos).GetSafeNormal();
		bool bWishAdd = false;
		if (BaseNV.IsZero())
		{
			bWishAdd = true;
		}
		else
		{
			const float DotValue = FVector::DotProduct(BaseNV, TargetNV);
			if (DotValue < 0.9f)
			{
				bWishAdd = true;
			}
		}
		
		if (bWishAdd)
		{
			OutResult.Add(PreCheckPos);
			PreCheckPos = VoxelPos;
			BaseNV = TargetNV;
		}
		else
		{
			PreCheckPos = VoxelPos;
		}
	}
	OutResult.Add(InGoal);
}

/**
 * @brief	2点間のVoxelの間に障害物Voxelあるかどうか
 */
bool FCSKit_VoxelNavChunk::LineTrace(const FVector& InBasePos, const FVector& InTargetPos) const
{
	const FCSKit_VoxelIndex TargetNode = GetVoxelIndex(InTargetPos);
	if (const FCSKit_VoxelNav* TargetVoxel = static_cast<const FCSKit_VoxelNav*>(FindVoxelBase(TargetNode)))
	{
		if (TargetVoxel->mbHitCollision)
		{
			return true;
		}
	}
	const FVector TargetNV = FVector(InTargetPos - InBasePos).GetSafeNormal();
	const float OffsetLength = GetVoxelLength() * 0.5f;
	const float TargetDistance = FVector::Distance(InBasePos, InTargetPos);
	float CheckLength = 0.f;
	while (CheckLength < TargetDistance)
	{
		if (CheckLength + OffsetLength >= TargetDistance)
		{
			CheckLength = TargetDistance;
		}
		const FVector CheckPos = InBasePos + TargetNV * CheckLength;
		const FCSKit_VoxelIndex CheckNode = GetVoxelIndex(CheckPos);
		if (const FCSKit_VoxelNav* CheckVoxel = static_cast<const FCSKit_VoxelNav*>(FindVoxelBase(CheckNode)))
		{
			if (CheckVoxel->mbHitCollision)
			{
				return true;
			}
		}
		
		CheckLength += OffsetLength;
	}
	return false;
}
bool FCSKit_VoxelNavChunk::LineTrace(
	const FCSKit_VoxelIndex& InBaseVoxelIndex,
	const FCSKit_VoxelIndex& InTargetVoxelIndex
	) const
{
	return LineTrace(GetVoxelPos(InBaseVoxelIndex), GetVoxelPos(InTargetVoxelIndex));
}

/**
 * @brief	隣接するノード（移動可能な隣のマス）を取得する
 */
void FCSKit_VoxelNavChunk::GetNeighbors(FNodeRef NodeRef, TArray<FNodeRef>& OutNeighbors) const
{
	if (const FCSKit_VoxelNav* Voxel = FindVoxel(NodeRef))
	{
		const TArray<FCSKit_VoxelIndex> LinkIndexList = Voxel->GetLinkIndexList();
		for (const FCSKit_VoxelIndex& VoxelIndex : LinkIndexList)
		{
			OutNeighbors.Add(VoxelIndex);
		}
	}
}

/**
 * @brief	A点からB点への実際の移動コスト（G値）
 */
// ReSharper disable once CppMemberFunctionMayBeStatic
float FCSKit_VoxelNavChunk::GetCost(FNodeRef From, FNodeRef To) const
{
	float Cost = 1.f;
	//高低差ある場合は加算
	if (From.mIndex.mZ != To.mIndex.mZ)
	{
		Cost += 0.5f;
	}
	return Cost;
}

/**
 * @brief	
 */
FCSKit_VoxelNavChunk::FNodeRef FCSKit_VoxelNavChunk::GetNeighbour(FNodeRef Node, int32 NeighbourIndex) const
{
	TArray<FNodeRef> NeighborList;
	GetNeighbors(Node, NeighborList);
	if (NeighbourIndex < NeighborList.Num())
	{
		return NeighborList[NeighbourIndex];
	}
	return FNodeRef();
}

#if USE_CSKIT_DEBUG

/**
 * @brief	
 */
void FCSKit_VoxelNavChunk::Draw(
	const UWorld* InWorld,
	UCanvas* InCanvas,
	const FVector& InBasePos,
	const int32 InDrawAroundNum
	) const
{
	const float VoxelExtentLen= GetVoxelLength() * 0.5f;
	const float DrawVoxelExtentLen = VoxelExtentLen - 1.f;//重なり回避のために少し小さく
	const FVector DrawVoxelExtentV(DrawVoxelExtentLen, DrawVoxelExtentLen, DrawVoxelExtentLen);
	
	const FCSKit_VoxelIndex LastSafeVoxelIndex = GetVoxelIndex(InBasePos);
	TArray<FCSKit_VoxelIndex> AroundVoxelIndexList;
	LastSafeVoxelIndex.GetAroundVoxelList(AroundVoxelIndexList, InDrawAroundNum);
	AroundVoxelIndexList.Add(LastSafeVoxelIndex);
	for (const FCSKit_VoxelIndex& VoxelIndex : AroundVoxelIndexList)
	{
		const FCSKit_VoxelNav* Voxel = FindVoxel(VoxelIndex);
		if (Voxel == nullptr)
		{
			continue;
		}
		const FVector VoxelPos = GetVoxelPos(VoxelIndex);

		FColor DrawColor = FColor::White;
		if (Voxel->mbHitCollision)
		{
			DrawColor = FColor::Orange;
		}
		
		DrawDebugBox(InWorld, VoxelPos, DrawVoxelExtentV, GetTransform().Rotator().Quaternion(), DrawColor);

		FCSKitDebug_ScreenWindowText Window;
		Window.SetWindowName(FString::Printf(TEXT("%s"), *VoxelIndex.ToString()));
		Window.SetWindowFrameColor(DrawColor);
		Window.AddText(FString::Printf(TEXT("mbHitCollision : %d"), Voxel->mbHitCollision));
		Window.Draw(InCanvas, VoxelPos, 200.f);
	}
}

/**
 * @brief	
 */
void FCSKit_VoxelNavChunk::DrawFindPathResult(const UWorld* InWorld,
	UCanvas* InCanvas,
	const FVector& InStart,
	const FVector& InGoal,
	const FCSKit_VoxelNavFindPathOption& InOption) const
{
	TArray<FVector> ResultPath;
	if (!FindPath(ResultPath, InStart, InGoal, InOption))
	{
		const FColor DrawColor = FColor::Red;
		DrawDebugLine(InWorld, InStart, InGoal, DrawColor, false, -1.f, 0, 3.f);
			
		FCSKitDebug_ScreenWindowText Window;
		Window.SetWindowName(FString::Printf(TEXT("Miss PathFind")));
		Window.SetWindowFrameColor(DrawColor);
		Window.AddText(FString::Printf(TEXT("Start : %s"), *InStart.ToString()));
		Window.AddText(FString::Printf(TEXT("Goal : %s"), *InGoal.ToString()));
		Window.Draw(InCanvas, InStart, 500.f);
		return;
	}
	
	const FColor DrawColor = FColor::Green;
	FVector PrePos = ResultPath[0];
	{
		FCSKitDebug_ScreenWindowText Window;
		Window.SetWindowName(FString::Printf(TEXT("%d"), 0));
		Window.SetWindowFrameColor(DrawColor);
		Window.AddText(FString::Printf(TEXT("%s"), *PrePos.ToString()));
		Window.Draw(InCanvas, PrePos, 500.f);
	}
	for (int32 i=1; i<ResultPath.Num(); ++i)
	{
		const FVector Pos = ResultPath[i];
		DrawDebugLine(InWorld, PrePos, Pos, DrawColor, false, -1.f, 0, 3.f);
			
		FCSKitDebug_ScreenWindowText Window;
		Window.SetWindowName(FString::Printf(TEXT("%d"), i));
		Window.SetWindowFrameColor(DrawColor);
		Window.AddText(FString::Printf(TEXT("%s"), *Pos.ToString()));
		Window.Draw(InCanvas, Pos, 500.f);
			
		PrePos = Pos;
	}
}
#endif