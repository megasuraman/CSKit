// Copyright 2020 megasuraman
/**
 * @file CSKitDebug_SceneComponent.h
 * @brief AttachEventを親Actorが登録できるようにした版
 * @author megasuraman
 * @date 2025/03/29
 */
#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "CSKitDebug_SceneComponent.generated.h"

DECLARE_DELEGATE(FCSKitDebug_OnAttachedDelegate)
DECLARE_DELEGATE_OneParam(FCSKitDebug_OnAddChildDelegate, USceneComponent*)

UCLASS()
class CSKITDEBUG_API UCSKitDebug_SceneComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	UCSKitDebug_SceneComponent(const FObjectInitializer& ObjectInitializer);
	
	virtual void OnAttachmentChanged() override;
	virtual void OnChildAttached(USceneComponent* ChildComponent) override;

#if WITH_EDITOR
public:
	void EditorSetOnAttachedDelegate(const FCSKitDebug_OnAttachedDelegate& InDelegate){mEditorOnAttachedDelegate = InDelegate;}
	void EditorSetOnAddChildDelegate(const FCSKitDebug_OnAddChildDelegate& InDelegate){mEditorOnAddChildDelegate = InDelegate;}
private:
	FCSKitDebug_OnAttachedDelegate mEditorOnAttachedDelegate;
	FCSKitDebug_OnAddChildDelegate mEditorOnAddChildDelegate;
#endif
};
