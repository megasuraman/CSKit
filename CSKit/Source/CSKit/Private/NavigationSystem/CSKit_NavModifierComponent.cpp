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
	Bounds.Min -= mOffsetBoundsExtent;
	Bounds.Max += mOffsetBoundsExtent;
	for (int32 Idx = 0; Idx < ComponentBounds.Num(); Idx++)
	{
		ComponentBounds[Idx].Box.Min -= mOffsetBoundsExtent;
		ComponentBounds[Idx].Box.Max += mOffsetBoundsExtent;
	}
}
