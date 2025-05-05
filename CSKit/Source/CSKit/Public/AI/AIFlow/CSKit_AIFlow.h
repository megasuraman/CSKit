// Copyright 2020 megasuraman.
/**
 * @file CSKit_AIFlow.h
 * @brief 配置での行動指定用Actor
 * @author megasuraman
 * @date 2025/05/05
 */
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CSKit_AIFlowNode.h"
#include "CSKit_AIFlow.generated.h"


UENUM(BlueprintType)
enum class ECSKit_AIFlowGenerateRouteType : uint8
{
	Invalid,
	Loop,
};

UCLASS(Blueprintable, BlueprintType)
class CSKIT_API ACSKit_AIFlow : public AActor
{
	GENERATED_BODY()
	
public:
	ACSKit_AIFlow(const FObjectInitializer& ObjectInitializer);
#if ENGINE_MAJOR_VERSION == 5
	virtual void PreSave(FObjectPreSaveContext ObjectSaveContext) override;
#endif

	const TArray<FCSKit_AIFlowNodeData>& GetNodeDataList() const{return mNodeDataList;}
	const FCSKit_AIFlowNodeData* GetNodeData(const int32 InIndex) const;

protected:
	virtual void PostInitProperties() override;
	virtual void PostLoad() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void BeginDestroy() override;

private:
	UPROPERTY(VisibleAnywhere)
	TArray<FCSKit_AIFlowNodeData> mNodeDataList;

#if USE_CSKIT_DEBUG
public:
	void DebugDrawCall(UCanvas* InCanvas) const;
protected:
	void DebugRequestDraw(const bool bInActive);
	void DebugDraw(UCanvas* InCanvas, class APlayerController* InPlayerController);
	void DebugDrawInfo(UCanvas* InCanvas) const;
	void DebugDrawNodeList(UCanvas* InCanvas) const;
	bool DebugIsWishDraw() const;
private:
	FDelegateHandle mDebugDrawHandle;
	mutable float mDebugDrawSec = 0.f;
	mutable float mDebugDrawRatio = 0.f;
#endif

#if WITH_EDITOR
public:
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;

	UFUNCTION(CallInEditor, Category = CSKit_AIFlow)
	void EditorSetupNodeDataListButton();
	
	void EditorSetupNodeDataList();
	void EditorUpdateNodeDataList(const ACSKit_AIFlowNode* InNode);
	const TSoftObjectPtr<ACSKit_AIFlowNode>& EditorGetLastSelectedNode() const{return mEditorLastSelectedNode;}
	bool EditorCalcNodeBoundingSphere(FVector& OutPos, float& OutRadius) const;
	void EditorGenerateRoute();
	void EditorGenerateRoute_Loop();
	void EditorDeleteNodeGeneratedByRoute();
	
protected:
	void EditorCheckError() const;
	void EditorGetNodeActorList(TArray<const ACSKit_AIFlowNode*>& OutList) const;
	static void EditorSortNodeActorList(TArray<const ACSKit_AIFlowNode*>& OutList);
	void EditorSetupNodeActorList(const TArray<const ACSKit_AIFlowNode*>& InList);
	void EditorOnAddChild(USceneComponent* InChild);
	void EditorCheckLastSelectedNode();

private:
	TSoftObjectPtr<ACSKit_AIFlowNode> mEditorLastSelectedNode;
#endif
	
#if WITH_EDITORONLY_DATA
protected:
	UPROPERTY(VisibleDefaultsOnly, Category = CSKit_AIFlow, meta = (AllowPrivateAccess = "true"))
	class UBillboardComponent* SpriteComponent = nullptr;
	UPROPERTY(EditInstanceOnly, Category = CSKit_AIFlow, meta = (DisplayName = "ルート生成", DisplayPriority = 1))
	ECSKit_AIFlowGenerateRouteType mEditorGenerateRouteType = ECSKit_AIFlowGenerateRouteType::Invalid;
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = CSKit_AIFlow, meta = (DisplayName = "全ノード詳細情報表示"))
	bool mbDrawAllNodeInfo = false;

	bool mbEditorRequestSetupNodeDataList = false;
#endif
};
