// Copyright 2020 megasuraman.
/**
 * @file CSKit_AIFlow.cpp
 * @brief 配置での行動指定用のノード
 * @author megasuraman
 * @date 2025/05/05
 */
#include "AI/AIFlow/CSKit_AIFlowNode.h"

#include "CSKit_SceneComponent.h"
#include "AI/AIFlow/CSKit_AIFlow.h"
#include "AI/AIFlow/CSKit_AIFlowDataTable.h"
#include "Components/BillboardComponent.h"

#if USE_CSKIT_DEBUG
#include "CSKitDebug_Utility.h"
#endif

ACSKit_AIFlowNode::ACSKit_AIFlowNode(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	UCSKit_SceneComponent* CSKitSceneComponent = ObjectInitializer.CreateDefaultSubobject<UCSKit_SceneComponent>(this, TEXT("SceneComponent"));
	RootComponent = CSKitSceneComponent;
	RootComponent->Mobility = EComponentMobility::Static;
#if WITH_EDITOR
	const auto ChangedDelegate = FCSKit_OnAttachedDelegate::CreateUObject(this, &ACSKit_AIFlowNode::EditorOnAttached);
	CSKitSceneComponent->EditorSetOnAttachedDelegate(ChangedDelegate);

	mActionRowSelector.mDataTableStruct = FCSKit_AIFlowActionTableRow::StaticStruct();
	// const UCSKit_Config* CSKitConfig = GetDefault<UCSKit_Config>();
	// if (UDataTable* DataTable = Cast<UDataTable>(CSKitConfig->mAIFlowNodeActionDataTablePath.LoadSynchronous()))
	// {
	mActionRowSelector.mDisplayName = FString(TEXT("アクション"));
#endif
	
	bNetLoadOnClient = false;
	bIsEditorOnlyActor = true;
	
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
				, TargetIconObject(TEXT("/Engine/EditorResources/EmptyActor"))
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

#if ENGINE_MAJOR_VERSION == 5
/**
 * @brief 
 */
void ACSKit_AIFlowNode::PreSave(FObjectPreSaveContext ObjectSaveContext)
{
	Super::PreSave(ObjectSaveContext);
}
#endif

/**
 * @brief 
 */
void ACSKit_AIFlowNode::PostInitProperties()
{
	Super::PostInitProperties();
}

void ACSKit_AIFlowNode::PostLoad()
{
	Super::PostLoad();
}

#if WITH_EDITOR
/**
 * @brief 
 */
void ACSKit_AIFlowNode::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	for(int32 i=0; i<mLinkList.Num(); ++i)
	{
		if(mLinkList[i].Get() == this)
		{
			mLinkList.RemoveAtSwap(i);
			break;
		}
	}

	if(ACSKit_AIFlow* AIFlow = Cast<ACSKit_AIFlow>(GetAttachParentActor()))
	{
		AIFlow->EditorSetupNodeDataList();
	}
}
/**
 * @brief 
 */
void ACSKit_AIFlowNode::PostEditMove(bool bFinished)
{
	Super::PostEditMove(bFinished);

	if(ACSKit_AIFlow* AIFlow = Cast<ACSKit_AIFlow>(GetAttachParentActor()))
	{
		AIFlow->EditorUpdateNodeDataList(this);
	}
}

/**
 * @brief 
 */
void ACSKit_AIFlowNode::PostEditImport()
{
	Super::PostEditImport();

	mbEditorCalledEditImport = true;
}

/**
 * @brief 
 */
void ACSKit_AIFlowNode::EditorAddNextNode()
{
	AActor* AIFlowActor = GetAttachParentActor();
	if (AIFlowActor == nullptr)
	{
		return;
	}
	FTransform SpawnTransform = GetActorTransform();
	SpawnTransform.SetLocation(SpawnTransform.GetLocation() + SpawnTransform.GetRotation().RotateVector(FVector(100.f,0.f,0.f)));
	FActorSpawnParameters SpawnParam;
	SpawnParam.OverrideLevel = AIFlowActor->GetLevel();
	if (ACSKit_AIFlowNode* AIFlowNode = GetWorld()->SpawnActor<ACSKit_AIFlowNode>(StaticClass(), SpawnTransform, SpawnParam))
	{
		const FAttachmentTransformRules Rules(EAttachmentRule::KeepWorld, false);
		AIFlowNode->AttachToActor(AIFlowActor, Rules);
		AIFlowNode->SetActorLabel(FString::Printf(TEXT("%s_%d"), *GetActorLabel(), mLinkList.Num()+1));
		EditorAddLinkNode(AIFlowNode);
#if WITH_EDITOR
		//AIFlowNode->SetFolderPath(*mEditorSpawnFolderPath);
#endif
	}
}
/**
 * @brief 
 */
TArray<FName> ACSKit_AIFlowNode::EditorGetActionNameList()
{
	TArray<FName> NameList;
	return NameList;
}

/**
 * @brief 
 */
FString ACSKit_AIFlowNode::EditorCheckError() const
{
	FString ErrorInfo;
	if(Cast<ACSKit_AIFlow>(GetAttachParentActor()) == nullptr)
	{
		ErrorInfo += FString(TEXT("AIFlowの子になってない\n"));
	}
	
	{
		const FString ErrorGround = UCSKitDebug_Utility::CheckFitGround(
			GetWorld(),
		GetActorLocation(),
			ECC_GameTraceChannel4
			);
		if(!ErrorGround.IsEmpty())
		{
			ErrorInfo += ErrorGround;
			ErrorInfo += FString::Printf(TEXT("\n"));	
		}
	}
	if(!UCSKitDebug_Utility::IsTouchNavmesh(GetWorld(),GetActorLocation()))
	{
		ErrorInfo += FString::Printf(TEXT("Navmesh上にない\n"));	
	}
	return ErrorInfo;
}

/**
 * @brief 
 */
void ACSKit_AIFlowNode::EditorGetAIFlowNodeData(FCSKit_AIFlowNodeData& OutNode) const
{
	if(const ACSKit_AIFlow* AIFlow = Cast<ACSKit_AIFlow>(GetAttachParentActor()))
	{
		OutNode.mPos = AIFlow->GetTransform().InverseTransformPosition(GetActorLocation());
	}
	else
	{
		OutNode.mPos = GetActorLocation();
	}
	OutNode.mActionName = mActionRowSelector.mRowName;
	OutNode.mWaitTimeAfterAction = mWaitTimeAfterAction;
}
/**
 * @brief 
 */
void ACSKit_AIFlowNode::EditorAddLinkNode(const ACSKit_AIFlowNode* InNode)
{
	if(InNode == nullptr
		|| InNode == this)
	{
		return;
	}
	mLinkList.AddUnique(TSoftObjectPtr<ACSKit_AIFlowNode>(FSoftObjectPath(InNode)));
}

/**
 * @brief 
 */
void ACSKit_AIFlowNode::EditorOnAttached()
{
#if ENGINE_MAJOR_VERSION == 5
	if(!mbEditorCalledEditImport)
	{
		return;
	}
	mbEditorCalledEditImport = false;
	
	mLinkList.Empty();
	mActionRowSelector.mRowName = FName();
	mActionName = FName();
	
	const ACSKit_AIFlow* AIFlow = Cast<ACSKit_AIFlow>(GetAttachParentActor());
	if(AIFlow == nullptr)
	{
		return;
	}
	if(ACSKit_AIFlowNode* LastSelectedNode = AIFlow->EditorGetLastSelectedNode().Get())
	{
		LastSelectedNode->EditorAddLinkNode(this);
	}
#endif
}
#endif
