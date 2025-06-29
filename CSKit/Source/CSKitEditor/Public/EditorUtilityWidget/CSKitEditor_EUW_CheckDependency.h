// Copyright 2020 megasuraman
/**
 * @file CSKitEditor_EUW_CheckDependency.h
 * @brief 依存アセットのリストアップ機能
 * @author megasuraman
 * @date 2025/06/29
 */
#pragma once

#include "CoreMinimal.h"
#include "CSKitEditor_EUW_Base.h"
#include "CSKitEditor_EUW_CheckDependency.generated.h"

//参照アセットと、参照ルート情報
USTRUCT(BlueprintType)
struct CSKITEDITOR_API FCSKitEditor_DependencyResult
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, Category = "CSKitEditor_DependencyResult", Meta = (DisplayName = "対象オブジェクト", DisplayPriority = 1))
	TSoftObjectPtr<UObject> mObject;
	UPROPERTY(EditAnywhere, Category = "CSKitEditor_DependencyResult", Meta = (DisplayName = "参照経路", DisplayPriority = 1))
	TArray<TSoftObjectPtr<UObject>> mRootObject;

	TArray<FString> mRootPath;
};
//依存アセット結果
USTRUCT(BlueprintType)
struct CSKITEDITOR_API FCSKitEditor_DependencyResultList
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, Category = "CSKitEditor_DependencyResult", Meta = (DisplayName = "対象リスト", DisplayPriority = 1))
	TArray<FCSKitEditor_DependencyResult> mList;
};

UCLASS()
class CSKITEDITOR_API UCSKitEditor_EUW_CheckDependency : public UCSKitEditor_EUW_Base
{
	GENERATED_BODY()

public:
	UCSKitEditor_EUW_CheckDependency(const FObjectInitializer& ObjectInitializer);
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
	
	UFUNCTION(BlueprintCallable, Category = "CSKitEditor_EUW_CheckDependency")
	FString GetDependencyInfo() const;
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "CSKitEditor_EUW_CheckDependency")
	void OnPostEditChangePropertyBP();

protected:
	bool IsHitFilter(const FString& InString) const;
	void UpdateDependencyResultList();
	void UpdateDependencyResultListByFolder(const FName& InPath);
	void UpdateDependencyResultListByAsset(const UObject* InTarget);
	void OutputResultFile() const;

protected:
	//例： /Game/45_Action/Enemy
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CSKitEditor_EUW_CheckDependency", Meta = (DisplayName = "チェック対象(フォルダ)", DisplayPriority = 1))
	FName mTargetFolderPath;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CSKitEditor_EUW_CheckDependency", Meta = (DisplayName = "チェック対象(単体)", DisplayPriority = 1))
	UObject* mTarget = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CSKitEditor_EUW_CheckDependency", Meta = (DisplayName = "フィルタ文字列", DisplayPriority = 1))
	TArray<FString> mFilterList;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CSKitEditor_EUW_CheckDependency", Meta = (DisplayName = "チェック結果", DisplayPriority = 1))
	TMap<TSoftObjectPtr<UObject>, FCSKitEditor_DependencyResultList> mDependencyResultMap;

};