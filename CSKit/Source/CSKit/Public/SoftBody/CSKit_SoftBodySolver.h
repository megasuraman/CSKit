// Copyright 2020 megasuraman.
#pragma once

#include "CoreMinimal.h"
#include "CSKit_SoftBodySolver.generated.h"

// バネ挙動用の設定値
USTRUCT(BlueprintType)
struct FCSKit_SoftBodySpringSetting
{
	GENERATED_BODY() 
	
	//元の位置に戻ろうとするバネの強さ
	UPROPERTY(EditAnywhere, Category = "Spring")
	float mShapeStiffness = 50.0f;
	
	// 頂点間のバネの強さ
	UPROPERTY(EditAnywhere, Category = "Spring")
	float mLinkStiffness = 100.0f;
	
	// 体積を維持するための圧力の強さ
	UPROPERTY(EditAnywhere, Category = "Spring")
	float mPressureStiffness = 10.0f;
	
	// 減衰力 大きくすると早く止まる
	UPROPERTY(EditAnywhere, Category = "Spring")
	float mDamping = 5.0f;
	
	// 質量 (m)
	UPROPERTY(EditAnywhere, Category = "Spring")
	float mMass = 1.0f;
};

//頂点情報
struct FCSKit_SoftBodyPoint
{
	FVector mPos = FVector::ZeroVector;
	FVector mOriginalPos = FVector::ZeroVector;
	FVector mLocalPos = FVector::ZeroVector;
	FVector mSpeed = FVector::ZeroVector;
	FVector mForce = FVector::ZeroVector;//蓄積された外力
	bool mbIsPinned = false;
	
	FCSKit_SoftBodyPoint(){}
	FCSKit_SoftBodyPoint( const FVector& InLocalPos, const FTransform& InBaseTransform);
	
};

//頂点のリンク情報
struct FCSKit_SoftBodyPointLink
{
	int32 mIndexA = 0;
	int32 mIndexB = 0;
	float mBaseLength = 0.0f; // バネの自然長
};

//頂点間をバネで結合してSoftBodyとする処理クラス
class CSKit_SoftBodySolver
{
public:
	void SetSpringSetting(const FCSKit_SoftBodySpringSetting& InSpringSetting);
	void SetPointList(const TArray<FCSKit_SoftBodyPoint>& InPoints);
	void SetPointList(const TArray<FCSKit_SoftBodyPoint>& InPoints, const TArray<FCSKit_SoftBodyPointLink>& InLinks);
	void Update(const UWorld* InWorld, const float InDeltaTime);
	void SetBaseTransform(const FTransform& InBaseTransform);
	void AddForce(const FVector& InForce);
	void AddForce(const FVector& InForce, const int32 InPointIndex);
	float CalcVolumeFake() const;
	
protected:
	void UpdatePressureForce();
	void UpdateShapeForce();
	void UpdateLinkForce();
	void UpdatePointPos(const UWorld* InWorld, const float InDeltaTime);
	void ResetForce();
	
private:
	FTransform mBaseTransform;
	TArray<FCSKit_SoftBodyPoint> mPointList;
	TArray<FCSKit_SoftBodyPointLink> mLinkList;
	FCSKit_SoftBodySpringSetting mSpringSetting;
	float mBaseVolume = 0.f;//基準とする体積
	ECollisionChannel mCollisionChannel = ECC_WorldStatic;
	
#if !UE_BUILD_SHIPPING
public:
	void DebugDraw(const UWorld * InWorld, UCanvas* InCanvas) const;
#endif
};