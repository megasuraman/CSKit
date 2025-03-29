// Copyright 2020 megasuraman
/**
 * @file CSKitEditor_EUW_CommentPoint.cpp
 * @brief CommentPoint操作EUW
 * @author megasuraman
 * @date 2025/03/29
 */
#include "EditorUtilityWidget/CSKitEditor_EUW_CommentPoint.h"

#include "CSKitDebug_CommentPoint.h"
#include "Editor.h"
#include "Kismet/GameplayStatics.h"

void UCSKitEditor_EUW_CommentPoint::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	CollectCommentPointList();
}

void UCSKitEditor_EUW_CommentPoint::DeleteAlreadyReadAllPoint()
{
	TArray<AActor*> ActorList;
	UGameplayStatics::GetAllActorsOfClass(GetWorld_Editor(), ACSKitDebug_CommentPoint::StaticClass(), ActorList);
	for (AActor* Actor : ActorList)
	{
		if(const ACSKitDebug_CommentPoint* CommentPoint = Cast<ACSKitDebug_CommentPoint>(Actor))
		{
			if(CommentPoint->IsAlreadyReadAllComment())
			{
				GetWorld()->DestroyActor(Actor);
			}
		}
	}
	
	CollectCommentPointList();
}

/**
 * @brief	
 */
void UCSKitEditor_EUW_CommentPoint::CollectCommentPointList()
{
	mCommentPointList.Empty();
	const UWorld* World = GetWorld_Editor();
	if (const UWorld* GameWorld = GetWorld_GameServer())
	{
		World = GameWorld;
	}
	const FString UserName = FPlatformProcess::UserName(false);;
	TArray<AActor*> ActorList;
	UGameplayStatics::GetAllActorsOfClass(World, ACSKitDebug_CommentPoint::StaticClass(), ActorList);
	for (const AActor* Actor : ActorList)
	{
		const ACSKitDebug_CommentPoint* CommentPoint = Cast<ACSKitDebug_CommentPoint>(Actor);
		if(CommentPoint == nullptr)
		{
			continue;
		}

		if (CommentPoint->IsDrawAlways())
		{//単体で表示されてるのでスキップ
			continue;
		}

		if(mFilterPriority != ECSKitDebug_CommentPriority::Invalid
			&& CommentPoint->GetPriority() != mFilterPriority)
		{
			continue;
		}

		if(mbLastMessageByOther
			&& CommentPoint->GetLastCommentOwner() == UserName)
		{
			continue;
		}
		
		mCommentPointList.Add(CommentPoint);
	}

	OnChangeCollectCommentPointListBP();
}

/**
 * @brief	
 */
void UCSKitEditor_EUW_CommentPoint::GetCommentPointDisplayStringList(TArray<FString>& OutCommentPointTitleList) const
{
	for (int32 i=0; i<mCommentPointList.Num(); ++i)
	{
		const ACSKitDebug_CommentPoint* CommentPoint = mCommentPointList[i].Get();
		if(CommentPoint == nullptr)
		{
			continue;
		}

		const FString DisplayString = FString::Printf(TEXT("[%d] %s_(%s)"), i, *CommentPoint->GetTitle(), *CommentPoint->GetActorLabel());
		OutCommentPointTitleList.Add(DisplayString);
	}
}

/**
 * @brief	
 */
ACSKitDebug_CommentPoint* UCSKitEditor_EUW_CommentPoint::GetCommentPoint(const int32 InIndex) const
{
	if(InIndex >= 0
		&& InIndex < mCommentPointList.Num())
	{
		return mCommentPointList[InIndex].Get();
	}
	return nullptr;
}

/**
 * @brief	
 */
void UCSKitEditor_EUW_CommentPoint::Draw(UCanvas* InCanvas, APlayerController* InPlayerController)
{
	if(mbNoDraw)
	{
		return;
	}

	for (const TSoftObjectPtr<class ACSKitDebug_CommentPoint>& ObjectPtr : mCommentPointList)
	{
		const ACSKitDebug_CommentPoint* CommentPoint = ObjectPtr.Get();
		if(CommentPoint == nullptr)
		{
			continue;
		}

		CommentPoint->DebugDraw(InCanvas);
	}
}