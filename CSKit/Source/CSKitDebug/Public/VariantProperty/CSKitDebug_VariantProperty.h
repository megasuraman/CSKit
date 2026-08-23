// Copyright 2020 megasuraman.
#pragma once

#include "CoreMinimal.h"
#include "CSKitDebug_VariantProperty.generated.h"

// 扱える型
UENUM(BlueprintType)
enum class ECSKit_VariantPropertyType : uint8
{
	Invalid,
	Bool,
	Int,
	Float,
	Vector,
	Transform,
	String,
	StringList,
};

//複数の型をまとめて扱うための構造体
//FInstancedPropertyBag だと Blueprint で扱えないため用意
//FInstancedStruct で構成したいところ
USTRUCT(BlueprintType)
struct FCSKit_VariantProperty
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	FString mDataString;
	UPROPERTY(EditAnywhere)
	ECSKit_VariantPropertyType mType = ECSKit_VariantPropertyType::Invalid;
	
	void Set(const bool InValue);
	void Set(const int32 InValue);
	void Set(const float InValue);
	void Set(const FVector& InValue);
	void Set(const FTransform& InValue);
	void Set(const FString& InValue);
	void Set(const TArray<FString>& InList, const int32 InSelectIndex);
	void SetStringListSelectIndex(const int32 InSelectIndex);
	
	bool GetBool() const;
	int32 GetInt() const;
	float GetFloat() const;
	FVector GetVector() const;
	FTransform GetTransform() const;
	FString GetString() const;
	void GetStringList(TArray<FString>& OutList, int32& OutSelectIndex) const;
};
