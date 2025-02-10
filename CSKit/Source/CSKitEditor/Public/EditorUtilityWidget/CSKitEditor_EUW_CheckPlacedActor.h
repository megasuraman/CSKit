// Copyright 2020 megasuraman
/**
 * @file CSKitEditor_EUW_CheckPlacedActor.h
 * @brief 配置Actorに対するチェック
 * @author megasuraman
 * @date 2024/11/30
 */

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget/CSKitEditor_EUW_Base.h"
#include "CSKitEditor_EUW_CheckPlacedActor.generated.h"

USTRUCT(BlueprintType)
struct CSKITEDITOR_API FCSKitEditor_PlacedActorClassList
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CheckPlacedActor")
	TArray<TSoftClassPtr<AActor>> mClassList;
};

UCLASS()
class CSKITEDITOR_API UCSKitEditor_EUW_CheckPlacedActor : public UCSKitEditor_EUW_Base
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void SetupPlacedActorClassMap();
	UFUNCTION(BlueprintCallable)
	bool CheckError();

protected:
	bool CheckError_PossibleToPlaceActor();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CheckPlacedActor")
	TMap<FString, FCSKitEditor_PlacedActorClassList> mPlacedActorClassMap;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CheckPlacedActor")
	TMap<FString, FCSKitEditor_PlacedActorClassList> mPossibleToPlaceActorClassMap;
};
