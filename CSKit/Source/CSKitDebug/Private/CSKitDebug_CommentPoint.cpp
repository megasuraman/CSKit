// Copyright 2020 megasuraman
/**
 * @file CSKitDebug_CommentPoint.cpp
 * @brief 指定位置にコメントを書くActor
 * @author megasuraman
 * @date 2025/03/29
 */
#include "CSKitDebug_CommentPoint.h"

#include "CSKitDebug_SceneComponent.h"
#include "DrawDebugHelpers.h"
#include "Components/BillboardComponent.h"
#include "Debug/DebugDrawService.h"
#include "Engine/Canvas.h"
#include "ScreenWindow/CSKitDebug_ScreenWindowText.h"
#include "UObject/ConstructorHelpers.h"


ACSKitDebug_CommentPoint::ACSKitDebug_CommentPoint(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	bNetLoadOnClient = false;
	bIsEditorOnlyActor = true;

	mOwner = FPlatformProcess::UserName(false);
	
	UCSKitDebug_SceneComponent* CSKitDebugSceneComponent = ObjectInitializer.CreateDefaultSubobject<UCSKitDebug_SceneComponent>(this, TEXT("SceneComp"));
	RootComponent = CSKitDebugSceneComponent;
	RootComponent->Mobility = EComponentMobility::Static;
#if WITH_EDITOR
	const auto ChangedDelegate = FCSKitDebug_OnAttachedDelegate::CreateUObject(this, &ACSKitDebug_CommentPoint::EditorOnAttached);
	CSKitDebugSceneComponent->EditorSetOnAttachedDelegate(ChangedDelegate);
#endif
	
#if WITH_EDITORONLY_DATA
	SpriteComponent = CreateEditorOnlyDefaultSubobject<UBillboardComponent>(TEXT("Sprite"));

	if (!IsRunningCommandlet())
	{
		// Structure to hold one-time initialization
		struct FConstructorStatics
		{
			ConstructorHelpers::FObjectFinderOptional<UTexture2D> TargetIconSpawnObject;
			ConstructorHelpers::FObjectFinderOptional<UTexture2D> TargetIconObject;
			FName ID_TargetPoint;
			FText NAME_TargetPoint;
			FConstructorStatics()
				: TargetIconSpawnObject(TEXT("/Engine/EditorMaterials/TargetIconSpawn"))
				, TargetIconObject(TEXT("/Engine/EditorResources/S_FTest"))
				, ID_TargetPoint(TEXT("TargetPoint"))
				, NAME_TargetPoint(NSLOCTEXT("SpriteCategory", "TargetPoint", "Target Points"))
			{
			}
		};

		if (SpriteComponent)
		{
			static FConstructorStatics ConstructorStatics;
			SpriteComponent->Sprite = ConstructorStatics.TargetIconObject.Get();
			SpriteComponent->SetRelativeScale3D_Direct(FVector(0.5f, 0.5f, 0.5f));
			SpriteComponent->SpriteInfo.Category = ConstructorStatics.ID_TargetPoint;
			SpriteComponent->SpriteInfo.DisplayName = ConstructorStatics.NAME_TargetPoint;
			SpriteComponent->bIsScreenSizeScaled = true;

			SpriteComponent->SetupAttachment(RootComponent);
		}
	}
#endif
}

void ACSKitDebug_CommentPoint::DebugDraw(UCanvas* InCanvas) const
{
	DebugDraw(InCanvas, nullptr);
}

FString ACSKitDebug_CommentPoint::GetLastCommentOwner() const
{
	if(mCommentDataList.Num() > 0)
	{
		return mCommentDataList[mCommentDataList.Num()-1].mOwner;
	}
	return FString();
}

bool ACSKitDebug_CommentPoint::IsAlreadyReadAllComment() const
{
	for(const FCSKitDebug_CommentData& CommentData : mCommentDataList)
	{
		if(!CommentData.mbAlreadyRead)
		{
			return false;
		}
	}
	return true;
}

void ACSKitDebug_CommentPoint::PostInitProperties()
{
	Super::PostInitProperties();
	if (mbDrawAlways)
	{
		DebugRequestDraw(true);
	}
}

void ACSKitDebug_CommentPoint::PostLoad()
{
	Super::PostLoad();
	
	bIsEditorOnlyActor = true;
}

void ACSKitDebug_CommentPoint::BeginDestroy()
{
	Super::BeginDestroy();
	DebugRequestDraw(false);
}

void ACSKitDebug_CommentPoint::DebugRequestDraw(const bool bInActive)
{
	if (bInActive)
	{
		if (!mDebugDrawHandle.IsValid())
		{
			const auto DebugDrawDelegate = FDebugDrawDelegate::CreateUObject(this, &ACSKitDebug_CommentPoint::DebugDraw);
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

void ACSKitDebug_CommentPoint::DebugDraw(UCanvas* InCanvas, APlayerController* InPlayerController) const
{
#if WITH_EDITOR
	if(IsHiddenEd()
		|| IsHiddenEdAtStartup())
	{
		return;
	}
#endif
	const FVector BasePos = GetActorLocation();
	const FVector BaseProjectPos = InCanvas->Project(BasePos);
	if (BaseProjectPos.X < 0.f
		|| BaseProjectPos.X > InCanvas->SizeX
		|| BaseProjectPos.Y < 0.f
		|| BaseProjectPos.Y > InCanvas->SizeY
		|| BaseProjectPos.Z <= 0.f)
	{
		return;
	}
	const FVector2D BaseScreenPos(BaseProjectPos);
	DrawDebugPoint(GetWorld(), BasePos, 15.f, FColor::Red);
	
	FCSKitDebug_ScreenWindowText ScreenWindowText;
	ScreenWindowText.SetWindowName(mTitle);
	for(const FCSKitDebug_CommentData& CommentData : mCommentDataList)
	{
		if(CommentData.mbAlreadyRead)
		{
			continue;
		}
		ScreenWindowText.AddText(CommentData.mOwner);
		ScreenWindowText.AddText(FString::Printf(TEXT("   %s"), *CommentData.mMessage));
	}

	FLinearColor FrameColor = FLinearColor(0.1f, 0.9f, 0.1f, 1.f);
	switch (mPriority)
	{
	case ECSKitDebug_CommentPriority::High:
		FrameColor = FLinearColor(0.9f, 0.1f, 0.1f, 1.f);
		break;
	case ECSKitDebug_CommentPriority::Low:
		FrameColor = FLinearColor(0.1f, 0.1f, 0.9f, 1.f);
		break;
	default:
		break;
	}
	ScreenWindowText.SetWindowFrameColor(FrameColor);
	ScreenWindowText.SetWindowExtent(ScreenWindowText.GetWindowExtent()*FVector2D(1.7f, 1.f));
	if(!ScreenWindowText.Draw(InCanvas, GetActorLocation()).IsZero())
	{
		DrawDebugCanvas2DLine(InCanvas, BaseScreenPos, BaseScreenPos, FrameColor);	
	}
}

#if WITH_EDITOR
bool ACSKitDebug_CommentPoint::CanEditChange(const FProperty* InProperty) const
{
	if( !Super::CanEditChange(InProperty) )
	{
		return false;
	}

	//作成者本人以外編集不可
	if(InProperty->GetName() == FString(TEXT("mTitle"))
		|| InProperty->GetName() == FString(TEXT("mPriority")))
	{
		return (mOwner == FPlatformProcess::UserName(false));
	}
	return true;
}

void ACSKitDebug_CommentPoint::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if(PropertyChangedEvent.GetPropertyName() == FName(TEXT("mCommentDataList")))
	{
		for(FCSKitDebug_CommentData& CommentData : mCommentDataList)
		{
			if(CommentData.mOwner.IsEmpty())
			{
				CommentData.mOwner = FPlatformProcess::UserName(false);;
			}
		}
	}
	DebugRequestDraw(mbDrawAlways);
}

void ACSKitDebug_CommentPoint::PostEditImport()
{
	Super::PostEditImport();
	mbEditorCalledEditImport = true;
	
	mTitle = FString();
	mOwner = FPlatformProcess::UserName(false);
	mCommentDataList.Empty();
	mPriority = ECSKitDebug_CommentPriority::Middle;
}

void ACSKitDebug_CommentPoint::EditorOnAttached()
{
	if(!mbEditorCalledEditImport)
	{
		return;
	}
	mbEditorCalledEditImport = false;
	mTitle = FString();
	mOwner = FPlatformProcess::UserName(false);
	mCommentDataList.Empty();
	mPriority = ECSKitDebug_CommentPriority::Middle;
}
#endif
