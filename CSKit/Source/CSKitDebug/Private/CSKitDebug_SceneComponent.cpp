// Copyright 2020 megasuraman
/**
 * @file CSKitDebug_SceneComponent.cpp
 * @brief AttachEventを親Actorが登録できるようにした版
 * @author megasuraman
 * @date 2025/03/29
 */
#include "CSKitDebug_SceneComponent.h"

UCSKitDebug_SceneComponent::UCSKitDebug_SceneComponent(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
}

void UCSKitDebug_SceneComponent::OnAttachmentChanged()
{
	Super::OnAttachmentChanged();
	
#if WITH_EDITOR
	if(const UWorld* World = GetWorld())
	{
		if(World->IsEditorWorld())
		{
			mEditorOnAttachedDelegate.ExecuteIfBound();
		}
	}
#endif
}

void UCSKitDebug_SceneComponent::OnChildAttached(USceneComponent* ChildComponent)
{
	Super::OnChildAttached(ChildComponent);
#if WITH_EDITOR
	if(const UWorld* World = GetWorld())
	{
		if(World->IsEditorWorld())
		{
			mEditorOnAddChildDelegate.ExecuteIfBound(ChildComponent);
		}
	}
#endif
}
