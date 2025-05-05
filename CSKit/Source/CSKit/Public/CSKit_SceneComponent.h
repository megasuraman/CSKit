// Copyright 2020 megasuraman
/**
 * @file CSKit_SceneComponent.h
 * @brief AttachEventを親Actorが登録できるようにした版
 * @author megasuraman
 * @date 2025/03/29
 */
#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "CSKit_SceneComponent.generated.h"

DECLARE_DELEGATE(FCSKit_OnAttachedDelegate)
DECLARE_DELEGATE_OneParam(FCSKit_OnAddChildDelegate, USceneComponent*)

UCLASS()
class CSKIT_API UCSKit_SceneComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	UCSKit_SceneComponent(const FObjectInitializer& ObjectInitializer);
	
	virtual void OnAttachmentChanged() override;
	virtual void OnChildAttached(USceneComponent* ChildComponent) override;

#if WITH_EDITOR
public:
	void EditorSetOnAttachedDelegate(const FCSKit_OnAttachedDelegate& InDelegate){mEditorOnAttachedDelegate = InDelegate;}
	void EditorSetOnAddChildDelegate(const FCSKit_OnAddChildDelegate& InDelegate){mEditorOnAddChildDelegate = InDelegate;}
private:
	FCSKit_OnAttachedDelegate mEditorOnAttachedDelegate;
	FCSKit_OnAddChildDelegate mEditorOnAddChildDelegate;
#endif
};
