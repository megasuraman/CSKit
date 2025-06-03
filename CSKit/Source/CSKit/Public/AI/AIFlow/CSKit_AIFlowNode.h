// Copyright 2020 megasuraman.
/**
 * @file CSKit_AIFlow.h
 * @brief 配置での行動指定用のノード
 * @author megasuraman
 * @date 2025/05/05
 */
#pragma once

#include "CoreMinimal.h"
#include "CSKit_DataTableRowSelector.h"
#include "GameFramework/Actor.h"
#include "CSKit_AIFlowNode.generated.h"

class ACSKit_AIFlowNode;

USTRUCT(BlueprintType)
struct CSKIT_API FCSKit_AIFlowNodeData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(VisibleAnywhere)
	FVector mPos = FVector::ZeroVector;
	UPROPERTY(VisibleAnywhere)
	TArray<int32> mLinkIndexList;
	UPROPERTY(VisibleAnywhere)
	FName mActionName;
	UPROPERTY(VisibleAnywhere)
	int32 mIndex = INDEX_NONE;
	UPROPERTY(VisibleAnywhere)
	float mWaitTimeAfterAction = 0.f;
#if WITH_EDITORONLY_DATA
	UPROPERTY(VisibleAnywhere)
	TSoftObjectPtr<ACSKit_AIFlowNode> mEditorOriginalAIFlowNode;
	//自動生成した際のオリジナルNode
	UPROPERTY(VisibleAnywhere)
	int32 mEditorGenerateRouteParentIndex = INDEX_NONE;
	//自動生成した際に自分をオリジナルにしたNode
	UPROPERTY(VisibleAnywhere)
	int32 mEditorGenerateRouteChildIndex = INDEX_NONE;
	//自動生成かどうか
	UPROPERTY(VisibleAnywhere)
	bool mbEditorGeneratedByRoute = false;
#endif
};

UCLASS(Blueprintable, BlueprintType, HideCategories = (Rendering,Replication,Collision,Input,Actor,LOD,Cooking))
class CSKIT_API ACSKit_AIFlowNode : public AActor
{
	GENERATED_BODY()
	
public:
	ACSKit_AIFlowNode(const FObjectInitializer& ObjectInitializer);
#if ENGINE_MAJOR_VERSION == 5
	virtual void PreSave(FObjectPreSaveContext ObjectSaveContext) override;
#endif
	
	const TArray<TSoftObjectPtr<ACSKit_AIFlowNode>>& GetLinkList() const{return mLinkList;}

protected:
	virtual void PostInitProperties() override;
	virtual void PostLoad() override;

protected:
	UPROPERTY(EditInstanceOnly, Category = "AIFlowNode", meta=(DisplayName = "Linkリスト"))
	TArray<TSoftObjectPtr<ACSKit_AIFlowNode>> mLinkList;
	UPROPERTY(EditInstanceOnly, Category = "AIFlowNode", meta=(DisplayName = "アクション"))
	FCSKit_DataTableRowSelector mActionRowSelector;
	UPROPERTY(EditInstanceOnly, Category = "AIFlowNode", meta=(DisplayName = "アクション", GetOptions="EditorGetActionNameList"))
	FName mActionName;
	UPROPERTY(EditInstanceOnly, Category = "AIFlowNode", meta=(DisplayName = "アクション後待機時間"))
	float mWaitTimeAfterAction = 0.f;
	
#if WITH_EDITOR
public:
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual void PostEditMove(bool bFinished) override;
	virtual void PostEditImport() override;
	
	UFUNCTION(CallInEditor, Category = "AIFlowNode")
	void EditorAddNextNode();
	//mActionNameのプロパティコンボボックス化用
	UFUNCTION()
	virtual TArray<FName> EditorGetActionNameList();

	virtual FString EditorCheckError() const;
	void EditorGetAIFlowNodeData(FCSKit_AIFlowNodeData& OutNode) const;
	void EditorAddLinkNode(const ACSKit_AIFlowNode* InNode);
protected:
	void EditorOnAttached();
private:
	bool mbEditorCalledEditImport = false;
#endif

#if WITH_EDITORONLY_DATA
protected:
	UPROPERTY(BlueprintReadOnly, Category = Display, meta = (AllowPrivateAccess = "true"))
	class UBillboardComponent* SpriteComponent = nullptr;
#endif
};
