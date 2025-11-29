// Copyright 2020 megasuraman.
/**
 * @file CSKit_NavModifierComponent.cpp
 * @brief NavModiferComponent 範囲指定を上書き
 * @author megasuraman
 * @date 2025/11/29
 */
#include "NavigationSystem/CSKit_NavModifierComponent.h"

/**
 * @brief 
 */
void UCSKit_NavModifierComponent::CalcAndCacheBounds() const
{
	if (mbUseManualBoundsExtent)
	{
		SetupManualCacheBounds();
	}
	else
	{
		Super::CalcAndCacheBounds();
	}

	if (!mOffsetBoundsExtent.IsZero())
	{
		OffsetBoundsExtent();
	}
	if (!mOffsetBoundsPos.IsZero())
	{
		OffsetBoundsPos();
	}
}

/**
 * @brief 
 */
void UCSKit_NavModifierComponent::SetupManualCacheBounds() const
{
	const AActor* MyOwner = GetOwner();
	if (MyOwner == nullptr)
	{
		return;
	}
	CachedTransform = MyOwner->GetActorTransform();

	Bounds = FBox::BuildAABB(MyOwner->GetActorLocation(), FailsafeExtent);

	ComponentBounds.Reset();
	ComponentBounds.Add(FRotatedBox(Bounds, MyOwner->GetActorQuat()));
	
	for (int32 Idx = 0; Idx < ComponentBounds.Num(); Idx++)
	{
		const FVector BoxOrigin = ComponentBounds[Idx].Box.GetCenter();
		const FVector BoxExtent = ComponentBounds[Idx].Box.GetExtent();

		const FVector NavModBoxOrigin = FTransform(ComponentBounds[Idx].Quat).InverseTransformPosition(BoxOrigin);
		ComponentBounds[Idx].Box = FBox::BuildAABB(NavModBoxOrigin, BoxExtent);
	}
}

/**
 * @brief 
 */
void UCSKit_NavModifierComponent::OffsetBoundsExtent() const
{
	OffsetBoundsExtent(Bounds, mOffsetBoundsExtent);
	for (int32 Idx = 0; Idx < ComponentBounds.Num(); Idx++)
	{
		OffsetBoundsExtent(ComponentBounds[Idx].Box, mOffsetBoundsExtent);
	}
}

/**
 * @brief 
 */
void UCSKit_NavModifierComponent::OffsetBoundsExtent(FBox& InBox, const FVector& InExtent)
{
	FVector Pos;
	FVector Extent;
	InBox.GetCenterAndExtents(Pos, Extent);

	Extent.X  = FMath::Max(Extent.X + InExtent.X, 0.0f);
	Extent.Y  = FMath::Max(Extent.Y + InExtent.Y, 0.0f);
	Extent.Z  = FMath::Max(Extent.Z + InExtent.Z, 0.0f);

	InBox = FBox::BuildAABB(Pos, Extent);
}

/**
 * @brief 
 */
void UCSKit_NavModifierComponent::OffsetBoundsPos() const
{
	Bounds = Bounds.ShiftBy(mOffsetBoundsPos);
	for (int32 Idx = 0; Idx < ComponentBounds.Num(); Idx++)
	{
		ComponentBounds[Idx].Box = ComponentBounds[Idx].Box.ShiftBy(mOffsetBoundsPos);
	}
}
