// Copyright 2020 megasuraman
/**
 * @file CSKitEditor_CheckLevelActors_InvalidClass.h
 * @brief 許可してない配置Actorがないかチェック
 * @author megasuraman
 * @date 2025/09/13
 */

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget/CheckLevelActors/CSKitEditor_CheckLevelActors_Base.h"
#include "CSKitEditor_CheckLevelActors_InvalidClass.generated.h"


USTRUCT(BlueprintType)
struct CSKITEDITOR_API FCSKitEditor_CheckLevelActors_ClassList
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CheckLevelActors_ClassList", meta = (DisplayName = "ClassList", DisplayPriority=1, AllowAbstract))
	TArray<TSoftClassPtr<AActor>> mClassList;
};

//許可してない配置Actorがないかチェック
UCLASS(BlueprintType)
class CSKITEDITOR_API UCSKitEditor_CheckLevelActors_InvalidClass : public UCSKitEditor_CheckLevelActors_Base
{
	GENERATED_BODY()

protected:
	virtual bool CheckErrorActor(FCSKitEditor_CheckLevelActors_ErrorData& OutError, const ULevel* InLevel, const AActor* InActor) override;

	const FCSKitEditor_CheckLevelActors_ClassList* FindPossibleToPlaceActorClassList(const FString& InLevelName) const;

	//Key:Level名(*のワイルドカードあり), Value:配置OKなクラス
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CheckLevelActors_InvalidClass")
	TMap<FString, FCSKitEditor_CheckLevelActors_ClassList> mPossibleToPlaceActorClassMap;
};