// Copyright 2020 megasuraman.
/**
 * @file CSKit_TerritoryComponent.cpp
 * @brief 活動範囲管理Component
 * @author megasuraman
 * @date 2025/05/05
 */
#include "AI/Territory/CSKit_TerritoryComponent.h"

#include "AI/CSKit_AIController.h"
#include "AI/Territory/CSKit_TerritoryVolume.h"
#include "CSKit_Subsystem.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"

UCSKit_TerritoryComponent::UCSKit_TerritoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UCSKit_TerritoryComponent::BeginPlay()
{
	Super::BeginPlay();

	UpdateTerritoryVolume();
}

/* ------------------------------------------------------------
   !更新処理(Tickは使わず、CSKit_AIControllerから呼ぶ)
------------------------------------------------------------ */
void UCSKit_TerritoryComponent::Update(const float InDeltaSec)
{
	const ACSKit_TerritoryVolume* Volume = mTerritoryVolume.Get();
	if(Volume == nullptr)
	{
		UpdateTerritoryVolume();
		return;;
	}

	const ACSKit_AIController* AIController = Cast<ACSKit_AIController>(GetOwner());
	if (AIController == nullptr)
	{
		return;
	}
	const APawn* OwnerPawn = AIController->GetPawn();
	if(OwnerPawn == nullptr)
	{
		return;
	}

	const FVector BasePos = OwnerPawn->GetActorLocation();
	mbInside = Volume->IsTouch(BasePos,mCheckRadius);
}

/**
 * @brief 
 */
bool UCSKit_TerritoryComponent::IsOwnTerritoryVolume() const
{
	return mTerritoryVolume.IsValid();
}

/**
 * @brief 
 */
bool UCSKit_TerritoryComponent::IsTouchTerritoryVolume(const FVector& InPos, const float InRadius) const
{
	const ACSKit_TerritoryVolume* Volume = mTerritoryVolume.Get();
	if(Volume == nullptr)
	{
		return false;
	}
	return Volume->IsTouch(InPos,InRadius);
}

/**
 * @brief 
 */
void UCSKit_TerritoryComponent::UpdateTerritoryVolume()
{
	const ACSKit_AIController* AIController = Cast<ACSKit_AIController>(GetOwner());
	const UCSKit_Subsystem* CSKitSubsystem = GetWorld()->GetSubsystem<UCSKit_Subsystem>();
	if (AIController == nullptr
		|| CSKitSubsystem == nullptr)
	{
		return;
	}
	const APawn* OwnerPawn = AIController->GetPawn();
	if(OwnerPawn == nullptr)
	{
		return;
	}

	const FVector BasePos = OwnerPawn->GetActorLocation();
	mTerritoryVolume = CSKitSubsystem->FindTouchTerritoryVolume(BasePos, mCheckRadius, mUserName);
}

#if USE_CSKIT_DEBUG
/* ------------------------------------------------------------
  !ActorWatcher選択時
------------------------------------------------------------ */
FString UCSKit_TerritoryComponent::DebugDrawSelectedActorWatcher(UCanvas* InCanvas) const
{
	if(const ACSKit_TerritoryVolume* Volume = mTerritoryVolume.Get())
	{
		Volume->DebugDraw(InCanvas, false);
	}

	FString DebugInfo;

	DebugInfo += FString(TEXT("[Territory]\n"));
	DebugInfo += FString::Printf(TEXT("   UseKindName : %s\n"), *mUserName.ToString());

	return DebugInfo;
}

#endif