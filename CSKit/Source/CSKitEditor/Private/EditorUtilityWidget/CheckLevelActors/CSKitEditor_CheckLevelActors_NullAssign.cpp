// Copyright 2020 megasuraman
/**
* @file CSKitEditor_CheckLevelActors_NullAssign.cpp
 * @brief アサインのない配置チェック
 * @author megasuraman
 * @date 2025/09/21
 */

#include "EditorUtilityWidget/CheckLevelActors/CSKitEditor_CheckLevelActors_NullAssign.h"

#include "Engine/DecalActor.h"
#include "Engine/LODActor.h"

/**
 * @brief	エラーチェック
 */
bool UCSKitEditor_CheckLevelActors_NullAssign::CheckErrorActor(
	FCSKitEditor_CheckLevelActors_ErrorData& OutError,
	const ULevel* InLevel, 
	const AActor* InActor
	)
{
	if (const ADecalActor* DecalActor = Cast<ADecalActor>(InActor))
	{
		if(DecalActor->GetDecalMaterial() == nullptr)
		{
			FCSKitEditor_CheckLevelActors_ErrorDataNode ErrorData(*InActor);
			ErrorData.mErrorString = FString(TEXT("DecalActor に Material がアサインされていない"));
			OutError.mList.AddUnique(ErrorData);
			return true;
		}
	}
	
	if (InActor->IsA(ALODActor::StaticClass()))
	{
		return false;
	}

	bool bHitError = false;
	// TArray<UStaticMeshComponent*> Components;
	// Actor->GetComponents<UStaticMeshComponent*>(Components);
	// FString ComponentName;
	// for(UStaticMeshComponent* StaticMeshComponent : Components)
	TArray<FString> ErrorComponentList;
	for (const UActorComponent* ActorComponent : InActor->GetComponents())
	{
		const UStaticMeshComponent* StaticMeshComponent = Cast<UStaticMeshComponent>(ActorComponent);
		if (StaticMeshComponent == nullptr)
		{
			continue;
		}
		if(StaticMeshComponent->IsA(UInstancedStaticMeshComponent::StaticClass()))
		{
			continue;
		}
		if(StaticMeshComponent->GetStaticMesh() == nullptr)
		{
			FCSKitEditor_CheckLevelActors_ErrorDataNode ErrorData(*InActor);
			ErrorData.mErrorString = FString::Printf(TEXT("%s にStaticMeshがアサインされてない"), *StaticMeshComponent->GetName());
			OutError.mList.AddUnique(ErrorData);
			bHitError = true;
			continue;
		}
	}
	return bHitError;
}