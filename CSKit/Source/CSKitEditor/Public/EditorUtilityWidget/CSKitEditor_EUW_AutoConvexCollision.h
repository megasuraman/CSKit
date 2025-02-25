// Copyright 2020 megasuraman
/**
 * @file CSKitEditor_EUW_AutoConvexCollision.h
 * @brief AutoConvexCollision操作用のEUW
 * @author megasuraman
 * @date 2025/02/22
 */

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget/CSKitEditor_EUW_Base.h"
#include "CSKitEditor_EUW_AutoConvexCollision.generated.h"
UCLASS()
class CSKITEDITOR_API UCSKitEditor_EUW_AutoConvexCollision : public UCSKitEditor_EUW_Base
{
	GENERATED_BODY()

	struct FStaticMeshInfo
	{
		TWeakObjectPtr<UStaticMesh> mStaticMesh;
		int64 mTimeStamp = 0;
		FString GetOutputString() const;
	};

public:
	virtual bool Initialize() override;
	
	UFUNCTION(BlueprintCallable)
	void LoadTargetFile();
	UFUNCTION(BlueprintCallable)
	void SaveTargetFile();
	UFUNCTION(BlueprintCallable)
	void UpdateTimeStamp(UStaticMesh* InStaticMesh);
	UFUNCTION(BlueprintCallable)
	void AssignTargetListFromTargetFile(bool bInOnlyNewTimeStamp);
	UFUNCTION(BlueprintCallable)
	void AddStaticMeshToTargetFile(UStaticMesh* InStaticMesh);
	UFUNCTION(BlueprintCallable)
	void AddIgnoreStaticMesh(TArray<UStaticMesh*> InList);
	UFUNCTION(BlueprintCallable)
	bool IsIgnoreStaticMesh(UStaticMesh* InStaticMesh) const;

protected:
	void SaveIgnoreFile();
	void LoadIgnoreFile();

protected:
	//コリジョンの分割数
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CSKitEditor_EUW_AutoConvexCollision", meta=(DisplayName = "分割数", UIMin=2, UIMax=64))
	int32 mHullCount = 4;
	//1つの塊を構成する最大頂点数
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CSKitEditor_EUW_AutoConvexCollision", meta=(DisplayName = "最大頂点数", UIMin=6, UIMax=32))
	int32 mMaxHullVerts = 16;
	//コリジョン生成の精度
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CSKitEditor_EUW_AutoConvexCollision", meta=(DisplayName = "ハル精度", UIMin=10000, UIMax=1000000))
	int32 mHullPrecision = 30000;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CSKitEditor_EUW_AutoConvexCollision", meta=(DisplayName = "対象リスト"))
	TArray<UStaticMesh*> mTargetList;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CSKitEditor_EUW_AutoConvexCollision", meta=(DisplayName = "無視リスト"))
	TArray<UStaticMesh*> mIgnoreList;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CSKitEditor_EUW_AutoConvexCollision", meta=(DisplayName = "TargetListFilePath"))
	FString mTargetFilePath = FString(TEXT("Saved/CSKitEditor/AutoConvexCollision/TargetList.txt"));
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CSKitEditor_EUW_AutoConvexCollision", meta=(DisplayName = "IgnoreListFilePath"))
	FString mIgnoreFilePath = FString(TEXT("Saved/CSKitEditor/AutoConvexCollision/IgnoreList.txt"));

private:
	TMap<FString, FStaticMeshInfo> mTargetMap;
};

