// Copyright 2020 megasuraman
/**
 * @file CSKitDebug_GhostController.cpp
 * @brief 任意のActorを間接的に操作可能にするController
 * @author megasuraman
 * @date 2025/03/30
 */
#include "CSKitDebug_GhostController.h"

#include "CSKitDebug_Config.h"
#include "CSKitDebug_GhostPawn.h"
#include "Debug/DebugDrawService.h"


ACSKitDebug_GhostController::ACSKitDebug_GhostController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetHidden(false);
#if WITH_EDITORONLY_DATA
	bHiddenEd = false;
#endif // WITH_EDITORONLY_DATA
	PrimaryActorTick.bTickEvenWhenPaused = true;
	bShouldPerformFullTickWhenPaused = true;
	SetAsLocalPlayerController();

	if(const UCSKitDebug_Config* CSKitDebug_Config = GetDefault<UCSKitDebug_Config>())
	{
		CheatClass = CSKitDebug_Config->mCheatClass;
	}
}

void ACSKitDebug_GhostController::AddCheats(bool bForce)
{
#if UE_BUILD_SHIPPING
	Super::AddCheats(bForce);
#else
	Super::AddCheats(true);
#endif
}

void ACSKitDebug_GhostController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(const ACSKitDebug_GhostPawn* GhostPawn = Cast<ACSKitDebug_GhostPawn>(GetPawn()))
	{
		if(AActor* TargetActor = GhostPawn->GetAttachParentActor())
		{
			if(const APawn* TargetPawn = Cast<APawn>(TargetActor))
			{
				if(AController* TargetController = TargetPawn->GetController())
				{
					TargetActor = TargetController;
				}
			}
			const FName OriginalFuncName(TEXT("DebugCallGhostInput"));
			FCSKitDebug_GhostInputDelegate GhostInputDelegate;
			GhostInputDelegate.BindUFunction(TargetActor, OriginalFuncName);
			if (GhostInputDelegate.IsBound())
			{
				GhostInputDelegate.Execute(this);
			}
		}
	}
}

void ACSKitDebug_GhostController::BeginPlay()
{
	Super::BeginPlay();
	DebugRequestDraw(true);
}

void ACSKitDebug_GhostController::OnPossess(APawn* aPawn)
{
	Super::OnPossess(aPawn);
	
}

void ACSKitDebug_GhostController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	DebugRequestDraw(false);
}

void ACSKitDebug_GhostController::DebugRequestDraw(const bool bInActive)
{
	if (bInActive)
	{
		if (!mDebugDrawHandle.IsValid())
		{
			const auto DebugDrawDelegate = FDebugDrawDelegate::CreateUObject(this, &ACSKitDebug_GhostController::DebugDraw);
			if (DebugDrawDelegate.IsBound())
			{
				mDebugDrawHandle = UDebugDrawService::Register(TEXT("GameplayDebug"), DebugDrawDelegate);
			}
		}
	}
	else
	{
		if (mDebugDrawHandle.IsValid())
		{
			UDebugDrawService::Unregister(mDebugDrawHandle);
			mDebugDrawHandle.Reset();
		}
	}
}

void ACSKitDebug_GhostController::DebugDraw(UCanvas* InCanvas, APlayerController* InPlayerController) const
{
	if(const ACSKitDebug_GhostPawn* GhostPawn = Cast<ACSKitDebug_GhostPawn>(GetPawn()))
	{
		if(AActor* TargetActor = GhostPawn->GetAttachParentActor())
		{
			if(const APawn* TargetPawn = Cast<APawn>(TargetActor))
			{
				if(AController* TargetController = TargetPawn->GetController())
				{
					TargetActor = TargetController;
				}
			}
			const FName OriginalFuncName(TEXT("DebugCallGhostInputDraw"));
			FCSKitDebug_GhostInputDrawDelegate GhostInputDelegate;
			GhostInputDelegate.BindUFunction(TargetActor, OriginalFuncName);
			if (GhostInputDelegate.IsBound())
			{
				GhostInputDelegate.Execute(InCanvas);
			}
		}
	}
}
