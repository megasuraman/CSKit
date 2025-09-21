// Copyright 2020 megasuraman
/**
 * @file CSKitEditor_CheckLevelActors_Foliage.h
 * @brief Foliageの不正データチェック
 * @author megasuraman
 * @date 2025/09/20
 */

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget/CheckLevelActors/CSKitEditor_CheckLevelActors_Base.h"
#include "CSKitEditor_CheckLevelActors_Foliage.generated.h"

USTRUCT(BlueprintType)
struct CSKITEDITOR_API FCSKitEditor_FoliageInstanceErrorNode
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FoliageInstanceDataError", meta = (DisplayName = "座標", DisplayPriority = 3))
	FVector mPos = FVector::ZeroVector;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FoliageInstanceDataError", meta = (DisplayName = "エラー内容", DisplayPriority = 1))
	FString mErrorString;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FoliageInstanceDataError", meta = (DisplayName = "InstancesのIndex", DisplayPriority = 2))
	int32 mIndex = INDEX_NONE;
};
USTRUCT(BlueprintType)
struct CSKITEDITOR_API FCSKitEditor_FoliageInstanceError
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FoliageInstanceDataError", meta = (DisplayName = "エラーリスト", DisplayPriority = 2))
	TArray<FCSKitEditor_FoliageInstanceErrorNode> mErrorNodeList;
};
USTRUCT(BlueprintType)
struct CSKITEDITOR_API FCSKitEditor_FoliageInstanceErrorResult
{
	GENERATED_BODY()

	//Key:Component名, Value:エラー情報
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FoliageInstanceDataError", meta = (DisplayName = "エラー", DisplayPriority = 2))
	TMap<FString,FCSKitEditor_FoliageInstanceError> mErrorMap;
};

UCLASS(BlueprintType)
class CSKITEDITOR_API UCSKitEditor_CheckLevelActors_Foliage : public UCSKitEditor_CheckLevelActors_Base
{
	GENERATED_BODY()

protected:
	virtual bool CheckErrorActor(FCSKitEditor_CheckLevelActors_ErrorData& OutError, const ULevel* InLevel, const AActor* InActor) override;

protected:
	//Key:レベル名, Value:エラー情報
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CheckLevelActors_Foliage", meta = (DisplayName = "エラー詳細", DisplayPriority = 1))
	TMap<FString,FCSKitEditor_FoliageInstanceErrorResult> mErrorResult;
};