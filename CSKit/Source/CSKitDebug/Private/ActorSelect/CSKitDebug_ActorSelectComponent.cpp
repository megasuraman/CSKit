// Copyright 2020 megasuraman
/**
 * @file CSKitDebug_ActorSelectComponent.cpp
 * @brief デバッグ選択対象Component
 * @author megasuraman
 * @date 2020/05/27
 */

#include "ActorSelect/CSKitDebug_ActorSelectComponent.h"
#include "ActorSelect/CSKitDebug_ActorSelectManager.h"
#include "CSKitDebug_Draw.h"

#include "GameFramework/Pawn.h"
#include "AIModule/Classes/AIController.h"

UCSKitDebug_ActorSelectComponent::UCSKitDebug_ActorSelectComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


#if USE_CSKIT_DEBUG
// Called when the game starts
void UCSKitDebug_ActorSelectComponent::BeginPlay()
{
	Super::BeginPlay();

	if (const AActor* OwnerActor = GetOwner())
	{
		mScreenWindow.SetWindowName(FString::Printf(TEXT("%s"), *OwnerActor->GetName()));
	}

#if USE_CSKIT_DEBUG
	UCSKitDebug_ActorSelectManager* ActorSelectManager = UCSKitDebug_ActorSelectManager::sGet(GetWorld());
	ActorSelectManager->EntryDebugSelectComponent(this);
	mManager = ActorSelectManager;
#endif
}

void UCSKitDebug_ActorSelectComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (UCSKitDebug_ActorSelectManager* ActorSelectManager = mManager.Get())
	{
		ActorSelectManager->ExitDebugSelectComponent(this);
	}
}

void UCSKitDebug_ActorSelectComponent::AddText(const FString& InString)
{
	mScreenWindow.AddText(InString);
}

void UCSKitDebug_ActorSelectComponent::SetDrawDelegate(const FCSKitDebug_ActorSelectDrawDelegate& InDelegate)
{
	mDrawDelegate = InDelegate;
}

/**
 * @brief	選択状態設定
 */
void UCSKitDebug_ActorSelectComponent::SetSelect(bool bInSelect)
{
	if (mbSelect == bInSelect)
	{
		return;
	}

	mbSelect = bInSelect;
}

/**
 * @brief	Draw
 */
void	UCSKitDebug_ActorSelectComponent::DebugDraw(UCanvas* InCanvas)
{
	const UCSKitDebug_ActorSelectManager* ActorSelectManager = mManager.Get();
	const AActor* OwnerActor = GetOwner();
	if (ActorSelectManager == nullptr
		|| OwnerActor == nullptr)
	{
		return;
	}

	if (mDrawDelegate.IsBound())
	{
		mScreenWindow.AddText(mDrawDelegate.Execute(InCanvas));
	}

	mScreenWindow.FittingWindowExtent(InCanvas);
	mScreenWindow.SetWindowFrameColor(GetColor());
	mScreenWindow.Draw(InCanvas, OwnerActor->GetActorLocation());

	mScreenWindow.ClearString();

	if (const APawn* Pawn = Cast<APawn>(OwnerActor))
	{
		if (const AAIController* AIController = Cast<AAIController>(Pawn->Controller))
		{
 			if (ActorSelectManager->IsShowPathFollow())
 			{
				UCSKitDebug_Draw::DrawPathFollowRoute(GetWorld(), InCanvas, AIController, true);
 			}
 			if (ActorSelectManager->IsShowLastEQS())
 			{
 				UCSKitDebug_Draw::DrawLastEQS(GetWorld(), InCanvas, AIController);
 			}
		}
	}
}

/**
 * @brief	DrawMark
 */
void	UCSKitDebug_ActorSelectComponent::DrawMark(UCanvas* InCanvas) const
{
	const AActor* OwnerActor = GetOwner();
	if (OwnerActor == nullptr)
	{
		return;
	}
	
	const FVector OwnerPos = OwnerActor->GetActorLocation();
	const FVector2D ExtentV(5.f, 5.f);
	UCSKitDebug_Draw::DrawCanvasQuadrangle(InCanvas, OwnerPos, ExtentV, GetColor());
}
#endif//USE_CSKIT_DEBUG