// Copyright 2020 megasuraman
/**
* @file CSKitEditor_CheckLevelActors_Foliage.cpp
 * @brief Foliageの不正データチェック
 * @author megasuraman
 * @date 2025/09/20
 */

#include "EditorUtilityWidget/CheckLevelActors/CSKitEditor_CheckLevelActors_Foliage.h"

#include "AITypes.h"
#include "CSKitDebug_Utility.h"
#include "Foliage/Public/InstancedFoliageActor.h"
#include "Foliage/Public/FoliageInstancedStaticMeshComponent.h"

/**
 * @brief	エラーチェック
 */
bool UCSKitEditor_CheckLevelActors_Foliage::CheckErrorActor(
	FCSKitEditor_CheckLevelActors_ErrorData& OutError,
	const ULevel* InLevel, 
	const AActor* InActor
	)
{
	const AInstancedFoliageActor* InstancedFoliageActor = Cast<AInstancedFoliageActor>(InActor);
	if (InstancedFoliageActor == nullptr)
	{
		return false;
	}
	
	const FString LevelName = UCSKitDebug_Utility::GetLevelName(InLevel);
	FCSKitEditor_FoliageInstanceErrorResult& ErrorResult = mErrorResult.FindOrAdd(LevelName);
	
	bool bOwnError = false;
	// TArray<UFoliageInstancedStaticMeshComponent*> Components;
	// InActor->GetComponents<UFoliageInstancedStaticMeshComponent*>(Components);
	// for(const UFoliageInstancedStaticMeshComponent* Component : Components)
	for (const UActorComponent* ActorComponent : InActor->GetComponents())
	{
		const UFoliageInstancedStaticMeshComponent* Component = Cast<UFoliageInstancedStaticMeshComponent>(ActorComponent);
		if (Component == nullptr)
		{
			continue;
		}
		FCSKitEditor_FoliageInstanceError& FoliageInstanceError = ErrorResult.mErrorMap.FindOrAdd(Component->GetName());
		for(int32 i=0; i<Component->PerInstanceSMData.Num(); ++i)
		{
			const FInstancedStaticMeshInstanceData& Data = Component->PerInstanceSMData[i];
			if(Data.Transform.ContainsNaN())
			{
				bOwnError = true;
				FCSKitEditor_FoliageInstanceErrorNode ErrorNode;
				ErrorNode.mPos = FAISystem::InvalidLocation;
				ErrorNode.mErrorString = FString(TEXT("座標情報が不正"));
				ErrorNode.mIndex = i;
				FoliageInstanceError.mErrorNodeList.Add(ErrorNode);
				continue;
			}
		}
	}
	if (bOwnError)
	{
		FCSKitEditor_CheckLevelActors_ErrorDataNode ErrorData(*InActor);
		ErrorData.mErrorString = FString(TEXT("Foliageにエラーがある(詳細はEUW_CheckLevelActorsを参照)"));
		OutError.mList.AddUnique(ErrorData);
	}
	return bOwnError;
}