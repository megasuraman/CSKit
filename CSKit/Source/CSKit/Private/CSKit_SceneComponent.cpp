// Copyright 2020 megasuraman
/**
 * @file CSKit_SceneComponent.cpp
 * @brief AttachEventを親Actorが登録できるようにした版
 * @author megasuraman
 * @date 2025/03/29
 */
#include "CSKit_SceneComponent.h"

UCSKit_SceneComponent::UCSKit_SceneComponent(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
}

void UCSKit_SceneComponent::OnAttachmentChanged()
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

void UCSKit_SceneComponent::OnChildAttached(USceneComponent* ChildComponent)
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
